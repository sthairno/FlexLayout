import os
import shutil
import sys

# ----------------------------

PROJECT_PATH = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIB_FILES: list[(str, str)] = {
    (
        os.path.join(PROJECT_PATH, "Build", "FlexLayout", "Debug", "FlexLayout_d.lib"),
        "FlexLayout/FlexLayout_d.lib"
    ),
    (
        os.path.join(PROJECT_PATH, "Build", "FlexLayout", "Release", "FlexLayout.lib"),
        "FlexLayout/FlexLayout.lib"
    ),
    (
        os.path.join(PROJECT_PATH, "vcpkg_installed", "x64-windows-static", "x64-windows-static", "debug", "lib", "yogacore.lib"),
        "yoga/yogacored.lib"
    ),
    (
        os.path.join(PROJECT_PATH, "vcpkg_installed", "x64-windows-static", "x64-windows-static", "lib", "yogacore.lib"),
        "yoga/yogacore.lib"
    )
}
SOURCE_DIR = os.path.join(PROJECT_PATH, "Library")
SOURCE_HEADER_EXTENSIONS = [ "h", "hpp", "ipp" ]
SOURCE_IGNORE_DIRS = [
    os.path.join(PROJECT_PATH, "Library", "FlexLayout", "Internal")
]

# ----------------------------

SOURCE_IGNORE_DIRS = [ os.path.normpath(path) for path in SOURCE_IGNORE_DIRS ]

print(SOURCE_IGNORE_DIRS[0])

def copy_headers(output_root: str) -> set[str]:
    def _ignore_patterns(path: str, names: list[str]):
        ignored_names = []
        for name in names:
            full_path = os.path.abspath(os.path.join(path, name))
            if os.path.isdir(full_path):
                if os.path.normpath(full_path) in SOURCE_IGNORE_DIRS:
                    ignored_names.append(name)
            else:
                ext = name.split(".")[-1]
                if ext not in SOURCE_HEADER_EXTENSIONS:
                    ignored_names.append(name)
        
        return set(ignored_names)
    
    shutil.copytree(
        SOURCE_DIR,
        output_root,
        ignore=_ignore_patterns,
        dirs_exist_ok=True
    )

def copy_libs(output_root: str):
    for (src_path, dst) in LIB_FILES:
        dst_path = os.path.join(output_root, dst)
        os.makedirs(os.path.dirname(dst_path), exist_ok=True)
        shutil.copyfile(src_path, dst_path)

def pack(output_dir: str):
    copy_headers(os.path.join(output_dir, "include"))
    copy_libs(os.path.join(output_dir, "lib", "Windows"))

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: python3 {sys.argv[0]} <output_dir>")
        sys.exit(1)
    
    pack(os.path.abspath(sys.argv[1]))