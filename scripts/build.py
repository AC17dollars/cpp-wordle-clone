#!/usr/bin/env python3
"""Build Wordle on macOS, two ways, against a pre-built wxWidgets 3.3.4 in the
parent directory:

    build/static/   wxWidgets linked statically (self-contained binary)
    build/dynamic/  wxWidgets linked as dylibs, copied next to the binary

Assets are embedded into the binary (see embed_assets.py), so the result needs
no external files at runtime. Whichever wxWidgets build is missing is skipped.
"""
import os
import re
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))  # scripts/..
os.chdir(ROOT)

WX_STATIC = "../wxWidgets-3.3.4/build-macos/wx-config"
WX_SHARED = "../wxWidgets-3.3.4/build-macos-shared/wx-config"
GEN_DIR = "build/generated"
WXLIBS = "core,base"
WX_DYLIB_RE = re.compile(r"/wxWidgets-3.3.4/.*libwx.*\.dylib$")

SRC = [
    "src/main.cpp", "src/Wordle.cpp", "src/timer.cpp", "src/gamemechanics.cpp",
    "src/platform/dock_icon_mac.mm", f"{GEN_DIR}/embedded_assets.cpp",
]


def run(cmd):
    subprocess.run(cmd, check=True)


def wx_config(wxc, *args):
    return subprocess.check_output([wxc, *args]).decode().split()


def build_variant(wxc, out_dir):
    shutil.rmtree(out_dir, ignore_errors=True)
    os.makedirs(out_dir)
    print(f"==> {out_dir}  [{subprocess.check_output([wxc, '--selected-config']).decode().strip()}]")
    run(["clang++", "-std=c++17", "-O2", *SRC, "-I", "include", "-I", GEN_DIR,
         *wx_config(wxc, "--cxxflags"), *wx_config(wxc, "--libs", WXLIBS),
         "-framework", "AppKit", "-o", f"{out_dir}/Wordle"])


def wx_deps(macho):
    out = subprocess.check_output(["otool", "-L", macho]).decode().splitlines()[1:]
    return [ln.strip().split(" ")[0] for ln in out
            if WX_DYLIB_RE.search(ln.strip().split(" ")[0])]


def bundle_dylibs(binary, libdir):
    """Copy the wxWidgets dylib closure next to the binary and repoint every
    reference at @loader_path so they are found in the same folder."""
    out = os.path.dirname(binary)
    seen, pending = set(), wx_deps(binary)
    while pending:
        nxt = []
        for dep in pending:
            base = os.path.basename(dep)
            if base in seen:
                continue
            seen.add(base)
            dst = os.path.join(out, base)
            shutil.copy(os.path.join(libdir, base), dst)
            os.chmod(dst, 0o755)
            run(["install_name_tool", "-id", f"@loader_path/{base}", dst])
            nxt += wx_deps(dst)
        pending = nxt
    for f in [binary] + [os.path.join(out, b) for b in seen]:
        for dep in wx_deps(f):
            subprocess.run(["install_name_tool", "-change", dep,
                            f"@loader_path/{os.path.basename(dep)}", f])


def main():
    # Turn assets/ into a compilable C++ byte array embedded into the binary.
    run([sys.executable, "scripts/embed_assets.py", GEN_DIR])

    if os.access(WX_STATIC, os.X_OK):
        build_variant(WX_STATIC, "build/static")
    else:
        print(f"skip static: {WX_STATIC} missing")

    if os.access(WX_SHARED, os.X_OK):
        build_variant(WX_SHARED, "build/dynamic")
        libdir = os.path.abspath(os.path.join(os.path.dirname(WX_SHARED), "lib"))
        bundle_dylibs("build/dynamic/Wordle", libdir)
    else:
        print(f"skip dynamic: {WX_SHARED} missing")

    print("Done. Run it with:  ./build/static/Wordle   (or build/dynamic/Wordle)")


if __name__ == "__main__":
    main()
