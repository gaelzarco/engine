module;
#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include <sstream>

export module obj;

export class obj_data {
public:
    struct vertex  { float x{}, y{}, z{}, w{}; };
    struct texture { float u{}, v{}, w{}; };
    struct normal  { float x{}, y{}, z{}; };
    struct param   { float u{}, v{}, w{}; };
    struct face    { float v{}, vt{}, vn{}; };

    using vertices      = std::vector<vertex>;
    using vert_textures = std::vector<texture>;
    using vert_normals  = std::vector<normal>;
    using vert_params   = std::vector<param>;
    using face_elements = std::vector<face>;

    constexpr explicit obj_data(vertices v = {}, vert_textures vt = {},
    vert_normals vn = {}, vert_params vp = {}) : v_(std::move(v)),
    vt_(std::move(vt)), vn_(std::move(vn)), vp_(std::move(vp)) {}

    constexpr inline void v_push_back(vertex& v) { v_.push_back(v); }
    constexpr inline void vt_push_back(texture& t) { vt_.push_back(t); }
    constexpr inline void vn_push_back(normal& n) { vn_.push_back(n); }
    constexpr inline void vp_push_back(param& p) { vp_.push_back(p); }
    constexpr inline void f_push_back(face& f) { f_.push_back(f); }

    constexpr inline vertices& get_v() noexcept { return v_; }
    constexpr inline vert_textures& get_vt() noexcept { return vt_; }
    constexpr inline vert_normals& get_vn() noexcept { return vn_; }
    constexpr inline vert_params& get_vp() noexcept { return vp_; }
    constexpr inline face_elements& get_f() noexcept { return f_; }

    friend std::istream& operator>>(std::istream& in, obj_data& obj);
    friend std::ostream& operator<<(std::ostream& os, obj_data& obj);

private:
    vertices      v_;
    vert_textures vt_;
    vert_normals  vn_;
    vert_params   vp_;
    face_elements f_;
};

std::istream& operator>>(std::istream& in, obj_data& obj) {
    std::string line;
    obj_data::vertex v;
    obj_data::texture vt;
    obj_data::normal vn;
    obj_data::param vp;
    obj_data::face f;

    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') [[likely]] line.pop_back();

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
            // TODO
            // std::string f_temp{};
            // std::string curr;
            // while (ss >> f_temp) {
            //     while (std::getline(ss, curr, '/')) {
            //         ss >> 
            //     }
            //     std::cout << f_temp << ' ';
            // }
            // std::cout << '\n';
        }
    }

    return in;
}

// OPERATOR>> OG

// std::istream& operator>>(std::istream& in, obj_data& obj) {
//     std::string content((std::istreambuf_iterator<char>(in)), {});
//     content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
//     std::istringstream clean(std::move(content));
//
//     std::string temp;
//     std::string f_temp;
//     std::string line_temp;
//     obj_data::vertex v;
//     obj_data::texture vt;
//     obj_data::normal vn;
//     obj_data::param vp;
// 
//     while (clean >> temp) {
//         if (temp == "#") {
//             clean.ignore(510, '\n');
//         } else if (temp == "v") {
//             clean >> f_temp;
//             v.x = std::stof(f_temp);
//             clean >> f_temp;
//             v.y = std::stof(f_temp);
//             clean >> f_temp;
//             v.z = std::stof(f_temp);
//             if (std::getline(clean, line_temp) && !line_temp.empty()) {
//                 v.w = std::stof(line_temp);
//                 line_temp.clear();
//             } else {
//                 v.w = 1.;
//             }
//             obj.v_push_back(v);
//         } else if (temp == "vt") {
//             clean >> f_temp;
//             vt.u = std::stof(f_temp);
//             if (std::getline(clean, line_temp) && !line_temp.empty()) {
//                 std::istringstream ss(line_temp);
//                 if (ss >> f_temp) vt.v = std::stof(f_temp);
//                 if (ss >> f_temp) vt.w = std::stof(f_temp);
//                 line_temp.clear();
//             }
//             obj.vt_push_back(vt);
//         } else if (temp == "vn") {
//             clean >> f_temp;
//             vn.x = std::stof(f_temp);
//             clean >> f_temp;
//             vn.y = std::stof(f_temp);
//             clean >> f_temp;
//             vn.z = std::stof(f_temp);
//             obj.vn_push_back(vn);
//         } else if (temp == "vp") {
//             clean >> f_temp;
//             vp.u = std::stof(f_temp);
//             if (std::getline(clean, line_temp) && !line_temp.empty()) {
//                 std::istringstream ss(line_temp);
//                 if (ss >> f_temp) vp.v = std::stof(f_temp);
//                 if (ss >> f_temp) vp.w = std::stof(f_temp);
//                 line_temp.clear();
//             }
//             obj.vp_push_back(vp);
//         } else {
//             continue;
//         }
//     }
//
//     return in;
// }

std::ostream& operator<<(std::ostream& os, obj_data& obj) {
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
    return os;
}
