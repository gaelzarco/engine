module;

#include <charconv>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <print>
#include <exception>

export module obj_v0_2;

export class obj_ {
public:
    static constexpr std::size_t K_RESERVE      = 5000;
    static constexpr std::size_t K_LINE_RESERVE = 1000;

    struct vertex      { float x{}, y{}, z{}, w{}; };
    struct texture     { float u{}, v{}, w{}; };
    struct normal      { float x{}, y{}, z{}; };
    struct param       { float u{}, v{}, w{}; };
    struct face        { int v{}, vt{}, vn{}; };
    using  line_vertex = int;

    using vertices      = std::vector<vertex>;
    using vert_textures = std::vector<texture>;
    using vert_normals  = std::vector<normal>;
    using vert_params   = std::vector<param>;
    using face_elements = std::vector<face>;
    using line = std::vector<line_vertex>;

    obj_() = delete;

    static auto make() -> obj_ {
        obj_ o{{}, {}, {}, {}, {}};
        o.v_.reserve(K_RESERVE);
        o.vt_.reserve(K_RESERVE);
        o.vn_.reserve(K_RESERVE);
        o.vp_.reserve(K_RESERVE);
        o.f_.reserve(K_RESERVE);
        o.l_.reserve(K_LINE_RESERVE);
        return o;
    }

    inline auto v_push_back(vertex& v) -> void { v_.push_back(v); }
    inline auto vt_push_back(texture& t) -> void { vt_.push_back(t); }
    inline auto vn_push_back(normal& n) -> void { vn_.push_back(n); }
    inline auto vp_push_back(param& p) -> void { vp_.push_back(p); }
    inline auto f_push_back(face& f) -> void { f_.push_back(f); }
    inline auto l_push_back(line_vertex& lv) -> void { l_.push_back(lv); }

    inline auto get_v() noexcept -> vertices& { return v_; }
    inline auto get_vt() noexcept -> vert_textures& { return vt_; }
    inline auto get_vn() noexcept -> vert_normals& { return vn_; }
    inline auto get_vp() noexcept -> vert_params& { return vp_; }
    inline auto get_f() noexcept -> face_elements& { return f_; }
    inline auto get_l() noexcept -> line& { return l_; }

    auto read(const std::string& file_name) -> void;
    [[nodiscard]] auto format() const -> std::string;
    inline auto print() const -> void;

private:
    vertices      v_;
    vert_textures vt_;
    vert_normals  vn_;
    vert_params   vp_;
    face_elements f_;
    line l_;

    explicit obj_(vertices v, vert_textures vt, vert_normals vn, vert_params vp, line l) :
    v_(std::move(v)), vt_(std::move(vt)), vn_(std::move(vn)), vp_(std::move(vp)), l_(std::move(l)){}
};

auto obj_::read(const std::string& file_name) -> void {
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

    auto read_face_token = [&](const char* p, face& f) noexcept -> const char* {
        p = skip_ws(p, end);
        auto [p1, _1] = std::from_chars(p, end, f.v); p = p1;
        if (p < end && *p == '/') {
            ++p;
            if (p < end && *p != '/') { auto [p2, _2] = std::from_chars(p, end, f.vt); p = p2; }
            if (p < end && *p == '/') { ++p; auto [p3, _3] = std::from_chars(p, end, f.vn); p = p3; }
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
            vertex vx{};
            p = read_float(p, vx.x); p = read_float(p, vx.y);
            p = read_float(p, vx.z); p = read_float(p, vx.w);
            v_.push_back(vx);
        } else if (kw == "vt") {
            texture tx{};
            p = read_float(p, tx.u); p = read_float(p, tx.v); p = read_float(p, tx.w);
            vt_.push_back(tx);
        } else if (kw == "vn") {
            normal nm{};
            p = read_float(p, nm.x); p = read_float(p, nm.y); p = read_float(p, nm.z);
            vn_.push_back(nm);
        } else if (kw == "vp") {
            param pm{};
            p = read_float(p, pm.u); p = read_float(p, pm.v); p = read_float(p, pm.w);
            vp_.push_back(pm);
        } else if (kw == "f") {
            p = skip_ws(p, end);
            while (p < end && *p != '\n' && *p != '\r') {
                face f{};
                p = read_face_token(p, f);
                f_.push_back(f);
                p = skip_ws(p, end);
            }
        } else if (kw == "l") {
            p = skip_ws(p, end);
            while (p < end && *p != '\n' && *p != '\r') {
                line_vertex lv{};
                auto [ptr, _] = std::from_chars(p, end, lv);
                p = ptr;
                l_.push_back(lv);
                p = skip_ws(p, end);
            }
        }

        p = skip_line(p, end);
    }
}

[[nodiscard]] auto obj_::format() const -> std::string {
    std::string out;
    out.reserve(4096);

    for (auto& v : v_) {
        if (v.w != 0.f)
            out += std::format("Vertex: {{ x: {}, y: {}, z: {}, w: {} }}\n", v.x, v.y, v.z, v.w);
        else
            out += std::format("Vertex: {{ x: {}, y: {}, z: {} }}\n", v.x, v.y, v.z);
    }
    for (auto& vt : vt_) {
        if (vt.v != 0.f && vt.w != 0.f)
            out += std::format("Vertex Texture: {{ u: {}, v: {}, w: {} }}\n", vt.u, vt.v, vt.w);
        else if (vt.v != 0.f)
            out += std::format("Vertex Texture: {{ u: {}, v: {} }}\n", vt.u, vt.v);
        else
            out += std::format("Vertex Texture: {{ u: {} }}\n", vt.u);
    }
    for (auto& vn : vn_)
        out += std::format("Vertex Normal: {{ x: {}, y: {}, z: {} }}\n", vn.x, vn.y, vn.z);

    for (auto& vp : vp_) {
        if (vp.v != 0.f && vp.w != 0.f)
            out += std::format("Parameter Space Vertex: {{ u: {}, v: {}, w: {} }}\n", vp.u, vp.v, vp.w);
        else if (vp.v != 0.f)
            out += std::format("Parameter Space Vertex: {{ u: {}, v: {} }}\n", vp.u, vp.v);
        else
            out += std::format("Parameter Space Vertex: {{ u: {} }}\n", vp.u);
    }
    for (auto& f : f_) {
        if (f.vt != 0 && f.vn != 0)
            out += std::format("Face: {{ v: {}, vt: {}, vn: {} }}\n", f.v, f.vt, f.vn);
        else if (f.vt != 0)
            out += std::format("Face: {{ v: {}, vt: {} }}\n", f.v, f.vt);
        else
            out += std::format("Face: {{ v: {} }}\n", f.v);
    }
    if (!l_.empty()) {
        out += "Line: { ";
        for (auto& lv : l_)
            out += std::format("{}, ", lv);
        out += "}\n";
    }

    return out;
}

auto obj_::print() const -> void {
    std::print("{}", format());
}
