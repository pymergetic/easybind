#include <nanobind/nanobind.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <pymergetic/easybind/module/__init__.hpp>


//
// Extension-side bindings for easybind.module.
//


namespace pymergetic::easybind::module {


void __init_bind__(nanobind::module_& m) {
  using ModuleNode = pymergetic::easybind::module::ModuleNode;

  nanobind::enum_<ModuleNode::FlagState>(m, "FlagState")
      .value("Unknown", ModuleNode::FlagState::Unknown)
      .value("False", ModuleNode::FlagState::False)
      .value("True", ModuleNode::FlagState::True);

  nanobind::class_<ModuleNode>(m, "ModuleNode")
      .def_prop_ro("name", &ModuleNode::name)
      .def("full_name", &ModuleNode::full_name)
      .def_prop_ro("parent", &ModuleNode::parent, nanobind::rv_policy::reference)
      .def_prop_ro("shared_object_state", &ModuleNode::shared_object_state)
      .def_prop_ro("shared_object", &ModuleNode::is_shared_object)
      .def("children", &ModuleNode::children, nanobind::rv_policy::reference);

  m.def("root", &ModuleNode::root, nanobind::rv_policy::reference);
  m.def("from_full_name",
      &ModuleNode::from,
      nanobind::arg("full_name"),
      nanobind::rv_policy::reference);
  m.def("create",
      [](const std::string& full_name, bool shared_object) {
        return ModuleNode::create(full_name, nullptr, shared_object);
      },
      nanobind::arg("full_name"),
      nanobind::arg("shared_object") = false,
      nanobind::rv_policy::reference);
}


ModuleNode* __init__ =
    ModuleNode::create("pymergetic.easybind.module", __init_bind__, false);
  

void set_package_path(nanobind::module_& m) {
  nanobind::module_ os = nanobind::module_::import_("os");
  nanobind::object dirname = os.attr("path").attr("dirname");
  nanobind::list package_path;
  package_path.append(dirname(m.attr("__file__")));
  m.attr("__path__") = package_path;
}


}  // namespace pymergetic::easybind::module