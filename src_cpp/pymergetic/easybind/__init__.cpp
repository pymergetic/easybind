#include <pymergetic/easybind/bind.hpp>

EASYBIND_NS_MODULE_SHARED_OBJECT(pymergetic::easybind, easybind, m, true, {
  m.doc() = "pymergetic.easybind module";
});
