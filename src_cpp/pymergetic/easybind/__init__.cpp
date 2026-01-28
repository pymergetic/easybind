#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <pymergetic/easybind/__init__.hpp>


namespace pymergetic::easybind {

  
void __init_bind__(nanobind::module_& m) {

  m.doc() = "pymergetic.easybind module";

  // Mark as package so submodules can be imported.
  pymergetic::easybind::module::set_package_path(m);

}

module::ModuleNode* __init__ =
    module::ModuleNode::create("pymergetic.easybind", __init_bind__, true);


}  // namespace pymergetic::easybind


NB_MODULE(easybind, m) {
  pymergetic::easybind::module::apply_init(pymergetic::easybind::__init__, m);
}
