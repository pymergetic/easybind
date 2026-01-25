from pymergetic.easybind.version import get_version, get_version_cpp


__package_name__ = "pymergetic-easybind"


__version__ = get_version(__package_name__, root="../../..", relative_to=__file__)
__version_cpp__ = get_version_cpp()


__all__ = ["__version__", "__version_cpp__", "__package_name__"]