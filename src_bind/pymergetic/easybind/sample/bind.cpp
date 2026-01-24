#include <pymergetic/easybind/prelude.hpp>
#include <pymergetic/easybind/sample/sample.hpp>

EASYBIND_PACKAGE("pymergetic.easybind.sample");

namespace pymergetic::easybind::sample {

EASYBIND_REGISTER_ENUM(WidgetKind);
EASYBIND_REGISTER_CLASS_METHODS(Widget,
                                EASYBIND_ASYNC_METHOD(Widget, summary)
                                EASYBIND_ASYNC_METHOD_ARGS(Widget, bump, ::easybind::arg("delta") = 1));

EASYBIND_REGISTER_ASYNC(async_add);

EASYBIND_REGISTER_FUNC(make_widget,
                       ::easybind::arg("name"),
                       ::easybind::arg("value") = 0,
                       ::easybind::arg("kind") = pymergetic::easybind::sample::WidgetKind::Basic);

}  // namespace pymergetic::easybind::sample


