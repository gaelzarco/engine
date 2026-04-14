cmake_minimum_required(VERSION 3.28)

if(APPLE)
    set(LLVM_PREFIX "$ENV{HOMEBREW_PREFIX}/opt/llvm")
    set(CMAKE_C_COMPILER   "${LLVM_PREFIX}/bin/clang"   CACHE PATH "")
    set(CMAKE_CXX_COMPILER "${LLVM_PREFIX}/bin/clang++" CACHE PATH "")
endif()

set(CMAKE_BUILD_TYPE Release)
project(engine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(modules/minifb)

set(CMAKE_CXX_CLANG_TIDY clang-tidy)
set_target_properties(minifb PROPERTIES CXX_CLANG_TIDY "")

add_executable(engine engine.cpp)

target_sources(engine
    PUBLIC FILE_SET CXX_MODULES FILES
    modules/obj/obj_v0_3.cppm
)
target_link_libraries(engine PRIVATE minifb)
target_compile_options(engine PRIVATE -O3 -flto -march=native)
target_link_options(engine PRIVATE -flto)
