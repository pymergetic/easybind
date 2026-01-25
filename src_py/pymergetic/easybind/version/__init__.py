def get_version(package_name: str, *, root: str, relative_to: str) -> str:
    try:
        from importlib.metadata import version as _pkg_version

        version = _pkg_version(package_name)
        if version != "0.0.0":
            return version
    except Exception:
        return "0.0.0"
    return "0.0.0"


def get_version_cpp(
    module_name: str,
    *,
    cpp_module: str = "__cpp__",
    attr: str = "__version__",
) -> str:
    try:
        from importlib import import_module

        module = import_module(f"{module_name}.{cpp_module}")
        return str(getattr(module, attr))
    except Exception:
        return "0.0.0"


__all__ = ["get_version", "get_version_cpp"]