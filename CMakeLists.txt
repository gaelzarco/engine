cmake_minimum_required(VERSION 3.30)
if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.4.0")
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "4.3.0")
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0.3")
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0.0")
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "3.31.8")
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
else()
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
endif()

if(APPLE)
    set(LLVM_PREFIX "$ENV{HOMEBREW_PREFIX}/opt/llvm")
    set(CMAKE_C_COMPILER   "${LLVM_PREFIX}/bin/clang"   CACHE PATH "")
    set(CMAKE_CXX_COMPILER "${LLVM_PREFIX}/bin/clang++" CACHE PATH "")
elseif(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "clang++" CACHE PATH "")
endif()

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

project(engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# NOTE: CMAKE_CXX_MODULE_STD removed from global scope — set per-target below

add_subdirectory(modules/minifb)

add_executable(engine engine.cpp)

# Scope both module-related properties to engine only, not the whole build
set_target_properties(engine PROPERTIES
    CXX_SCAN_FOR_MODULES ON
    CXX_MODULE_STD ON
)
target_compile_features(engine PUBLIC cxx_std_23)

target_sources(engine
    PUBLIC FILE_SET CXX_MODULES FILES
    modules/obj/obj_v0.cppm
)
target_sources(engine
    PUBLIC FILE_SET CXX_MODULES FILES
    modules/render/render_v0.cppm
)

target_link_libraries(engine PRIVATE minifb)
target_compile_options(engine PRIVATE -O3 -flto -march=native)
target_link_options(engine PRIVATE -flto)
