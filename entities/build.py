import os
import subprocess
from pathlib import Path

# all variables
ROOT = ('entities/')
BUILD_MODE = "RELEASE"
BUILD_DIR = "../../build/"
DEP_DIR = "../../build/"
INC_DIRS = "../../lib/arch/x86_64 ../../lib/comps ../../lib/include ../../lib/internals ../../lib/utils ../../api"
OBJ_DIR = "../../obj"
LINK_DIR = BUILD_DIR
LINK_OBJS = "-lmvs"

for root, dirs, files in os.walk(ROOT):
    if "Makefile" in files:
        makefile_dir = Path(root)
        print(f"DIR= {makefile_dir}")
        result = subprocess.run(
                [
                    "make",
                    f"BUILD_MODE={BUILD_MODE}",
                    f"BUILD_DIR={BUILD_DIR}",
                    f"DEP_DIR={DEP_DIR}",
                    f"INC_DIRS={INC_DIRS}",
                    f"OBJ_DIR={OBJ_DIR}",
                    f"LINK_DIR={LINK_DIR}",
                    f"LINK_OBJS={LINK_OBJS}"
                    ],
                cwd=makefile_dir
                )
        if result.returncode != 0:
            print(f"BUILD {makefile_dir} failed")
