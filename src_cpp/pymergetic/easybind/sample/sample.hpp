#pragma once

#include <string>

#include <boost/describe.hpp>


namespace pymergetic::easybind::sample {

enum class WidgetKind {
  Basic,
  Fancy,
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

}  // namespace pymergetic::easybind::sample

