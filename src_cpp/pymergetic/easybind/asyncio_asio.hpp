#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>

#include <thread>
#include <type_traits>
#include <utility>

#include <pymergetic/easybind/asyncio.hpp>

namespace easybind::asyncio {

template <typename Fn>
inline nanobind::object future_from_asio_post(Fn&& fn) {
  return future_from([fn = std::forward<Fn>(fn)](auto resolve, auto reject) mutable {
    std::thread([resolve = std::move(resolve), reject = std::move(reject), fn = std::move(fn)]() mutable {
      boost::asio::io_context io(1);
      boost::asio::post(io, [resolve = std::move(resolve), reject = std::move(reject), fn = std::move(fn)]() mutable {
        fn(std::move(resolve), std::move(reject));
      });
      io.run();
    }).detach();
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

}  // namespace easybind::asyncio

// Usage: EASYBIND_REGISTER_ASYNC("pkg.name", "func_name", func);
#define EASYBIND_REGISTER_ASYNC(PACKAGE, NAME, FUNC)                                             \
  EASYBIND_REGISTER_PACKAGE(PACKAGE, [](nanobind::module_& m) {                                  \
    m.def(NAME, &::easybind::asyncio::AsyncReturnWrapper<FUNC>::call);                           \
  })
