#pragma once

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>

#include <thread>
#include <type_traits>
#include <utility>

#include <pymergetic/easybind/asyncio.hpp>
#include <pymergetic/easybind/describe.hpp>

namespace easybind::asyncio {

namespace detail {

struct Worker {
  boost::asio::io_context io;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work;
  std::thread thread;

  Worker() : io(1), work(boost::asio::make_work_guard(io)), thread([this]() { io.run(); }) {}

  ~Worker() {
    work.reset();
    io.stop();
    if (thread.joinable()) {
      thread.join();
    }
  }
};

inline Worker& worker() {
  static Worker instance;
  return instance;
}

}  // namespace detail

template <typename Fn>
inline nanobind::object future_from_asio_post(Fn&& fn) {
  return future_from([fn = std::forward<Fn>(fn)](auto resolve, auto reject) mutable {
    auto& io = detail::worker().io;
    boost::asio::post(io, [resolve = std::move(resolve), reject = std::move(reject), fn = std::move(fn)]() mutable {
      fn(std::move(resolve), std::move(reject));
    });
  });
}

template <typename Func, typename... Args>
inline nanobind::object call_asio_return(Func&& func, Args&&... args) {
  return future_from_asio_post([func = std::forward<Func>(func),
                                ... args = std::forward<Args>(args)](auto resolve, auto reject) mutable {
    try {
      if constexpr (std::is_void_v<std::invoke_result_t<Func, Args...>>) {
        func(std::forward<Args>(args)...);
        resolve(nanobind::none());
      } else {
        resolve(func(std::forward<Args>(args)...));
      }
    } catch (const std::exception& e) {
      nanobind::object exc = nanobind::module_::import_("builtins").attr("RuntimeError")(e.what());
      reject(std::move(exc));
    }
  });
}

template <auto Func>
struct AsyncReturnWrapper;

template <typename R, typename... Args, R (*Func)(Args...)>
struct AsyncReturnWrapper<Func> {
  static nanobind::object call(Args... args) {
    return call_asio_return(Func, std::forward<Args>(args)...);
  }
};

template <auto Method>
struct AsyncMethodWrapper;

template <typename C, typename R, typename... Args, R (C::*Method)(Args...)>
struct AsyncMethodWrapper<Method> {
  static nanobind::object call(nanobind::object self_obj, Args... args) {
    C& self = nanobind::cast<C&>(self_obj);
    auto self_ptr = &self;
    return call_asio_return([self_obj = std::move(self_obj), self_ptr](Args... inner) {
      return (self_ptr->*Method)(std::forward<Args>(inner)...);
    }, std::forward<Args>(args)...);
  }
};

template <typename C, typename R, typename... Args, R (C::*Method)(Args...) const>
struct AsyncMethodWrapper<Method> {
  static nanobind::object call(nanobind::object self_obj, Args... args) {
    const C& self = nanobind::cast<const C&>(self_obj);
    auto self_ptr = &self;
    return call_asio_return([self_obj = std::move(self_obj), self_ptr](Args... inner) {
      return (self_ptr->*Method)(std::forward<Args>(inner)...);
    }, std::forward<Args>(args)...);
  }
};

}  // namespace easybind::asyncio

// Usage: EASYBIND_ASYNC_METHOD(Type, method)
#define EASYBIND_ASYNC_METHOD(TYPE, METHOD)                                                       \
  cls.def(#METHOD, &::easybind::asyncio::AsyncMethodWrapper<&TYPE::METHOD>::call);

// Usage: EASYBIND_ASYNC_METHOD_ARGS(Type, method, nb::arg("x") = 1)
#define EASYBIND_ASYNC_METHOD_ARGS(TYPE, METHOD, ...)                                              \
  cls.def(#METHOD, &::easybind::asyncio::AsyncMethodWrapper<&TYPE::METHOD>::call, __VA_ARGS__);
