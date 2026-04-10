#include <easybind/sample/sample.hpp>

#include <magic_enum/magic_enum.hpp>

#include <algorithm>
#include <cctype>
#include <string>

namespace {

/// Lowercase slug from enum member name (`Basic` → `basic`). Widget_easy::kind_name() only;
/// classic Widget::kind_name() uses an explicit switch.
template <typename E>
std::string enum_kind_slug(E value) {
  const auto name = magic_enum::enum_name(value);
  if (name.empty()) {
    return "unknown";
  }
  std::string out(name.begin(), name.end());
  std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return out;
}

}  // namespace

namespace easybind::sample {


void raise_sample_error(const std::string& message) {
  throw SampleError(message);
}

void raise_widget_error(const std::string& message) {
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

Widget_easy::Widget_easy() = default;

Widget_easy::Widget_easy(std::string name_in, int value_in, WidgetKind_easy kind_in)
    : name(std::move(name_in)), value(value_in), kind(kind_in) {}

int Widget_easy::bump(int delta) {
  value += delta;
  return value;
}

std::string Widget_easy::summary() const {
  return name + ":" + std::to_string(value);
}

std::string Widget_easy::kind_name() const {
  return enum_kind_slug(kind);
}

const int kDefaultValue = 42;
const int kDefaultValue_easy = kDefaultValue;
const double kPi = 3.141592653589793;
const double kPi_easy = kPi;
const char* kLibName = "easybind.sample";
const char* kLibName_easy = kLibName;

int add(int left, int right) {
  return left + right;
}

int add_easy(int left, int right) {
  return add(left, right);
}

std::string greet(const std::string& name) {
  return "hello " + name;
}

std::string greet_easy(const std::string& name) {
  return greet(name);
}

Widget make_widget(const std::string& name, int value, WidgetKind kind) {
  return Widget{name, value, kind};
}

Widget_easy make_widget_easy(const std::string& name, int value, WidgetKind_easy kind) {
  return Widget_easy{name, value, kind};
}


}  // namespace easybind::sample
