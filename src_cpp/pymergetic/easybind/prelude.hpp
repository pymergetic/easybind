#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <pymergetic/easybind/asyncio_asio.hpp>
#include <pymergetic/easybind/bind_utils.hpp>

namespace easybind {

inline auto arg(const char* name) {
  return nanobind::arg(name);
}

}  // namespace easybind
