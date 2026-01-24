#include <pymergetic/easybind/sample/sample.hpp>

namespace pymergetic::easybind::sample {

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

}  // namespace pymergetic::easybind::sample
