from __future__ import annotations

def get_version(package_name: str, *, root: str, relative_to: str) -> str:
    try:
        from importlib.metadata import version as _pkg_version

        return _pkg_version(package_name)
    except Exception:
        try:
            from setuptools_scm import get_version as _get_version  # type: ignore

            return _get_version(
                root=root,
                relative_to=relative_to,
                local_scheme="no-local-version",
            )
        except Exception:
            return "0.0.0"


def get_version_cpp() -> str:
    try:
        from pymergetic.easybind.version import __cpp__ as _cpp  # type: ignore[import-not-found]

        return str(_cpp.__version__)
    except Exception:
        return "0.0.0"


__all__ = ["get_version", "get_version_cpp"]
