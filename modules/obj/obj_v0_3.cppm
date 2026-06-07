/**
 * @file obj_v0_3.cppm
 * @brief C++20 module providing a parser and in-memory representation for the Wavefront OBJ format.
 * @version 0.3
 */
module;

#include <charconv>
#include <vector>
#include <string>
#include <fstream>
#include <print>

export module obj_v0_3;

/**
 * @class obj
 * @brief Represents a parsed Wavefront OBJ file.
 *
 * Stores all geometric data defined by the OBJ specification, including vertices,
 * texture coordinates, vertex normals, parameter space vertices, faces, and lines.
 * Provides a high-performance parser using @c std::from_chars for zero-allocation
 * numeric conversion.
 *
 * @note Exported from C++20 module @c obj_v0_3.
 *
 * @par Example
 * @code
 * obj model;
 * model.read("mesh.obj");
 * model.print();
 * model.memory_out();
 * @endcode
 */
export class obj {
public:
    /**
     * @struct obj::_vertex
     * @brief A geometric vertex with optional homogeneous coordinate.
     * @var obj::_vertex::x  X component. Default: 0.
     * @var obj::_vertex::y  Y component. Default: 0.
     * @var obj::_vertex::z  Z component. Default: 0.
     * @var obj::_vertex::w  Homogeneous W component. Default: 1.
     */
    struct _vertex                  { float x{}, y{}, z{}, w{ 1.f }; };
    /**
     * @struct obj::_texture_coordinate
     * @brief A texture (UV or UVW) coordinate.
     * @var obj::_texture_coordinate::u  Horizontal texture axis. Default: 0.
     * @var obj::_texture_coordinate::v  Vertical texture axis. Default: 0.
     * @var obj::_texture_coordinate::w  Depth texture axis (for 3D textures). Default: 0.
     */
    struct _texture_coordinate      { float u{}, v{}, w{}; };
    /**
     * @struct obj::_vertex_normal
     * @brief A vertex normal vector. Not required to be unit length.
     * @var obj::_vertex_normal::x  X component. Default: 0.
     * @var obj::_vertex_normal::y  Y component. Default: 0.
     * @var obj::_vertex_normal::z  Z component. Default: 0.
     */
    struct _vertex_normal           { float x{}, y{}, z{}; };
    /**
     * @struct _parameter_space_vertex
     * @brief A point in parameter space, used for free-form geometry.
     * @var _parameter_space_vertex::u  U axis component. Default: 0.
     * @var _parameter_space_vertex::v  V axis component. Default: 0.
     * @var _parameter_space_vertex::w  W axis component. Default: 0.
     */
    struct _parameter_space_vertex  { float u{}, v{}, w{}; };

    /**
     * @struct _face_vertex
     * @brief A single vertex reference within a face, encoding optional texture and normal indices.
     * @var _face_vertex::v   Index into the vertex list (1-based per OBJ spec). Default: 0.
     * @var _face_vertex::vt  Index into the texture coordinate list. 0 if absent. Default: 0.
     * @var _face_vertex::vn  Index into the vertex normal list. 0 if absent. Default: 0.
     */
    struct _face_vertex             { int v{}, vt{}, vn{}; };

    /** @typedef _face
     *  @brief An ordered list of face vertices forming a polygon. */
    using  _face                    = std::vector<_face_vertex>;
    /** @typedef _line_vertex
     *  @brief A single vertex index reference within a line element (1-based). */
    using  _line_vertex             = int;
    /** @typedef _line
     *  @brief An ordered list of vertex indices forming a polyline. */
    using  _line                    = std::vector<_line_vertex>;
    /** @typedef _vertices
     *  @brief Collection of all geometric vertices in the model. */
    using _vertices                 = std::vector<_vertex>;
    /** @typedef _texture_coordinates
     *  @brief Collection of all texture coordinates in the model. */
    using _texture_coordinates      = std::vector<_texture_coordinate>;
    /** @typedef _vertex_normals
     *  @brief Collection of all vertex normals in the model. */
    using _vertex_normals           = std::vector<_vertex_normal>;
    /** @typedef _parameter_space_vertices
     *  @brief Collection of all parameter space vertices in the model. */
    using _parameter_space_vertices = std::vector<_parameter_space_vertex>;
    /** @typedef _faces
     *  @brief Collection of all face polygons in the model. */
    using _faces                    = std::vector<_face>;
    /** @typedef _lines
     *  @brief Collection of all polyline elements in the model. */
    using _lines                    = std::vector<_line>;

