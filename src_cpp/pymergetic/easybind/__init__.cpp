#include <pymergetic/easybind/bind.hpp>

EASYBIND_NS_MODULE_SHARED_OBJECT(easybind, m, pymergetic::easybind, true, {
  m.doc() = "pymergetic.easybind module";
});
