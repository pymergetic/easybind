#include <pymergetic/easybind/prelude.hpp>
#include <pymergetic/easybind/sample/sample.hpp>


namespace pymergetic::easybind::sample {


EASYBIND_PACKAGE("pymergetic.easybind.sample");
EASYBIND_REGISTER_EXCEPTION(SampleError);
EASYBIND_REGISTER_ENUM(WidgetKind);
EASYBIND_REGISTER_CLASS_METHODS(
    Widget,
    EASYBIND_ASYNC_METHOD(Widget, summary)
    EASYBIND_ASYNC_METHOD_ARGS(Widget, bump, ::easybind::arg("delta") = 1));
EASYBIND_REGISTER_ASYNC(async_add);
EASYBIND_REGISTER_FUNC(raise_error, ::easybind::arg("message"));
EASYBIND_REGISTER_FUNC(make_widget,
                       ::easybind::arg("name"),
                       ::easybind::arg("value") = 0,
                       ::easybind::arg("kind") = WidgetKind::Basic);


std::string Widget::summary() const {
  const char* kind_label = kind == WidgetKind::Fancy ? "fancy" : "basic";
  return name + ":" + std::to_string(value) + ":" + kind_label;
}


int Widget::bump(int delta) {
  value += delta;
  return value;
}


Widget make_widget(std::string name, int value, WidgetKind kind) {
  Widget w;
  w.name = std::move(name);
  w.value = value;
  w.kind = kind;
  return w;
}


int async_add(int a, int b) {
  return a + b;
}

void raise_error(const std::string& message) {
  throw SampleError(message);
}


}  // namespace pymergetic::easybind::sample


EASYBIND_MODULE_PACKAGE(__cpp__);
