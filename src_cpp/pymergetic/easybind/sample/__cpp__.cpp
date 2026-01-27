#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <stdexcept>

#include <pymergetic/easybind/sample/__init__.hpp>

namespace nb = nanobind;

NB_MODULE(sample, m) {
  using namespace pymergetic::easybind::sample;

  m.doc() = "Nanobind sample module for easybind.";

  
  // easybind bindings (see __init__.cpp for registration details)
  if (__module__) {
    __module__->apply(m);
  } else {
    throw std::runtime_error("sample module node not initialized");
  }


  // Classic nanobind bindings

  // Register enum with nanobind
  nb::enum_<WidgetKind>(m, "WidgetKind")
      .value("Basic", WidgetKind::Basic)
      .value("Fancy", WidgetKind::Fancy)
      .value("Deluxe", WidgetKind::Deluxe)
      .export_values();

  auto sample_error = nb::exception<SampleError>(m, "SampleError");
      nb::exception<WidgetError>(m, "WidgetError", sample_error);
    

  nb::class_<Widget>(m, "Widget")
      .def(nb::init<>())
      .def(nb::init<std::string, int, WidgetKind>(),
           nb::arg("name"),
           nb::arg("value") = 0,
           nb::arg("kind") = WidgetKind::Basic)
      .def_rw("name", &Widget::name)
      .def_rw("value", &Widget::value)
      .def_rw("kind", &Widget::kind)
      .def("bump", &Widget::bump, nb::arg("delta") = 1)
      .def("summary", &Widget::summary)
      .def("kind_name", &Widget::kind_name);

  m.def("add", &add, nb::arg("left"), nb::arg("right"));
  m.def("greet", &greet, nb::arg("name"));
  m.def("make_widget",
        &make_widget,
        nb::arg("name"),
        nb::arg("value") = 0,
        nb::arg("kind") = WidgetKind::Basic);
  m.def("raise_sample_error", &raise_sample_error, nb::arg("message"));
  m.def("raise_widget_error", &raise_widget_error, nb::arg("message"));

  m.attr("DEFAULT_VALUE") = kDefaultValue;
  m.attr("PI") = kPi;
  m.attr("LIB_NAME") = kLibName;
}
