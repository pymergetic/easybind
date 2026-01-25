#include <pymergetic/easybind/version/__init__.hpp>

#ifndef EASYBIND_BUILD_VERSION
#define EASYBIND_BUILD_VERSION "0.0.0"
#endif

namespace pymergetic::easybind::version {

std::string get_version() {
  return EASYBIND_BUILD_VERSION;
}

}  // namespace pymergetic::easybind::version
