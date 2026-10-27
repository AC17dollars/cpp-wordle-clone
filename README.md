
## Binaries
Windows: [Wordle v1.1](https://github.com/AC17dollars/cpp-wordle-clone/releases/tag/v1.1)

## Compilation

### Dependencies

+wxWidgets v3.0 or above

### Windows

```
g++ ./src/*.cpp ./icon.res -o ./build/release/Wordle.exe -I ./include -I path/to/wxWidgets/315/include -I path/to/wxWidgets/315/lib/gcc_dll/mswu -L path/to/wxWidgets/315/lib/gcc_dll -l wxmsw31u_core -l wxbase31u -mwindows
```

### macOS (wxWidgets 3.3.4)

`build-macos.sh` assumes wxWidgets 3.3.4 is already built next to this project
(in the parent directory) and links against it via a relative path to its
`wx-config` — nothing is installed on the system `PATH`. It produces two builds:

| Folder          | wxWidgets link | Needs the wxWidgets build in |
| --------------- | -------------- | ---------------------------- |
| `build/static`  | static — self-contained binary | `../wxWidgets-3.3.4/build-macos` (configured `--disable-shared`) |
| `build/dynamic` | dynamic — wx dylibs copied next to the binary | `../wxWidgets-3.3.4/build-macos-shared` (configured shared) |

Build wxWidgets once (either or both configs, depending on which build you want):

```
cd ..
git clone --depth 1 --branch v3.3.4 --recurse-submodules --shallow-submodules \
    https://github.com/wxWidgets/wxWidgets.git wxWidgets-3.3.4
cd wxWidgets-3.3.4

# static build (for build/static)
mkdir -p build-macos && cd build-macos
../configure --disable-shared --disable-debug --with-osx_cocoa && make -j$(sysctl -n hw.ncpu)
cd ..

# shared build (for build/dynamic)
mkdir -p build-macos-shared && cd build-macos-shared
../configure --disable-debug --with-osx_cocoa && make -j$(sysctl -n hw.ncpu)
```

Then build the game (whichever wxWidgets build is missing is skipped):

```
python3 build.py
```

The `dynamic` build depends only on the wxWidgets dylibs, which are copied into
`build/dynamic/` beside the executable (via `@loader_path`); wxWidgets' own
dependencies stay baked into those dylibs. Resource paths are relative to the
build folder, so run each binary from there:

```
cd build/static  && ./Wordle     # or: cd build/dynamic && ./Wordle
```
