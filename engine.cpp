#include <vector>
#include <print>
#include "modules/minifb/include/MiniFB_cpp.h"

import obj;

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::println("[ERR] Please include file path for obj file");
        return 1;
    }

    obj_ test;
    test.read_from_file(argv[1]);
    test.print();

    struct mfb_window *window = mfb_open_ex("CXX Rasterizer", 800, 600,
        MFB_WF_RESIZABLE);

    if (window == NULL) {
        std::println("[ERR] Failed to create window");
        return 1;
    };

    std::vector<uint32_t> buffer(800 * 600);
    mfb_update_state state;

    while (mfb_wait_sync(window)) {
        state = mfb_update_ex(window, buffer.data(), 800, 600);
        if (state != MFB_STATE_OK) break;
    }

    return 0;
}
