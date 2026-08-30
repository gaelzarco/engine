# C++ 3D Renderer
A C++23 3D renderer built with MiniFB as single dependency. Targets Unix
environments.

## Features
- Window rendering with [MiniFB](https://github.com/nicowillis/minifb)
- OBJ file parser built from scratch via named module (`import obj`) 
- 3D rendering pipeline built from scratch via named module (`import render`)

## Requirements
- C++23
- Clang 22+
- CMake 3.30+
- Ninja

## Build
Requires Clang 22+ and CMake 3.30+ with Ninja.
```bash
cmake -B build -G Ninja
ln -sf build/compile_commands.json .
cmake --build build
```
Or run `chmod +x ./build.sh` on root directory, then `./build.sh`

## Usage
```bash
./build/renderer ./assets/<asset>
```

Or programmatically:
```cpp
obj_ model;
model.read("./assets/<asset>");

const std::size_t WIDTH = 1920;
const std::size_t HEIGHT = 1080;
const std::vector<std::uint32_t> buffer(static_cast<std::size_t>(WIDTH * 
HEIGHT)};

mfb_canvas canvas(WIDTH, HEIGHT, buffer);
mfb_canvas.render(model);
```