    /**
     * @brief Constructs an @c obj with optionally pre-populated geometry data.
     *
     * All parameters are moved into internal storage. Passing no arguments
     * constructs an empty model.
     *
     * @param v   Geometric vertices.
     * @param vt  Texture coordinates.
     * @param vn  Vertex normals.
     * @param vp  Parameter space vertices.
     * @param f   Face polygons.
     * @param l   Polyline elements.
     */
    explicit obj(_vertices v = {}, _texture_coordinates vt = {}, _vertex_normals vn = {},
    _parameter_space_vertices vp = {}, _faces f = {}, _lines l = {}) : v_(std::move(v)),
    vt_(std::move(vt)), vn_(std::move(vn)), vp_(std::move(vp)), f_(std::move(f)), l_(std::move(l)){}

    /**
     * @brief Returns a mutable reference to the geometric vertex list.
     * @return Reference to the internal @c _vertices container.
     */
    auto vertices() noexcept -> _vertices& { return v_; }
    /**
     * @brief Returns a read-only reference to the geometric vertex list.
     * @return Const reference to the internal @c _vertices container.
     */
    [[nodiscard]]
    auto vertices() const noexcept -> const _vertices& { return v_; }

    /**
     * @brief Returns a mutable reference to the texture coordinate list.
     * @return Reference to the internal @c _texture_coordinates container.
     */
    auto texture_coordinates() noexcept -> _texture_coordinates& { return vt_; }
    /**
     * @brief Returns a read-only reference to the texture coordinate list.
     * @return Const reference to the internal @c _texture_coordinates container.
     */
    [[nodiscard]]
    auto texture_coordinates() const noexcept -> const _texture_coordinates& { return vt_; }

    /**
     * @brief Returns a mutable reference to the vertex normal list.
     * @return Reference to the internal @c _vertex_normals container.
     */
    auto vertex_normals() noexcept -> _vertex_normals& { return vn_; }
    /**
     * @brief Returns a read-only reference to the vertex normal list.
     * @return Const reference to the internal @c _vertex_normals container.
     */
    [[nodiscard]]
    auto vertex_normals() const noexcept -> const _vertex_normals& { return vn_; }

    /**
     * @brief Returns a mutable reference to the parameter space vertex list.
     * @return Reference to the internal @c _parameter_space_vertices container.
     */
    auto parameter_space_vertices() noexcept -> _parameter_space_vertices& { return vp_; }
    /**
     * @brief Returns a read-only reference to the parameter space vertex list.
     * @return Const reference to the internal @c _parameter_space_vertices container.
     */
    [[nodiscard]]
    auto parameter_space_vertices() const noexcept -> const _parameter_space_vertices& { return vp_; }

    /**
     * @brief Returns a mutable reference to the face list.
     * @return Reference to the internal @c _faces container.
     */
    auto faces() noexcept -> _faces& { return f_; }
    /**
     * @brief Returns a read-only reference to the face list.
     * @return Const reference to the internal @c _faces container.
     */
    [[nodiscard]]
    auto faces() const noexcept -> const _faces& { return f_; }

    /**
     * @brief Returns a mutable reference to the polyline list.
     * @return Reference to the internal @c _lines container.
     */
    auto lines() noexcept -> _lines& { return l_; }
    /**
     * @brief Returns a read-only reference to the polyline list.
     * @return Const reference to the internal @c _lines container.
     */
    [[nodiscard]]
    auto lines() const noexcept -> const _lines& { return l_; }

    /**
     * @brief Parses a Wavefront OBJ file and populates the model's geometry data.
     *
     * Reads the entire file into a buffer and performs a single-pass parse using
     * @c std::from_chars for locale-independent, allocation-free numeric conversion.
     * Recognised keywords: @c v, @c vt, @c vn, @c vp, @c f, @c l.
     * Comment lines (@c #) and blank lines are skipped. Unrecognised keywords are
     * silently ignored and the remainder of the line is consumed.
     *
     * Face vertex tokens support all three OBJ index formats:
     * - @c v
     * - @c v/vt
     * - @c v/vt/vn
     * - @c v//vn
     *
     * @param file_name  Path to the @c .obj file to read.
     *
     * @throws std::runtime_error if the file cannot be opened.
     *
     * @note Appends parsed data to any geometry already stored in the object.
     *       Call on a default-constructed instance to parse a single file cleanly.
     */
    auto read(const std::string& file_name) -> void;

    /**
     * @brief Formats all stored geometry as a human-readable string.
     *
     * Each element is written on its own line. Optional components (e.g. the @c w
     * component of a vertex, or the @c vt / @c vn indices of a face vertex) are
     * omitted from the output when they hold their default zero value.
     *
     * @return A @c std::string containing the formatted representation of the model.
     */
    [[nodiscard]]
    auto format_output() const -> std::string;
    /**
     * @brief Prints the formatted geometry to standard output.
     *
     * Equivalent to @c std::print("{}", format_output()).
     *
     * @see format_output()
     */
    auto print() const -> void;

