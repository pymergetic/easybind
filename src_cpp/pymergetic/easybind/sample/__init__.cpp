#include <pymergetic/easybind/sample/__init__.hpp>

#include <string>


namespace pymergetic::easybind::sample {


module::ModuleNode* __module__ =
    module::ModuleNode::create("pymergetic.easybind.sample", module::ModuleNode::FlagState::True);

// easybind registration (compare to classic nanobind bindings in __cpp__.cpp)
bool __module_initialized__ = []() {
  if (__module__) {
    // Register enum with easybind
    __module__->register_enum<WidgetKind_easy>(); 

    
  }
  return true;
}();


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

const int kDefaultValue = 42;
const double kPi = 3.141592653589793;
const char* kLibName = "pymergetic.easybind.sample";

int add(int left, int right) {
  return left + right;
}

std::string greet(const std::string& name) {
  return "hello " + name;
}

Widget make_widget(const std::string& name, int value, WidgetKind kind) {
  return Widget{name, value, kind};
}

void raise_sample_error(const std::string& message) {
  throw SampleError(message);
}

void raise_widget_error(const std::string& message) {
  throw WidgetError(message);
}


}  // namespace pymergetic::easybind::sample
