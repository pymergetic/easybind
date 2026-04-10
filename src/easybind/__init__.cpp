#include <easybind/bind.hpp>

EASYBIND_NS_MODULE_SHARED_OBJECT(easybind, easybind, m, true, {
  m.doc() = "easybind module";
});
