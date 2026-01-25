#include <pymergetic/easybind/prelude.hpp>
#include <pymergetic/easybind/version/__init__.hpp>

EASYBIND_SHARED_OBJECT();

EASYBIND_REGISTER_ATTR("__version__", ::pymergetic::easybind::version::get_version());
