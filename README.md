# Engine

A C++23 3D engine built with modules and MiniFB.

## Features

- OBJ file parsing via C++23 named modules (`import obj`)
- Real-time windowed rendering with [MiniFB](https://github.com/nicowillis/minifb)
- Vertex, texture, normal, parameter, face, and line element support

## Build

Requires Clang 22+ and CMake with Ninja.
```bash
cmake -B build -G Ninja
cmake --build build
```

## Usage
```bash
./engine < model.obj
```

Or programmatically:
```cpp
obj_ model;
model.read_from_file("model.obj");
model.print();
```

## Requirements

- C++23
- Clang 22+
- CMake + Ninja
- MiniFB (included as submodule)
