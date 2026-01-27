#pragma once

#include <stdexcept>
#include <string>

#include <boost/describe.hpp>


namespace pymergetic::easybind::sample {

enum class WidgetKind {
  Basic,
  Fancy,
};

class SampleError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

struct Widget {
  std::string name;
  int value = 0;
  WidgetKind kind = WidgetKind::Basic;

  std::string summary() const;
  int bump(int delta = 1);

  BOOST_DESCRIBE_CLASS(Widget, (), (name, value, kind, summary, bump), (), ())
};

Widget make_widget(std::string name, int value = 0, WidgetKind kind = WidgetKind::Basic);
int async_add(int a, int b);
void raise_error(const std::string& message);

}  // namespace pymergetic::easybind::sample
