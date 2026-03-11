module;
#include <vector>

export module obj;

export namespace obj {
    class file {
    public:
        struct vertex  { float x{}, y{}, z{}, w{}; };
        struct texture { float u{}; float v{}; float w{}; };
        struct normal  { float x{}; float y{}; float z{}; };
        struct param   { float u{}; float v{}; float w{}; };

        using vertices      = std::vector<vertex>;
        using vert_textures = std::vector<texture>;
        using vert_normals  = std::vector<normal>;
        using vert_params   = std::vector<param>;

        explicit file(vertices v = {}, vert_textures vt = {}, vert_normals vn = {}
        , vert_params vp = {}) : v_(std::move(v)), vt_(std::move(vt)),
        vn_(std::move(vn)), vp_(std::move(vp)) {}

    private:
        vertices      v_;
        vert_textures vt_;
        vert_normals  vn_;
        vert_params   vp_;
    };
}
