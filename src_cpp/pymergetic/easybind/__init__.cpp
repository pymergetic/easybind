#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <stdexcept>

#include <pymergetic/easybind/__init__.hpp>


namespace pymergetic::easybind {

  
void set_package_path(nanobind::module_& m) {
  nanobind::module_ os = nanobind::module_::import_("os");
  nanobind::object dirname = os.attr("path").attr("dirname");
  nanobind::list package_path;
  package_path.append(dirname(m.attr("__file__")));
  m.attr("__path__") = package_path;
}

void apply_init(module::ModuleNode* init_node, nanobind::module_& m) {
  pymergetic::easybind::module::ApplyHooks::set(
      pymergetic::easybind::module::make_nanobind_apply_hooks());

  if (init_node) {
    init_node->apply(m);
  } else {
    throw std::runtime_error("easybind module node not initialized");
  }
}

void __init_bind__(nanobind::module_& m) {

  m.doc() = "pymergetic.easybind module";

  // Mark as package so submodules can be imported.
  set_package_path(m);

}

module::ModuleNode* __init__ =
    module::ModuleNode::create("pymergetic.easybind", __init_bind__, true);


}  // namespace pymergetic::easybind


NB_MODULE(easybind, m) {
  pymergetic::easybind::apply_init(pymergetic::easybind::__init__, m);
}
