from pymergetic.easybind.version import get_version


__package_name__ = "pymergetic-easybind"
__version__ = get_version(__package_name__, root="../../..", relative_to=__file__)

try:
    from pymergetic.easybind import __cpp__
    __version_cpp__ = __cpp__.__version__
except Exception:
    __version_cpp__ = "0.0.0"


__all__ = ["__package_name__", "get_version", "__version__", "__version_cpp__"]