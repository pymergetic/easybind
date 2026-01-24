#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <pymergetic/easybind/asyncio_asio.hpp>
#include <pymergetic/easybind/bind_utils.hpp>
#include <pymergetic/easybind/sample/sample.hpp>

namespace nb = nanobind;

namespace pymergetic::easybind::sample {

EASYBIND_REGISTER_ENUM("pymergetic.easybind.sample", WidgetKind);

static int async_add_impl(int a, int b) {
  return a + b;
}

EASYBIND_REGISTER_ASYNC("pymergetic.easybind.sample", "async_add", async_add_impl);

void bind_sample(::nanobind::module_& m) {
  nb::class_<Widget>(m, "Widget")
      .def(nb::init<>())
      .def_rw("name", &Widget::name)
      .def_rw("value", &Widget::value)
      .def_rw("kind", &Widget::kind)
      .def("summary", &Widget::summary)
      .def("bump", &Widget::bump, nb::arg("delta") = 1);

  m.def("make_widget",
        &make_widget,
        nb::arg("name"),
        nb::arg("value") = 0,
        nb::arg("kind") = WidgetKind::Basic);
}

}  // namespace pymergetic::easybind::sample

EASYBIND_REGISTER_PACKAGE("pymergetic.easybind.sample",
                          [](nanobind::module_& m) { pymergetic::easybind::sample::bind_sample(m); });
