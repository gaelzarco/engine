#include <string>
#include <vector>
#include <print>
#include <chrono>
#include "modules/minifb/include/MiniFB_cpp.h"

import obj_v0_3;

auto main(int argc, const char* argv[]) -> int {
    if (argc < 2) {
        std::println("[ERR] Please include file path for obj file");
        return 1;
    }
    
    std::string file_path = argv[1];
    obj file{};

    auto start = std::chrono::high_resolution_clock::now();
    file.read(file_path);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop - start);
    std::println("[LOG] .obj read duration: {} ms", duration.count());

    std::size_t total = file.vertices().capacity() * sizeof(obj::_vertex)
    + file.texture_coordinates().capacity() * sizeof(obj::_texture_coordinate)
    + file.vertex_normals().capacity() * sizeof(obj::_vertex_normal)
    + file.parameter_space_vertices().capacity() * sizeof(obj::_parameter_space_vertex)
    + file.faces().capacity() * sizeof(obj::_face)
    + file.lines().capacity() * sizeof(obj::_line);
    std::println("[LOG] Total memory allocated: {} bytes ({} MB)", total, total / 1024 / 1024);

    struct mfb_window *window = mfb_open_ex("CXX Rasterizer", 800, 600, MFB_WF_RESIZABLE);

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
