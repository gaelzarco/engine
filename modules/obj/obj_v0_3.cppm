module;

#include <charconv>
#include <vector>
#include <string>
#include <fstream>
#include <print>
#include <expected>

export module obj_v0_3;

export class obj {
public:
    struct _vertex      { float x{}, y{}, z{}, w{ 1.f }; };
    struct _texture_coordinate     { float u{}, v{}, w{}; };
    struct _vertex_normal      { float x{}, y{}, z{}; };
    struct _parameter_space_vertex   { float u{}, v{}, w{}; };
    struct _face_vertex { int v{}, vt{}, vn{}; };
    using  _face        = std::vector<_face_vertex>;
    using  _line_vertex = int;
    using  _line        = std::vector<_line_vertex>;

    using _vertices                 = std::vector<_vertex>;
    using _texture_coordinates      = std::vector<_texture_coordinate>;
    using _vertex_normals           = std::vector<_vertex_normal>;
    using _parameter_space_vertices = std::vector<_parameter_space_vertex>;
    using _faces                    = std::vector<_face>;
    using _lines                    = std::vector<_line>;

    explicit obj(_vertices v = {}, _texture_coordinates vt = {}, _vertex_normals vn = {},
    _parameter_space_vertices vp = {}, _faces f = {}, _lines l = {}) : _v(std::move(v)),
    _vt(std::move(vt)), _vn(std::move(vn)), _vp(std::move(vp)), _f(std::move(f)), _l(std::move(l)){}

    auto vertices() noexcept -> _vertices& { return _v; }
    auto texture_coordinates() noexcept -> _texture_coordinates& { return _vt; }
    auto vertex_normals() noexcept -> _vertex_normals& { return _vn; }
    auto parameter_space_vertices() noexcept -> _parameter_space_vertices& { return _vp; }
    auto faces() noexcept -> _faces& { return _f; }
    auto lines() noexcept -> _lines& { return _l; }

    auto read(const std::string& file_name) -> void;
    [[nodiscard]] auto format_output() const -> std::string;
    inline auto print() const -> void;

private:
    _vertices                 _v;
    _texture_coordinates      _vt;
    _vertex_normals           _vn;
    _parameter_space_vertices _vp;
    _faces                    _f;
    _lines                    _l;

};

