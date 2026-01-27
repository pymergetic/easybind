#pragma once

#include <stdexcept>
#include <string>

#include <pymergetic/easybind/module/__init__.hpp>

namespace pymergetic::easybind::sample {

enum class WidgetKind {
  Basic = 0,
  Fancy = 1,
  Deluxe = 2,
};

enum class WidgetKind_easy {
  Basic = 0,
  Fancy = 1,
  Deluxe = 2,
};

extern module::ModuleNode* __module__;
extern bool __module_initialized__;

struct SampleError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct WidgetError : SampleError {
  using SampleError::SampleError;
};

struct Widget {
  std::string name;
  int value = 0;
  WidgetKind kind = WidgetKind::Basic;

  Widget();
  Widget(std::string name_in, int value_in, WidgetKind kind_in);

  int bump(int delta = 1);
  std::string summary() const;
  std::string kind_name() const;
};



extern const int kDefaultValue;
extern const double kPi;
extern const char* kLibName;

int add(int left, int right);
std::string greet(const std::string& name);
Widget make_widget(const std::string& name, int value = 0, WidgetKind kind = WidgetKind::Basic);
void raise_sample_error(const std::string& message);
void raise_widget_error(const std::string& message);

}  // namespace pymergetic::easybind::sample
