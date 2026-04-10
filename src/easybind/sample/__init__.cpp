#include <easybind/bind.hpp>

#include <easybind/sample/sample.hpp>


EASYBIND_NS_MODULE_SHARED_OBJECT(easybind::sample, sample, m, true, {
  m.doc() = "Nanobind sample module for easybind.";


  // Enums
  nanobind::enum_<WidgetKind>(m, "WidgetKind")
      .value("Basic", WidgetKind::Basic)
      .value("Fancy", WidgetKind::Fancy)
      .value("Deluxe", WidgetKind::Deluxe)
      .export_values();


  auto widget_kind_easy = easybind::enum_<WidgetKind_easy>(m, "WidgetKind_easy");
  (void)widget_kind_easy;


  // Exceptions
  auto sample_error_classic = nanobind::exception<SampleError>(m, "SampleError");
  auto widget_error_classic =
      nanobind::exception<WidgetError>(m, "WidgetError", sample_error_classic);
  (void)widget_error_classic;

  auto sample_error = nanobind::exception<SampleError_easy>(m, "SampleError_easy");
  auto widget_error =
      nanobind::exception<WidgetError_easy>(m, "WidgetError_easy", sample_error);
  (void)widget_error;


  m.def("raise_sample_error", &raise_sample_error, nanobind::arg("message"));
  m.def("raise_widget_error", &raise_widget_error, nanobind::arg("message"));


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


  // Constants
  m.attr("kDefaultValue") = kDefaultValue;
  m.attr("kDefaultValue_easy") = kDefaultValue_easy;

  
  m.attr("kPi") = kPi;
  m.attr("kPi_easy") = kPi_easy;


  m.attr("kLibName") = kLibName;
  m.attr("kLibName_easy") = kLibName_easy;


  // Functions
  m.def("add", &add, nanobind::arg("left"), nanobind::arg("right"));
  m.def("add_easy", &add_easy, nanobind::arg("left"), nanobind::arg("right"));


  m.def("greet", &greet, nanobind::arg("name"));
  m.def("greet_easy", &greet_easy, nanobind::arg("name"));


  m.def("make_widget",
      &make_widget,
      nanobind::arg("name"),
      nanobind::arg("value") = 0,
      nanobind::arg("kind") = WidgetKind::Basic);
  m.def("make_widget_easy",
      &make_widget_easy,
      nanobind::arg("name"),
      nanobind::arg("value") = 0,
      nanobind::arg("kind") = WidgetKind::Basic);
});