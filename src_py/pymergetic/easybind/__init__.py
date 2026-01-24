"""Easybind public Python surface."""

from pymergetic.easybind.__project__ import __version__

# Import the native module to trigger registry application.
try:
    from . import _internal as _native  # noqa: F401
except Exception:
    _native = None

__all__ = ["__version__"]
