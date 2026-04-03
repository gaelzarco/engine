module;

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <print>
#include <exception>

export module obj;

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

    constexpr void read_from_file(const std::string& file_name);
    void print() noexcept;

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

constexpr void obj_::read_from_file(const std::string& file_name) {
    std::ifstream file(file_name);

    if (!file.is_open()) {
        throw std::runtime_error("[ERR] Could not open file");
    }

    file >> *this;
}

void obj_::print() noexcept {
    for (auto& v : get_v()) {
        std::print("Vertex: {{ x: {}, y: {}, z: {}", v.x, v.y, v.z);
        if (v.w) std::print(", w: {}", v.w);
        std::println(" }}");
    }

    for (auto& vt : get_vt()) {
        std::print("Vertex Texture: {{ u: {}", vt.u);
        if (vt.v) std::print(", v: {}", vt.v);
        if (vt.w) std::print(", w: {}", vt.w);
        std::println(" }}");
    }

    for (auto& vn : get_vn()) {
        std::println("Vertex Normal: {{ x: {}, y: {}, z: {} }}", vn.x, vn.y, vn.z);
    }

    for (auto& vp : get_vp()) {
        std::print("Parameter Space Vertex: {{ u: {}", vp.u);
        if (vp.v) std::print(", v: {}", vp.v);
        if (vp.w) std::print(", w: {}", vp.w);
        std::println(" }}");
    }

    for (auto& f : get_f()) {
        std::print("Face values: {{ v: {}", f.v);
        if (f.vt) std::print(", vt: {}", f.vt);
        if (f.vn) std::print(", vn: {}", f.vn);
        std::println(" }}");
    }

    if (!get_l().empty()) {
        std::print("Line elements: {{ ");
        for (auto& lv : get_l()) {
            std::print("{}, ", lv);
        }
        std::println(" }}");
    }
}

std::istream& operator>>(std::istream& in, obj_& obj) {
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
    }

    return in;
}

std::ostream& operator<<(std::ostream& os, obj_& obj) {
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