auto obj::read(const std::string& file_name) -> void {
    std::ifstream file(file_name);
    if (!file.is_open()) throw std::runtime_error("[ERR] Could not open file");

    std::string buf{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

    const char* p   = buf.data();
    const char* end = p + buf.size();

    auto skip_ws = [](const char* p, const char* end) noexcept -> const char* {
        while (p < end && (*p == ' ' || *p == '\t')) ++p;
        return p;
    };

    auto skip_line = [](const char* p, const char* end) noexcept -> const char* {
        while (p < end && *p != '\n') ++p;
        return p < end ? p + 1 : p;
    };

    auto read_float = [&](const char* p, float& out) noexcept -> const char* {
        p = skip_ws(p, end);
        auto [ptr, _] = std::from_chars(p, end, out);
        return ptr;
    };

    auto read_face_token = [&](const char* p, _face_vertex& fv) noexcept -> const char* {
        p = skip_ws(p, end);
        auto [p1, _1] = std::from_chars(p, end, fv.v); p = p1;
        if (p < end && *p == '/') {
            ++p;
            if (p < end && *p != '/') { auto [p2, _2] = std::from_chars(p, end, fv.vt); p = p2; }
            if (p < end && *p == '/') { ++p; auto [p3, _3] = std::from_chars(p, end, fv.vn); p = p3; }
        }
        return p;
    };

    while (p < end) {
        p = skip_ws(p, end);
        if (p >= end) break;
        if (*p == '#' || *p == '\n' || *p == '\r') { p = skip_line(p, end); continue; }

        const char* tok = p;
        while (p < end && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') ++p;
        const std::string_view kw{ tok, static_cast<std::size_t>(p - tok) };

        if (kw == "v") {
            _vertex v{};
            p = read_float(p, v.x);
            p = read_float(p, v.y);
            p = read_float(p, v.z);
            p = read_float(p, v.w);
            _v.push_back(v);
        } else if (kw == "vt") {
            _texture_coordinate vt{};
            p = read_float(p, vt.u);
            p = read_float(p, vt.v);
            p = read_float(p, vt.w);
            _vt.push_back(vt);
        } else if (kw == "vn") {
            _vertex_normal vn{};
            p = read_float(p, vn.x);
            p = read_float(p, vn.y);
            p = read_float(p, vn.z);
            _vn.push_back(vn);
        } else if (kw == "vp") {
            _parameter_space_vertex vp{};
            p = read_float(p, vp.u);
            p = read_float(p, vp.v);
            p = read_float(p, vp.w);
            _vp.push_back(vp);
        } else if (kw == "f") {
            _face f{};
            p = skip_ws(p, end);
            while (p < end && *p != '\n' && *p != '\r') {
                _face_vertex fv{};
                p = read_face_token(p, fv);
                f.push_back(fv);
                p = skip_ws(p, end);
            }
            _f.push_back(f);
        } else if (kw == "l") {
            _line l{};
            p = skip_ws(p, end);
            while (p < end && *p != '\n' && *p != '\r') {
                _line_vertex lv{};
                auto [ptr, _] = std::from_chars(p, end, lv);
                p = ptr;
                l.push_back(lv);
                p = skip_ws(p, end);
            }
            _l.push_back(l);
        }

        p = skip_line(p, end);
    }
}

[[nodiscard]] auto obj::format_output() const -> std::string {
    std::string out;
    out.reserve(4096);

    for (auto& v : _v) {
        if (v.w != 0.f) {
            out += std::format("Vertex: {{ x: {}, y: {}, z: {}, w: {} }}\n", v.x, v.y, v.z, v.w);
        } else {
            out += std::format("Vertex: {{ x: {}, y: {}, z: {} }}\n", v.x, v.y, v.z);
        }
    }
    for (auto& vt : _vt) {
        if (vt.v != 0.f && vt.w != 0.f) {
            out += std::format("Vertex Texture: {{ u: {}, v: {}, w: {} }}\n", vt.u, vt.v, vt.w);
        } else if (vt.v != 0.f) {
            out += std::format("Vertex Texture: {{ u: {}, v: {} }}\n", vt.u, vt.v);
        } else {
            out += std::format("Vertex Texture: {{ u: {} }}\n", vt.u);
        }
    }
    for (auto& vn : _vn)
        out += std::format("Vertex Normal: {{ x: {}, y: {}, z: {} }}\n", vn.x, vn.y, vn.z);

    for (auto& vp : _vp) {
        if (vp.v != 0.f && vp.w != 0.f) {
            out += std::format("Parameter Space Vertex: {{ u: {}, v: {}, w: {} }}\n", vp.u, vp.v, vp.w);
        } else if (vp.v != 0.f) {
            out += std::format("Parameter Space Vertex: {{ u: {}, v: {} }}\n", vp.u, vp.v);
        } else {
            out += std::format("Parameter Space Vertex: {{ u: {} }}\n", vp.u);
        }
    }
    for (auto& f : _f) {
        out += "Face: {";
        for (auto& fv : f) {
            if (fv.vt != 0 && fv.vn != 0) {
                out += std::format(" [{}, {}, {}]", fv.v, fv.vt, fv.vn);
            } else if (fv.vt != 0) {
                out += std::format(" [{}, {}]", fv.v, fv.vt);
            } else {
                out += std::format(" [{}]", fv.v);
            }
        }
        out += " }\n";
    }

    for (auto& l : _l) {
        out += "Line: { ";
        for (auto& lv : l) out += std::format("{}, ", lv);
        out += "}\n";
    }

    return out;
}

auto obj::print() const -> void {
    std::print("{}", format_output());
}
