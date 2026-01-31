#include <pymergetic/easybind/bind.hpp>


//
// Extension-side bindings for easybind.module.
//


EASYBIND_NS_MODULE(pymergetic::easybind::module, m, false, {
  using ModuleNode = pymergetic::easybind::module::ModuleNode;

  nanobind::enum_<ModuleNode::FlagState>(m, "FlagState")
      .value("Unknown", ModuleNode::FlagState::Unknown)
      .value("False", ModuleNode::FlagState::False)
      .value("True", ModuleNode::FlagState::True);

  nanobind::class_<ModuleNode>(m, "ModuleNode")
      .def_prop_ro("name", &ModuleNode::name)
      .def("full_name", &ModuleNode::full_name)
      .def_prop_ro("parent", &ModuleNode::parent, nanobind::rv_policy::reference)
      .def_prop_ro("package_state", &ModuleNode::package_state)
      .def_prop_ro("package", &ModuleNode::is_package)
      .def_prop_ro("shared_object_state", &ModuleNode::shared_object_state)
      .def_prop_ro("shared_object", &ModuleNode::is_shared_object)
      .def("children", &ModuleNode::children, nanobind::rv_policy::reference);

  m.def("root", &ModuleNode::root, nanobind::rv_policy::reference);
  m.def("from_full_name",
      &ModuleNode::from,
      nanobind::arg("full_name"),
      nanobind::rv_policy::reference);
  m.def("create",
      [](const std::string& full_name, bool is_package, bool shared_object) {
        return ModuleNode::create(full_name, nullptr, is_package, shared_object);
      },
      nanobind::arg("full_name"),
      nanobind::arg("is_package") = false,
      nanobind::arg("shared_object") = false,
      nanobind::rv_policy::reference);
});