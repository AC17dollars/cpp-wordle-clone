#!/usr/bin/env python3
"""Generate embedded_assets.{h,cpp} (C++ byte arrays) from the files in assets/.

Usage: embed_assets.py <output_dir>   (defaults to build/generated)

Run from anywhere; asset paths are resolved relative to the repo root (the
parent of this script's directory). The generated .cpp is compiled into the
binary so the game carries its data with it.
"""
import os
import sys

# (asset path relative to repo root, C symbol name)
ASSETS = [
    ("assets/dictionary/words_alpha.txt", "asset_words_select"),
    ("assets/dictionary/5lettersword.txt", "asset_words_valid"),
    ("assets/images/Wordle.ico", "asset_icon_ico"),
]

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
out_dir = sys.argv[1] if len(sys.argv) > 1 else os.path.join(root, "build", "generated")
os.makedirs(out_dir, exist_ok=True)

with open(os.path.join(out_dir, "embedded_assets.h"), "w") as h:
    h.write("#pragma once\n#include <cstddef>\n\n")
    for _, sym in ASSETS:
        h.write(f"extern const unsigned char {sym}[];\n")
        h.write(f"extern const size_t {sym}_len;\n")

with open(os.path.join(out_dir, "embedded_assets.cpp"), "w") as c:
    c.write('#include "embedded_assets.h"\n\n')
    for path, sym in ASSETS:
        with open(os.path.join(root, path), "rb") as f:
            data = f.read()
        c.write(f"const unsigned char {sym}[] = {{")
        c.write(",".join(str(b) for b in data))
        c.write(f"}};\nconst size_t {sym}_len = {len(data)};\n\n")

print(f"embedded {len(ASSETS)} assets -> {out_dir}")
