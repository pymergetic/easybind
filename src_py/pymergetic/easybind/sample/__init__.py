"""Sample bindings for easybind."""

try:
    from . import _sample as _native  # type: ignore[import-not-found]  # noqa: F401
except Exception:
    _native = None

if _native is not None:
    Widget = _native.Widget
    WidgetKind = _native.WidgetKind
    __all__ = ["Widget", "WidgetKind"]
else:
    __all__ = []