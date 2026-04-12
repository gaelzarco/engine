#include <string>
#include <vector>
#include <print>
#include <chrono>
#include "modules/minifb/include/MiniFB_cpp.h"

import obj_v0_1;

auto main(int argc, const char* argv[]) -> int {
    if (argc < 2) {
        std::println("[ERR] Please include file path for obj file");
        return 1;
    }
    
    std::string file_path = argv[1];
    auto obj = obj_::make();

    auto start = std::chrono::high_resolution_clock::now();
    obj.read(file_path);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop - start);
    std::println("[LOG] .obj read duration: {} ms", duration.count());

    struct mfb_window *window = mfb_open_ex("CXX Rasterizer", 800, 600,
        MFB_WF_RESIZABLE);

    if (window == nullptr) {
        std::println("[ERR] Failed to create window");
        return 1;
    };

    std::vector<std::size_t> buffer(static_cast<size_t>(800 * 600));
    mfb_update_state state;

    while (mfb_wait_sync(window)) {
        state = mfb_update_ex(window, buffer.data(), 800, 600);
        if (state != MFB_STATE_OK) break;
    }

    return 0;
}
