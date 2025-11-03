"""
Utilities for managing a bundled GridLAB-D installation.
"""

import os
import sys
from pathlib import Path
from typing import Optional, Dict


def get_bundled_lib_path() -> Optional[str]:
    """
    Locate the directory that holds the bundled GridLAB-D shared libraries.
    """
    package_dir = Path(__file__).resolve().parent.parent  # e.g. site-packages or src/

    candidates = [
        package_dir / "gridlabd_bundle" / "lib",          # installed wheel layout
        package_dir / ".." / "gridlabd_bundle" / "lib",   # editable/development install
        package_dir / ".." / ".." / ".." / "cmake-build" / "lib",  # local build tree
    ]

    for candidate in candidates:
        if candidate.exists():
            return str(candidate.resolve())

    return None


def setup_bundled_environment() -> bool:
    """
    Configure environment variables so the C++ API can locate bundled libraries.
    """
    lib_path = get_bundled_lib_path()
    if not lib_path:
        return False

    lib_dir = Path(lib_path).resolve()
    root_dir = lib_dir.parent  # one level up from lib/

    os.environ["GRIDLABD_ROOT"] = str(root_dir)

    # Teach the core API where the install root lives (even if there is no executable).
    try:
        from .gridlabd_core import set_install_root
        set_install_root(str(root_dir))
    except Exception:
        pass

    if sys.platform.startswith("linux"):
        lib_env = "LD_LIBRARY_PATH"
    elif sys.platform == "darwin":
        lib_env = "DYLD_LIBRARY_PATH"
    else:
        lib_env = "PATH"  # Windows: DLLs are looked up via PATH

    current = os.environ.get(lib_env, "")
    if lib_path not in current.split(os.pathsep):
        os.environ[lib_env] = (
            f"{lib_dir}{os.pathsep}{current}" if current else str(lib_dir)
        )

    return True


def get_gridlabd_info() -> Dict[str, Optional[str]]:
    """
    Report where the package found bundled assets.
    """
    lib_path = get_bundled_lib_path()
    root_dir = str(Path(lib_path).resolve().parent) if lib_path else None

    return {
        "bundled_libraries": lib_path,
        "gridlabd_root": root_dir,
        "environment_setup": lib_path is not None,
    }