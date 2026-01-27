#include <nanobind/nanobind.h>
#include <stdexcept>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <pymergetic/easybind/__init__.hpp>


NB_MODULE(easybind, m) {
  m.doc() = "pymergetic.easybind module";

  // Mark as package so submodules can be imported.
  nanobind::module_ os = nanobind::module_::import_("os");
  nanobind::object dirname = os.attr("path").attr("dirname");
  nanobind::list package_path;
  package_path.append(dirname(m.attr("__file__")));
  m.attr("__path__") = package_path;

  nanobind::module_ module_ns =
      m.def_submodule("module", "Easybind module tree utilities.");

  using ModuleNode = pymergetic::easybind::module::ModuleNode;

  nanobind::enum_<ModuleNode::FlagState>(module_ns, "FlagState")
      .value("Unknown", ModuleNode::FlagState::Unknown)
      .value("False", ModuleNode::FlagState::False)
      .value("True", ModuleNode::FlagState::True);

  if (pymergetic::easybind::__module__) {
    pymergetic::easybind::__module__->apply(m);
  } else {
    throw std::runtime_error("easybind module node not initialized");
  }

  nanobind::class_<ModuleNode::Symbol>(module_ns, "Symbol")
      .def_prop_ro("kind", [](const ModuleNode::Symbol& symbol) { return symbol.kind; })
      .def_prop_ro("name", [](const ModuleNode::Symbol& symbol) { return symbol.name; })
      .def_prop_ro("cpp_type", [](const ModuleNode::Symbol& symbol) { return symbol.cpp_type; });

  nanobind::class_<ModuleNode::EnumSymbol, ModuleNode::Symbol>(module_ns, "EnumSymbol")
      .def_prop_ro("enum_items",
          [](const ModuleNode::EnumSymbol& symbol) { return symbol.enum_items; });

  nanobind::class_<ModuleNode>(module_ns, "ModuleNode")
      .def_prop_ro("name", &ModuleNode::name)
      .def("full_name", &ModuleNode::full_name)
      .def_prop_ro("parent", &ModuleNode::parent, nanobind::rv_policy::reference)
      .def_prop_ro("shared_object_state", &ModuleNode::shared_object_state)
      .def_prop_ro("shared_object", &ModuleNode::is_shared_object)
      .def("symbols", &ModuleNode::symbols)
      .def("children", &ModuleNode::children, nanobind::rv_policy::reference);

  module_ns.def("root", &ModuleNode::root, nanobind::rv_policy::reference);
  module_ns.def("from_full_name",
      &ModuleNode::from,
      nanobind::arg("full_name"),
      nanobind::rv_policy::reference);
  module_ns.def("create",
      &ModuleNode::create,
      nanobind::arg("full_name"),
      nanobind::arg("shared_object") = ModuleNode::FlagState::Unknown,
      nanobind::rv_policy::reference);
}