/**
 * @file engine.cpp
 * @brief Entry point for the CXX Rasterizer — a minimal Wavefront OBJ viewer
 *        with a software-rendered MiniFB window.
 *
 * @par Dependencies
 * - C++23 module @c obj_v0_3  — OBJ file parsing and in-memory representation.
 * - C++23 module @c render_v0 — 2D primitive rasterization into a pixel buffer.
 * - MiniFB                    — cross-platform framebuffer window library.
 *
 * @par Usage
 * @code
 * ./rasterizer <path/to/model.obj>
 * @endcode
 */

#include "modules/minifb/include/MiniFB_cpp.h"

import std;
import obj_v0;
import render_v0;

/** @brief Horizontal resolution of the framebuffer and window, in pixels. */
const static std::size_t WIDTH = 1920;
/** @brief Vertical resolution of the framebuffer and window, in pixels. */
const static std::size_t HEIGHT = 1080;

/**
 * @brief Application entry point.
 *
 * @param argc  Number of command-line arguments (must be ≥ 2).
 * @param argv  Argument vector. @c argv[1] must be a path to a valid @c .obj file.
 *
 * @return 0 on clean exit, 1 on argument or window-creation error.
 */
auto main(int argc, const char* argv[]) -> int {

    if (argc < 2) {
        std::println("[ERR] Please include file path for obj file");
        return 1;
    }
    
    std::string file_path = argv[1];
    obj file{};

    // LOGGING
    auto start = std::chrono::high_resolution_clock::now();

    file.read(file_path);

    // LOGGING
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop - start);
    std::println("[LOG] .obj read duration: {} ms", duration.count());
    file.memory_out();

    struct mfb_window *window = mfb_open_ex("CXX Rasterizer", WIDTH, HEIGHT, MFB_WF_RESIZABLE);

    if (window == nullptr) {
        std::println("[ERR] Failed to create window");
        return 1;
    };

    std::vector<std::size_t> buffer(static_cast<size_t>(WIDTH * HEIGHT));
    mfb_update_state state;

    point a{7, 3};
    point b{12, 37};
    point c{62, 53};

    buff_color color{};

    triangle(a, b, c, buffer, color, WIDTH, HEIGHT);

    while (mfb_wait_sync(window)) {
        state = mfb_update_ex(window, buffer.data(), WIDTH, HEIGHT);
        if (state != MFB_STATE_OK) break;
    }

    return 0;
}
