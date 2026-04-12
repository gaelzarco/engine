module;

#include <charconv>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <print>
#include <exception>

export module obj_v0_1;

export class obj_ {
public:
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
        o.v_.reserve(5000);
        o.vt_.reserve(5000);
        o.vn_.reserve(5000);
        o.vp_.reserve(5000);
        o.f_.reserve(5000);
        o.l_.reserve(1000);
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

    inline auto read(const std::string& file_name) -> void;
    [[nodiscard]] auto format() const -> std::string;
    auto print() const -> void;

    friend auto operator>>(std::istream& in, obj_& obj) -> std::istream&;
    friend auto operator<<(std::ostream& os, obj_& obj) -> std::ostream&;

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

inline auto obj_::read(const std::string& file_name) -> void {
    std::ifstream file(file_name);
    if (!file.is_open()) throw std::runtime_error("[ERR] Could not open file");

    std::string buf{ std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>() };

    const char* p   = buf.data();
    const char* end = p + buf.size();

    auto skip_ws = [](const char* p, const char* end) noexcept ->  const char* {
        while (p < end && (*p == ' ' || *p == '\t')) ++p;
        return p;
    };

    auto skip_line = [](const char* p, const char* end) noexcept -> const char* {
        while (p < end && *p != '\n') ++p;
        return p < end ? p + 1 : p;
    };

    auto read_float = [](const char* p, const char* end, float& out) noexcept -> const char* {
        p += (*p == ' ' || *p == '\t'); // single ws skip is enough post-keyword
        while (p < end && (*p == ' ' || *p == '\t')) ++p;
        auto [ptr, _] = std::from_chars(p, end, out);
        return ptr;
    };

    auto read_int = [](const char* p, const char* end, int& out) noexcept -> const char* {
        while (p < end && (*p == ' ' || *p == '\t')) ++p;
        auto [ptr, _] = std::from_chars(p, end, out);
        return ptr;
    };

    auto read_face_token = [&](const char* p, const char* end, face& f) noexcept -> const char* {
        p = skip_ws(p, end);
        auto [p1, _1] = std::from_chars(p, end, f.v);   p = p1;
        if (p < end && *p == '/') {
            ++p;
            if (p < end && *p != '/') {
                auto [p2, _2] = std::from_chars(p, end, f.vt); p = p2;
            }
            if (p < end && *p == '/') {
                ++p;
                auto [p3, _3] = std::from_chars(p, end, f.vn); p = p3;
            }
        }
        return p;
    };

    while (p < end) {
        p = skip_ws(p, end);
        if (p >= end) break;
        if (*p == '#' || *p == '\n' || *p == '\r') { p = skip_line(p, end); continue; }

        const char* tok = p;
        while (p < end && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') ++p;
        const auto tlen = static_cast<std::size_t>(p - tok);

        auto kw = [&](std::string_view s) noexcept -> bool {
            return std::string_view{ tok, tlen } == s;
        };

        if (kw("v")) {
            vertex vx{};
            p = read_float(p, end, vx.x);
            p = read_float(p, end, vx.y);
            p = read_float(p, end, vx.z);
            p = read_float(p, end, vx.w);
            v_.push_back(vx);
        } else if (kw("vt")) {
            texture tx{};
            p = read_float(p, end, tx.u);
            p = read_float(p, end, tx.v);
            p = read_float(p, end, tx.w);
            vt_.push_back(tx);
        } else if (kw("vn")) {
            normal nm{};
            p = read_float(p, end, nm.x);
            p = read_float(p, end, nm.y);
            p = read_float(p, end, nm.z);
            vn_.push_back(nm);
        } else if (kw("vp")) {
            param pm{};
            p = read_float(p, end, pm.u);
            p = read_float(p, end, pm.v);
            p = read_float(p, end, pm.w);
            vp_.push_back(pm);
        } else if (kw("f")) {
            p = skip_ws(p, end);
            while (p < end && *p != '\n' && *p != '\r') {
                face f{};
                p = read_face_token(p, end, f);
                f_.push_back(f);
                p = skip_ws(p, end);
            }
        } else if (kw("l")) {
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
        std::format_to(std::back_inserter(out), "Vertex: {{ x: {}", v.x);
        std::format_to(std::back_inserter(out), ", y: {}", v.y);
        std::format_to(std::back_inserter(out), ", z: {}", v.z);
        if (v.w != 0.f) std::format_to(std::back_inserter(out), ", w: {}", v.w);
        out += " }\n";
    }
    for (auto& vt : vt_) {
        std::format_to(std::back_inserter(out), "Vertex Texture: {{ u: {}", vt.u);
        if (vt.v != 0.f) std::format_to(std::back_inserter(out), ", v: {}", vt.v);
        if (vt.w != 0.f) std::format_to(std::back_inserter(out), ", w: {}", vt.w);
        out += " }\n";
    }
    for (auto& vn : vn_) {
        std::format_to(std::back_inserter(out), "Vertex Normal: {{ x: {}", vn.x);
        std::format_to(std::back_inserter(out), ", y: {}", vn.y);
        std::format_to(std::back_inserter(out), ", z: {}", vn.z);
        out += " }\n";
    }
    for (auto& vp : vp_) {
        std::format_to(std::back_inserter(out), "Parameter Space Vertex: {{ u: {}", vp.u);
        if (vp.v != 0.f) std::format_to(std::back_inserter(out), ", v: {}", vp.v);
        if (vp.w != 0.f) std::format_to(std::back_inserter(out), ", w: {}", vp.w);
        out += " }\n";
    }
    for (auto& f : f_) {
        std::format_to(std::back_inserter(out), "Face: {{ v: {}", f.v);
        if (f.vt != 0) std::format_to(std::back_inserter(out), ", vt: {}", f.vt);
        if (f.vn != 0) std::format_to(std::back_inserter(out), ", vn: {}", f.vn);
        out += " }\n";
    }
    if (!l_.empty()) {
        out += "Line: { ";
        for (auto& lv : l_)
            std::format_to(std::back_inserter(out), "{}, ", lv);
        out += "}\n";
    }

    return out;
}

auto obj_::print() const -> void {
    std::print("{}", format());
}

[[deprecated]] auto operator>>(std::istream& in, obj_& obj) -> std::istream& {
    std::string line;
    obj_::vertex v;
    obj_::texture vt;
    obj_::normal vn;
    obj_::param vp;
    obj_::face f;
    obj_::line_vertex lv;

    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') [[unlikely]] line.pop_back();

        std::istringstream ss(line);
        std::string temp{};

        if (!(ss >> temp) || temp == "#") continue;

        if (temp == "v") {
            // x, y, z vertex values
            ss >> v.x >> v.y >> v.z;
            v.w = (ss >> v.w) ? v.w : 1.f;
            obj.v_push_back(v);
        } else if (temp == "vt") {
            // u, v, w texture values
            vt.v = 0.f;
            vt.w = 0.f;
            ss >> vt.u >> vt.v >> vt.w;
            obj.vt_push_back(vt);
        } else if (temp == "vn") {
            // x, y, z normal values
            ss >> vn.x >> vn.y >> vn.z;
            obj.vn_push_back(vn);
        } else if (temp == "vp") {
            // u, v, w parameter space vertex values
            vp.v = 0.f;
            vp.w = 0.f;
            ss >> vp.u >> vp.v >> vp.w;
            obj.vp_push_back(vp);
        } else if (temp == "f") {
            // v, vt, vn face values
            std::string f_temp{};
            while (ss >> f_temp) {
                std::istringstream face(f_temp);
                std::string curr_value;

                auto f_push = [&face, &curr_value](int& f_value) mutable -> void {
                    std::getline(face, curr_value, '/');
                    f_value = curr_value.empty() ? 0 : std::stoi(curr_value);
                };

                f_push(f.v);
                f_push(f.vt);
                f_push(f.vn);
                obj.f_push_back(f);
            }
        } else if (temp == "l") {
            while (ss >> lv) {
                obj.l_push_back(lv);
            }
        }
        // silently skip: mtllib, usemtl, o, g, s, etc.
    }

    return in;
}

[[deprecated]] auto operator<<(std::ostream& os, obj_& obj) -> std::ostream& {
    for (auto& v : obj.get_v()) {
        os << "Vertex: { x: " << v.x << ", y: " << v.y << ", z: " << v.z;
        if (v.w != 0.f) os << ", w: " << v.w;
        os << " }\n";
    }
    for (auto& vt : obj.get_vt()) {
        os << "Vertex Texture: { u: " << vt.u;
        if (vt.v != 0.f) os << ", v: " << vt.v;
        if (vt.w != 0.f) os << ", w: " << vt.w;
        os << " }\n";
    }
    for (auto& vn : obj.get_vn()) {
        os << "Vertex Normal: { x: " << vn.x << ", y: " << vn.y
        << ", z: " << vn.z << " }\n";
    }
    for (auto& vp : obj.get_vp()) {
        os << "Parameter Space Vertex: { u: " << vp.u;
        if (vp.v != 0.f) os << ", v: " << vp.v;
        if (vp.w != 0.f) os << ", w: " << vp.w;
        os << " }\n";
    }
    for (auto& f : obj.get_f()) {
        os << "Face values: { v: " << f.v;
        if (f.vt != 0) os << ", vt: " << f.vt;
        if (f.vn != 0) os << ", vn: " << f.vn;
        os << " }\n";
    }
    if (!obj.get_l().empty()) {
        os << "Line elements: { ";
        for (auto& lv : obj.get_l()) {
            os << lv << ", ";
        }
        os << " }\n";
    }
    return os;
}
