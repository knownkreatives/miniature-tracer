try:
    # Import the compiled extension placed into _ext by CMake
    from ._ext import minitracer_bindings as _bindings
except (ImportError, ModuleNotFoundError):
    _bindings = None

def add(a, b):
    if _bindings:
        return _bindings.add(a, b)
    raise RuntimeError("C extension not available. Build the project with BUILD_PY_EXTENSION=ON and install the package.")

__all__ = ["add"]
