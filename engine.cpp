#include <string>
#include <print>
#include <chrono>
#include <ctime>
#include "modules/minifb/include/MiniFB_cpp.h"

import obj_v0_3;
import render_v0;

const static std::size_t WIDTH = 800;
const static std::size_t HEIGHT = 600;

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
