"""Easybind public Python surface."""

from pymergetic.easybind.__project__ import __version__, __version_cpp__


# Import the native module to trigger registry application.
try:
    from . import __cpp__  # noqa: F401
except Exception:
    __cpp__ = None


def refresh_bindings() -> None:
    """Apply newly registered native bindings to this module."""
    if __cpp__ is None:
        raise RuntimeError("easybind native module is not available")
    __cpp__.refresh_bindings()


__all__ = ["__version__", "__version_cpp__", "refresh_bindings"]
