#include <pymergetic/easybind/sample/sample.hpp>

#include <string>


namespace pymergetic::easybind::sample {


EASYBIND_SAMPLE_API void raise_sample_error(const std::string& message) {
  throw SampleError(message);
}

EASYBIND_SAMPLE_API void raise_widget_error(const std::string& message) {
  throw WidgetError(message);
}

Widget::Widget() = default;

Widget::Widget(std::string name_in, int value_in, WidgetKind kind_in)
    : name(std::move(name_in)), value(value_in), kind(kind_in) {}

int Widget::bump(int delta) {
  value += delta;
  return value;
}

std::string Widget::summary() const {
  return name + ":" + std::to_string(value);
}

std::string Widget::kind_name() const {
  switch (kind) {
    case WidgetKind::Basic:
      return "basic";
    case WidgetKind::Fancy:
      return "fancy";
    case WidgetKind::Deluxe:
      return "deluxe";
  }
  return "unknown";
}

EASYBIND_SAMPLE_API const int kDefaultValue = 42;
EASYBIND_SAMPLE_API const int kDefaultValue_easy = kDefaultValue;
EASYBIND_SAMPLE_API const double kPi = 3.141592653589793;
EASYBIND_SAMPLE_API const double kPi_easy = kPi;
EASYBIND_SAMPLE_API const char* kLibName = "pymergetic.easybind.sample";
EASYBIND_SAMPLE_API const char* kLibName_easy = kLibName;

EASYBIND_SAMPLE_API int add(int left, int right) {
  return left + right;
}

EASYBIND_SAMPLE_API int add_easy(int left, int right) {
  return add(left, right);
}

EASYBIND_SAMPLE_API std::string greet(const std::string& name) {
  return "hello " + name;
}

EASYBIND_SAMPLE_API std::string greet_easy(const std::string& name) {
  return greet(name);
}

EASYBIND_SAMPLE_API Widget make_widget(const std::string& name, int value, WidgetKind kind) {
  return Widget{name, value, kind};
}

EASYBIND_SAMPLE_API Widget make_widget_easy(const std::string& name, int value, WidgetKind kind) {
  return make_widget(name, value, kind);
}


}  // namespace pymergetic::easybind::sample



