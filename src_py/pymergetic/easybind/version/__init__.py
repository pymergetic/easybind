from __future__ import annotations

def get_version(package_name: str, *, root: str, relative_to: str) -> str:
    try:
        from importlib.metadata import version as _pkg_version

        version = _pkg_version(package_name)
        if version != "0.0.0":
            return version
    except Exception:
        return "0.0.0"
    return "0.0.0"


def get_version_cpp() -> str:
    try:
        from pymergetic.easybind.version import __cpp__ as _cpp  # type: ignore[import-not-found]

        return str(_cpp.__version__)
    except Exception:
        return "0.0.0"


__all__ = ["get_version", "get_version_cpp"]
