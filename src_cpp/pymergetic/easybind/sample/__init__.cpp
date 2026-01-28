#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <pymergetic/easybind/__init__.hpp>
#include <pymergetic/easybind/sample/__init__.hpp>
#include <pymergetic/easybind/sample/sample.hpp>


namespace pymergetic::easybind::sample {


void __init_bind__(nanobind::module_& m) {
  m.doc() = "Nanobind sample module for easybind.";

  // Enums needed for default arguments.
  nanobind::enum_<WidgetKind>(m, "WidgetKind")
      .value("Basic", WidgetKind::Basic)
      .value("Fancy", WidgetKind::Fancy)
      .value("Deluxe", WidgetKind::Deluxe)
      .export_values();

  // Enums
  easybind::enum_<WidgetKind_easy>(m, "WidgetKind_easy");

  // Exceptions
  auto sample_error = nanobind::exception<SampleError_easy>(m, "SampleError_easy");
  auto widget_error =
      nanobind::exception<WidgetError_easy>(m, "WidgetError_easy", sample_error);
  (void)widget_error;

  // Constants
  m.attr("kDefaultValue_easy") = kDefaultValue_easy;
  m.attr("kPi_easy") = kPi_easy;
  m.attr("kLibName_easy") = kLibName_easy;

  // Functions
  m.def("add_easy", &add_easy, nanobind::arg("left"), nanobind::arg("right"));
  m.def("greet_easy", &greet_easy, nanobind::arg("name"));
  m.def("make_widget_easy",
      &make_widget_easy,
      nanobind::arg("name"),
      nanobind::arg("value") = 0,
      nanobind::arg("kind") = WidgetKind::Basic);

  // Classic nanobind bindings (mirrors easybind order where possible).

  // Exceptions
  auto sample_error_classic = nanobind::exception<SampleError>(m, "SampleError");
  auto widget_error_classic =
      nanobind::exception<WidgetError>(m, "WidgetError", sample_error_classic);
  (void)widget_error_classic;

  // Class
  nanobind::class_<Widget>(m, "Widget")
      .def(nanobind::init<>())
      .def(nanobind::init<std::string, int, WidgetKind>(),
           nanobind::arg("name"),
           nanobind::arg("value") = 0,
           nanobind::arg("kind") = WidgetKind::Basic)
      .def_rw("name", &Widget::name)
      .def_rw("value", &Widget::value)
      .def_rw("kind", &Widget::kind)
      .def("bump", &Widget::bump, nanobind::arg("delta") = 1)
      .def("summary", &Widget::summary)
      .def("kind_name", &Widget::kind_name);
  m.def("raise_sample_error", &raise_sample_error, nanobind::arg("message"));
  m.def("raise_widget_error", &raise_widget_error, nanobind::arg("message"));

  // Constants
  m.attr("kDefaultValue") = kDefaultValue;
  m.attr("kPi") = kPi;
  m.attr("kLibName") = kLibName;

  // Functions
  m.def("add", &add, nanobind::arg("left"), nanobind::arg("right"));
  m.def("greet", &greet, nanobind::arg("name"));
  m.def("make_widget",
      &make_widget,
      nanobind::arg("name"),
      nanobind::arg("value") = 0,
      nanobind::arg("kind") = WidgetKind::Basic);
}

module::ModuleNode* __init__ =
    module::ModuleNode::create("pymergetic.easybind.sample", __init_bind__, true);


}  // namespace pymergetic::easybind::sample


NB_MODULE(sample, m) {
  pymergetic::easybind::apply_init(pymergetic::easybind::sample::__init__, m);
}