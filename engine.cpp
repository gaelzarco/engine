/**
 * @file engine.cpp
 * @brief Entry point for the CXX Rasterizer — a minimal Wavefront OBJ viewer
 *        with a software-rendered MiniFB window.
 *
 * @par Dependencies
 * - C++23 module @c           — OBJ file parsing and in-memory representation.
 * - C++23 module @c render_v0 — 2D primitive rasterization into a pixel buffer.
 * - MiniFB                    — cross-platform framebuffer window library.
 *
 * @par Usage
 * @code
 * ./build/engine <path/to/model.obj>
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

    rgb_color color{};

    triangle(a, b, c, buffer, color, WIDTH, HEIGHT);

    // Draw 50 random triangles
    for (auto i{0uz}; i < 50; ++i) {
        std::random_device rd;
        std::mt19937 rng(rd());

        // Limit random distribution to values within bounds
        std::uniform_int_distribution<int> distX(0, WIDTH - 1);
        std::uniform_int_distribution<int> distY(0, HEIGHT - 1);

        // Generate points within random distribution
        const int v1x = distX(rng); const int v1y = distY(rng);
        const int v2x = distX(rng); const int v2y = distY(rng);
        const int v3x = distX(rng); const int v3y = distY(rng);

        // Build vertices
        const point v1{v1x, v1y};
        const point v2{v2x, v2y};
        const point v3{v3x, v3y};

        // Generate random color
        rgb_color v_color{};

        // Draw triangle
        triangle(v1, v2, v3, buffer, v_color, WIDTH, HEIGHT);
    }

    while (mfb_wait_sync(window)) {
        state = mfb_update_ex(window, buffer.data(), WIDTH, HEIGHT);
        if (state != MFB_STATE_OK) break;
    }

    return 0;
}
