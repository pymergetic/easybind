#pragma once

#include <stdexcept>
#include <string>


namespace pymergetic::easybind::sample {


// Enums
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

// Exceptions
struct SampleError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct WidgetError : SampleError {
  using SampleError::SampleError;
};

struct SampleError_easy : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct WidgetError_easy : SampleError_easy {
  using SampleError_easy::SampleError_easy;
};

void raise_sample_error(const std::string& message);
void raise_widget_error(const std::string& message);

// Class
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

// Constants
extern const int kDefaultValue;
extern const int kDefaultValue_easy;

extern const double kPi;
extern const double kPi_easy;

extern const char* kLibName;
extern const char* kLibName_easy;

// Functions
int add(int left, int right);
int add_easy(int left, int right);

std::string greet(const std::string& name);
std::string greet_easy(const std::string& name);

Widget make_widget(const std::string& name,
    int value = 0,
    WidgetKind kind = WidgetKind::Basic);
Widget make_widget_easy(const std::string& name,
    int value = 0,
    WidgetKind kind = WidgetKind::Basic);
}  // namespace pymergetic::easybind::sample
