/**
 * @file engine.cpp
 * @brief Entry point for the CXX Rasterizer — a minimal Wavefront OBJ viewer
 *        with a software-rendered MiniFB window.
 *
 * Loads a Wavefront OBJ file supplied on the command line, logs parse time and
 * heap usage, opens an 800×600 resizable MiniFB window, and renders a small
 * set of test lines using @c render_v0's Bresenham rasterizer before entering
 * the main display loop.
 *
 * @par Dependencies
 * - C++20 module @c obj_v0_3  — OBJ file parsing and in-memory representation.
 * - C++20 module @c render_v0 — 2D primitive rasterization into a pixel buffer.
 * - MiniFB                    — cross-platform framebuffer window library.
 *
 * @par Usage
 * @code
 * ./rasterizer <path/to/model.obj>
 * @endcode
 *
 * @par Diagnostics
 * The following log messages are written to standard output:
 * @code
 * [ERR] Please include file path for obj file   // no argument supplied
 * [LOG] .obj read duration: <N> ms              // OBJ parse wall-clock time
 * [LOG] Memory allocated: <bytes> bytes (~N MB) // heap reserved by obj containers
 * [ERR] Failed to create window                 // MiniFB window creation failure
 * @endcode
 */

#include <string>
#include <print>
#include <chrono>
#include <ctime>
#include "modules/minifb/include/MiniFB_cpp.h"

import obj_v0_3;
import render_v0;

/** @brief Horizontal resolution of the framebuffer and window, in pixels. */
const static std::size_t WIDTH = 800;
/** @brief Vertical resolution of the framebuffer and window, in pixels. */
const static std::size_t HEIGHT = 600;

/**
 * @brief Application entry point.
 *
 * Performs the following steps in order:
 * -# Validates that a file path argument was supplied; exits with code 1 otherwise.
 * -# Parses the OBJ file at @p argv[1], logging wall-clock parse time and
 *    reserved heap memory via @c obj::memory_out().
 * -# Opens a resizable MiniFB window titled "CXX Rasterizer" at
 *    @c WIDTH × @c HEIGHT pixels; exits with code 1 on failure.
 * -# Allocates a flat @c WIDTH × @c HEIGHT pixel buffer initialised to zero.
 * -# Constructs three test points and four randomly coloured @c buff_color
 *    instances, then draws four lines between those points into the buffer
 *    using @c render_v0::line().
 * -# Enters the MiniFB event loop, pushing the buffer to the window every
 *    sync interval until the window is closed or an error state is returned.
 *
 * @param argc  Number of command-line arguments (must be ≥ 2).
 * @param argv  Argument vector. @c argv[1] must be a path to a valid @c .obj file.
 *
 * @return 0 on clean exit, 1 on argument or window-creation error.
 *
 * @note The four @c buff_color instances are default-constructed (all channels 0),
 *       which triggers random colour generation in @c buff_color's constructor.
 *       Each line is therefore drawn in a distinct, unpredictable colour.
 *
 * @warning The pixel buffer is @c std::size_t typed to match the MiniFB API on
 *          this platform. Ensure @c mfb_update_ex expects the same element width
 *          if porting to a different MiniFB configuration.
 *
 * @see obj::read()
 * @see obj::memory_out()
 * @see render_v0::line()
 * @see buff_color
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

    point a{}; a.x = 7; a.y = 3;
    point b{}; b.x = 12; b.y = 37;
    point c{}; c.x = 62; c.y = 53;

    buff_color color{};
    buff_color color1{};
    buff_color color2{};
    buff_color color3{};

    line(a, b, buffer, color, WIDTH, HEIGHT);
    line(c, b, buffer, color1, WIDTH, HEIGHT);
    line(c, a, buffer, color2, WIDTH, HEIGHT);
    line(a, c, buffer, color3, WIDTH, HEIGHT);

    while (mfb_wait_sync(window)) {
        state = mfb_update_ex(window, buffer.data(), WIDTH, HEIGHT);
        if (state != MFB_STATE_OK) break;
    }

    return 0;
}
