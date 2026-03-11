import obj;

#include <print>

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::println("[ERR] Please provide .obj file name to parse");
        return 1;
    }

    std::println("[LOG] obj file to parse: {}", argv[1]);

    obj::file test;

    return 0;
}
