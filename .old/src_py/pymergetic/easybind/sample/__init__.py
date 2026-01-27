"""Sample bindings for easybind."""

try:
    from . import __cpp__  # type: ignore[import-not-found]  # noqa: F401
except Exception:
    __cpp__ = None

if __cpp__ is not None:
    Widget = __cpp__.Widget
    WidgetKind = __cpp__.WidgetKind
    async_add = __cpp__.async_add
    make_widget = __cpp__.make_widget
    SampleError = __cpp__.SampleError
    raise_error = __cpp__.raise_error
    __all__ = ["Widget", "WidgetKind", "async_add", "make_widget", "SampleError", "raise_error"]
else:
    __all__ = []