# Engine
A C++23 3D engine built with MiniFB as single dependency. Small and fast.

## Features
- OBJ file parsing via C++23 named modules (`import obj`) built from scratch.
- Real-time windowed rendering with [MiniFB](https://github.com/nicowillis/minifb)
- Simple 3D rendering pipeline built from scratch.

## Requirements
- C++23
- Clang 22+
- CMake + Ninja

## Configuration
Using different obj file parser versions programatically:
```cpp
// change import to target version
import obj_v0_1;
// OR
import obj_v0
// etc.
```
Be sure to update CMake file to reflect target file version to include:
```cmake
target_sources(engine
    PUBLIC FILE_SET CXX_MODULES FILES
    modules/obj_v0_1.cppm    # update this
)
// OR
```

## Build
Requires Clang 22+ and CMake with Ninja.
```bash
cmake -B build -G Ninja
cmake --build build
```

## Usage
```bash
./build/engine ./assets/<asset>
```

Or programmatically:
```cpp
obj_ model;
model.read("./assets/<asset>");
model.print();
```
