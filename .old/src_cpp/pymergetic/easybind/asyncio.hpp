#pragma once

#include <nanobind/nanobind.h>

namespace easybind::asyncio {

struct FuturePair {
  nanobind::object loop;
  nanobind::object future;
};

inline FuturePair make_future() {
  nanobind::object loop = nanobind::module_::import_("asyncio").attr("get_running_loop")();
  nanobind::object future = loop.attr("create_future")();
  return {std::move(loop), std::move(future)};
}

template <typename T>
inline void set_result_threadsafe(nanobind::object loop, nanobind::object future, T&& value) {
  nanobind::gil_scoped_acquire _gil;
  nanobind::object call_soon_threadsafe = loop.attr("call_soon_threadsafe");
  call_soon_threadsafe(future.attr("set_result"), nanobind::cast(std::forward<T>(value)));
}

inline void set_exception_threadsafe(nanobind::object loop, nanobind::object future, nanobind::object exc) {
  nanobind::gil_scoped_acquire _gil;
  nanobind::object call_soon_threadsafe = loop.attr("call_soon_threadsafe");
  call_soon_threadsafe(future.attr("set_exception"), std::move(exc));
}

template <typename Fn>
inline nanobind::object future_from(Fn&& fn) {
  FuturePair pair = make_future();
  nanobind::object loop = nanobind::object(pair.loop);
  nanobind::object future = nanobind::object(pair.future);

  auto resolve = [loop = nanobind::object(loop), future = nanobind::object(future)](auto&& value) mutable {
    set_result_threadsafe(loop, future, std::forward<decltype(value)>(value));
  };
  auto reject = [loop = nanobind::object(loop), future = nanobind::object(future)](nanobind::object exc) mutable {
    set_exception_threadsafe(loop, future, std::move(exc));
  };

  fn(std::move(resolve), std::move(reject));
  return future;
}

}  // namespace easybind::asyncio
