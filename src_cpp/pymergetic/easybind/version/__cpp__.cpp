#include <pymergetic/easybind/prelude.hpp>
#include <pymergetic/easybind/version/__init__.hpp>

EASYBIND_MODULE("pymergetic.easybind.version");

namespace pymergetic::easybind::version {

EASYBIND_REGISTER_FUNC(get_version);
EASYBIND_REGISTER_ATTR("__version__", get_version());

}  // namespace pymergetic::easybind::version