    /**
     * @brief Logs the total heap memory reserved by all internal geometry containers.
     *
     * Reports the sum of @c capacity() * sizeof(element) across all six containers.
     * Output is written to standard output in the form:
     * @code
     * [LOG] Memory allocated: <bytes> bytes (~<MB> MB)
     * @endcode
     *
     * @note Reports reserved (capacity) memory, not the memory occupied by
     *       currently stored elements (size). Values may therefore exceed the
     *       memory strictly required to hold the parsed geometry.
     */
    auto memory_out() const -> void;

private:
    _vertices                 v_;
    _texture_coordinates      vt_;
    _vertex_normals           vn_;
    _parameter_space_vertices vp_;
    _faces                    f_;
    _lines                    l_;
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
            v_.push_back(v);
        } else if (kw == "vt") {
            _texture_coordinate vt{};
            p = read_float(p, vt.u);
            p = read_float(p, vt.v);
            p = read_float(p, vt.w);
            vt_.push_back(vt);
        } else if (kw == "vn") {
            _vertex_normal vn{};
            p = read_float(p, vn.x);
            p = read_float(p, vn.y);
            p = read_float(p, vn.z);
            vn_.push_back(vn);
        } else if (kw == "vp") {
            _parameter_space_vertex vp{};
            p = read_float(p, vp.u);
            p = read_float(p, vp.v);
            p = read_float(p, vp.w);
            vp_.push_back(vp);
        } else if (kw == "f") {
            _face f{};
            p = skip_ws(p, end);
            while (p < end && *p != '\n' && *p != '\r') {
                _face_vertex fv{};
                p = read_face_token(p, fv);
                f.push_back(fv);
                p = skip_ws(p, end);
            }
            f_.push_back(std::move(f));
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
            l_.push_back(std::move(l));
        }

        p = skip_line(p, end);
    }
}

auto obj::format_output() const -> std::string {
    std::string out;
    out.reserve(4096);

    for (auto& v : v_) {
        if (v.w != 0.f) {
            out += std::format("Vertex: {{ x: {}, y: {}, z: {}, w: {} }}\n", v.x, v.y, v.z, v.w);
        } else {
            out += std::format("Vertex: {{ x: {}, y: {}, z: {} }}\n", v.x, v.y, v.z);
        }
    }
    for (auto& vt : vt_) {
        if (vt.v != 0.f && vt.w != 0.f) {
            out += std::format("Vertex Texture: {{ u: {}, v: {}, w: {} }}\n", vt.u, vt.v, vt.w);
        } else if (vt.v != 0.f) {
            out += std::format("Vertex Texture: {{ u: {}, v: {} }}\n", vt.u, vt.v);
        } else {
            out += std::format("Vertex Texture: {{ u: {} }}\n", vt.u);
        }
    }
    for (auto& vn : vn_)
        out += std::format("Vertex Normal: {{ x: {}, y: {}, z: {} }}\n", vn.x, vn.y, vn.z);

    for (auto& vp : vp_) {
        if (vp.v != 0.f && vp.w != 0.f) {
            out += std::format("Parameter Space Vertex: {{ u: {}, v: {}, w: {} }}\n", vp.u, vp.v, vp.w);
        } else if (vp.v != 0.f) {
            out += std::format("Parameter Space Vertex: {{ u: {}, v: {} }}\n", vp.u, vp.v);
        } else {
            out += std::format("Parameter Space Vertex: {{ u: {} }}\n", vp.u);
        }
    }
    for (auto& f : f_) {
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

    for (auto& l : l_) {
        out += "Line: { ";
        for (auto& lv : l) out += std::format("{}, ", lv);
        out += "}\n";
    }

    return out;
}

auto obj::print() const -> void {
    std::print("{}", format_output());
}
 auto obj::memory_out() const -> void {
    const std::size_t total = this->vertices().capacity() * sizeof(obj::_vertex)
    + this->texture_coordinates().capacity() * sizeof(obj::_texture_coordinate)
    + this->vertex_normals().capacity() * sizeof(obj::_vertex_normal)
    + this->parameter_space_vertices().capacity() * sizeof(obj::_parameter_space_vertex)
    + this->faces().capacity() * sizeof(obj::_face)
    + this->lines().capacity() * sizeof(obj::_line);
    std::println("[LOG] Memory allocated: {} bytes (~{} MB)", total, total / 1024 / 1024);
 }
