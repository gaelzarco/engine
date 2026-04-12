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

    constexpr explicit obj_(vertices v = {}, vert_textures vt = {},
    vert_normals vn = {}, vert_params vp = {}, line l = {}) : v_(std::move(v)),
    vt_(std::move(vt)), vn_(std::move(vn)), vp_(std::move(vp)), l_(std::move(l))
    {
        v_.reserve(5000);
        vt_.reserve(5000);
        vn_.reserve(5000);
        vp_.reserve(5000);
        f_.reserve(5000);
        l_.reserve(1000);
    }

    constexpr inline void v_push_back(vertex& v) { v_.push_back(v); }
    constexpr inline void vt_push_back(texture& t) { vt_.push_back(t); }
    constexpr inline void vn_push_back(normal& n) { vn_.push_back(n); }
    constexpr inline void vp_push_back(param& p) { vp_.push_back(p); }
    constexpr inline void f_push_back(face& f) { f_.push_back(f); }
    constexpr inline void l_push_back(line_vertex& lv) { l_.push_back(lv); }

    constexpr inline vertices& get_v() noexcept { return v_; }
    constexpr inline vert_textures& get_vt() noexcept { return vt_; }
    constexpr inline vert_normals& get_vn() noexcept { return vn_; }
    constexpr inline vert_params& get_vp() noexcept { return vp_; }
    constexpr inline face_elements& get_f() noexcept { return f_; }
    constexpr inline line& get_l() noexcept { return l_; }

    constexpr inline void read(const std::string& file_name);
    constexpr std::string format() const noexcept;
    constexpr void print() const noexcept;

    friend std::istream& operator>>(std::istream& in, obj_& obj);
    friend std::ostream& operator<<(std::ostream& os, obj_& obj);

private:
    vertices      v_;
    vert_textures vt_;
    vert_normals  vn_;
    vert_params   vp_;
    face_elements f_;
    line l_;
};

constexpr inline void obj_::read(const std::string& file_name) {
    std::ifstream file(file_name);
    if (!file.is_open()) throw std::runtime_error("[ERR] Could not open file");

    std::string buf{ std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>() };

    const char* p   = buf.data();
    const char* end = p + buf.size();

    auto skip_ws = [](const char* p, const char* end) constexpr noexcept {
        while (p < end && (*p == ' ' || *p == '\t')) ++p;
        return p;
    };

    auto skip_line = [](const char* p, const char* end) constexpr noexcept {
        while (p < end && *p != '\n') ++p;
        return p < end ? p + 1 : p;
    };

    auto read_float = [](const char* p, const char* end, float& out) noexcept {
        p += (*p == ' ' || *p == '\t'); // single ws skip is enough post-keyword
        while (p < end && (*p == ' ' || *p == '\t')) ++p;
        auto [ptr, _] = std::from_chars(p, end, out);
        return ptr;
    };

    auto read_int = [](const char* p, const char* end, int& out) noexcept {
        while (p < end && (*p == ' ' || *p == '\t')) ++p;
        auto [ptr, _] = std::from_chars(p, end, out);
        return ptr;
    };

    auto read_face_token = [&](const char* p, const char* end, face& f) noexcept {
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

        auto kw = [&](std::string_view s) constexpr noexcept {
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

constexpr std::string obj_::format() const noexcept{
    std::string out;
    out.reserve(4096);

    for (auto& v : v_) {
        std::format_to(std::back_inserter(out), "Vertex: {{ x: {}", v.x);
        std::format_to(std::back_inserter(out), ", y: {}", v.y);
        std::format_to(std::back_inserter(out), ", z: {}", v.z);
        if (v.w) std::format_to(std::back_inserter(out), ", w: {}", v.w);
        out += " }\n";
    }
    for (auto& vt : vt_) {
        std::format_to(std::back_inserter(out), "Vertex Texture: {{ u: {}", vt.u);
        if (vt.v) std::format_to(std::back_inserter(out), ", v: {}", vt.v);
        if (vt.w) std::format_to(std::back_inserter(out), ", w: {}", vt.w);
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
        if (vp.v) std::format_to(std::back_inserter(out), ", v: {}", vp.v);
        if (vp.w) std::format_to(std::back_inserter(out), ", w: {}", vp.w);
        out += " }\n";
    }
    for (auto& f : f_) {
        std::format_to(std::back_inserter(out), "Face: {{ v: {}", f.v);
        if (f.vt) std::format_to(std::back_inserter(out), ", vt: {}", f.vt);
        if (f.vn) std::format_to(std::back_inserter(out), ", vn: {}", f.vn);
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

constexpr void obj_::print() const noexcept {
    std::print("{}", format());
}

[[deprecated]] std::istream& operator>>(std::istream& in, obj_& obj) {
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

                auto f_push = [&face, &curr_value](int& f_value) mutable {
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

[[deprecated]] std::ostream& operator<<(std::ostream& os, obj_& obj) {
    for (auto& v : obj.get_v()) {
        os << "Vertex: { x: " << v.x << ", y: " << v.y << ", z: " << v.z;
        if (v.w) os << ", w: " << v.w;
        os << " }\n";
    }
    for (auto& vt : obj.get_vt()) {
        os << "Vertex Texture: { u: " << vt.u;
        if (vt.v) os << ", v: " << vt.v;
        if (vt.w) os << ", w: " << vt.w;
        os << " }\n";
    }
    for (auto& vn : obj.get_vn()) {
        os << "Vertex Normal: { x: " << vn.x << ", y: " << vn.y
        << ", z: " << vn.z << " }\n";
    }
    for (auto& vp : obj.get_vp()) {
        os << "Parameter Space Vertex: { u: " << vp.u;
        if (vp.v) os << ", v: " << vp.v;
        if (vp.w) os << ", w: " << vp.w;
        os << " }\n";
    }
    for (auto& f : obj.get_f()) {
        os << "Face values: { v: " << f.v;
        if (f.vt) os << ", vt: " << f.vt;
        if (f.vn) os << ", vn: " << f.vn;
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
