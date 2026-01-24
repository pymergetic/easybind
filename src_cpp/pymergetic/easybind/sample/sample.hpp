#pragma once

#include <string>

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
};

Widget make_widget(std::string name, int value = 0, WidgetKind kind = WidgetKind::Basic);

}  // namespace pymergetic::easybind::sample
