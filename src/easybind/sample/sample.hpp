#pragma once

#include <stdexcept>
#include <string>

#include <easybind/export.hpp>

#if defined(EASYBIND_SAMPLE_BUILD)
#  define EASYBIND_SAMPLE_API EASYBIND_EXPORT
#else
#  define EASYBIND_SAMPLE_API EASYBIND_IMPORT
#endif


namespace easybind::sample {


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
struct EASYBIND_SAMPLE_API SampleError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct EASYBIND_SAMPLE_API WidgetError : SampleError {
  using SampleError::SampleError;
};

struct EASYBIND_SAMPLE_API SampleError_easy : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct EASYBIND_SAMPLE_API WidgetError_easy : SampleError_easy {
  using SampleError_easy::SampleError_easy;
};

EASYBIND_SAMPLE_API void raise_sample_error(const std::string& message);
EASYBIND_SAMPLE_API void raise_widget_error(const std::string& message);

// Class
struct EASYBIND_SAMPLE_API Widget {
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
EASYBIND_SAMPLE_API extern const int kDefaultValue;
EASYBIND_SAMPLE_API extern const int kDefaultValue_easy;

EASYBIND_SAMPLE_API extern const double kPi;
EASYBIND_SAMPLE_API extern const double kPi_easy;

EASYBIND_SAMPLE_API extern const char* kLibName;
EASYBIND_SAMPLE_API extern const char* kLibName_easy;

// Functions
EASYBIND_SAMPLE_API int add(int left, int right);
EASYBIND_SAMPLE_API int add_easy(int left, int right);

EASYBIND_SAMPLE_API std::string greet(const std::string& name);
EASYBIND_SAMPLE_API std::string greet_easy(const std::string& name);

EASYBIND_SAMPLE_API Widget make_widget(const std::string& name,
    int value = 0,
    WidgetKind kind = WidgetKind::Basic);
EASYBIND_SAMPLE_API Widget make_widget_easy(const std::string& name,
    int value = 0,
    WidgetKind kind = WidgetKind::Basic);
}  // namespace easybind::sample
