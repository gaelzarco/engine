/**
 * @file render_v0.cppm
 * @brief C++23 module providing primitive 2D rendering utilities for a MiniFB
 * pixel buffer.
 * @version 0.0
 */

module;
#include "../minifb/include/MiniFB_cpp.h"
#include <algorithm>
#include <print>
export module render_v0;

import std;

/**
 * @struct rgb_color
 * @brief An RGB colour value for use with a MiniFB pixel buffer.
 *
 * @par Example — explicit colour
 * @code
 * rgb_color red(255, 0, 0);
 * @endcode
 *
 * @par Example — random colour
 * @code
 * rgb_color random_color; // r, g, b chosen randomly at construction
 * @endcode
 */
export struct rgb_color {
    using color_t = uint8_t;
    rgb_color(color_t r = 0, color_t g = 0, color_t b = 0) : _r(r), _g(g),
    _b(b) {
        if (r == 0 && g == 0 & b == 0) {
            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_int_distribution<int> rgb_rng(0, 255);
            _r = rgb_rng(rng);
            _g = rgb_rng(rng);
            _b = rgb_rng(rng);
        }
    }

    color_t _r;
    color_t _g;
    color_t _b;
};

/**
 * @struct point
 * @brief A 2D integer screen-space coordinate.
 * @var point::x Horizontal position in pixels. Default: 0.
 * @var point::y Vertical position in pixels. Default: 0.
 */
export struct point { int x, y{}; };

/*
 * @brief Helper function that builds two edge vectors from a common origin @c
 * a to compute 2D cross-product for triangle rasterization.
 *
 * @param a First triangle vertex point (screen-space pixels).
 * @param b Second triangle vertex point (screen-space pixels).
 * @param c Third triangle vertex point (screen-space pixels).
 *
 * @return  Determinant value
 */
export auto get_determinant(const point& a, const point& b, const point& c) ->
int {
    const point ab = {b.x - a.x, b.y - a.y};
    const point ac = {c.x - a.x, c.y - a.y};

    const int result = ab.y * ac.x - ab.x * ac.y;

    return result;
}

export class mfb_canvas {
public:
    mfb_canvas(std::size_t w, std::size_t h, std::vector<std::uint32_t> b) :
    _width(std::move(w)), _height(std::move(h)), _buffer(std::move(b)) {}

    /**
    * @brief Getter for width of canvas buffer
    * @return Reference to _width
    */
    constexpr auto width() -> std::size_t& { return _width; }
    /**
    * @brief Getter for height of canvas buffer
    * @return Reference to _height
    */
    constexpr auto height() -> std::size_t& { return _height; }
    /**
    * @brief Getter for canvas buffer data
    * @brief Reference to _buffer
    */
    constexpr auto buffer() -> std::vector<std::uint32_t>& { return _buffer; }

    /**
    * @brief Verifies point is within bounds of canvas @c _width and @c _height
    *
    * @param x X-coordinate of point
    * @param y Y-coordinate of point
    *
    * @return
    */
    constexpr auto point_in_bounds(const int& x, const int& y) -> bool;

    /**
    * @brief Draws an anti-aliasing-free straight line into a MiniFB pixel
    *        buffer with bounds checking.
    *
    * @param a       Start point of the line (screen-space pixels).
    * @param b       End point of the line (screen-space pixels).
    * @param color   RGB colour to paint the line with.
    *
    * @note @p a and @p b are taken by value; the algorithm may swap their
    *       coordinates internally without affecting the caller's variables.
    */
    auto line(point a, point b, const rgb_color& color) -> void;

    /**
    * @brief Rasterizes triangle within MiniFB pixel buffer using provided color
    * and points. Utilizes bounding box algorithm to fill in triangle.
    *
    * @param a      First triangle vertex point (screen-space pixels).
    * @param b      Second triangle vertex point (screen-space pixels).
    * @param c      Third triangle vertex point (screen-space pixels).
    * @param color  RGB color to paint the triangle.
    *
    * @note @p a, @p b, and @p c are taken by value; the algorithm may swap
    * their coordinates internally without affecting the caller's variables.
    */
    auto triangle(point a, point b, point c, const rgb_color& color) -> void;

private:
    std::size_t                _width;
    std::size_t                _height;
    std::vector<std::uint32_t> _buffer;
};

constexpr auto mfb_canvas::point_in_bounds(const int& x, const int& y) -> bool {
    return x >= 0 && y >= 0 && static_cast<std::size_t>(x) < _width &&
    static_cast<std::size_t>(y) < _height;
}

auto mfb_canvas::line(point a, point b, const rgb_color& color) -> void {
    bool steep = std::abs(a.x - b.x) < std::abs(a.y - b.y);

    if (steep) {
        std::swap(a.x, a.y);
        std::swap(b.x, b.y);
    }

    if (a.x > b.x) std::swap(a, b);

    int y = a.y;
    int err = 0;

    for (int x{a.x}; x <= b.x; x++) {
        int col = steep ? y : x;
        int row = steep ? x : y;

        if (!point_in_bounds(col, row)) {
            std::println("[LOG] skipped point out of bounds");
        } else {
            _buffer[static_cast<std::size_t>(row) * _width +
            static_cast<std::size_t>(col)] = MFB_RGB(color._r,
            color._g, color._b);
        }

        err += 2 * std::abs(b.y - a.y);
        if (err > b.x - a.x) {
            y += b.y > a.y ? 1 : -1;
            err -= 2 * (b.x - a.x);
        }
    }
}

auto mfb_canvas::triangle(point a, point b, point c, const rgb_color& color) ->
void {
    if (get_determinant(a, b, c) == 0) return;
    if (get_determinant(a, b, c) < 0) std::swap(b, c);

    // Create triangle bounding box
    const int xmin = std::min({a.x, b.x, c.x});
    const int ymin = std::min({a.y, b.y, c.y});

    const int xmax = std::max({a.x, b.x, c.x});
    const int ymax = std::max({a.y, b.y, c.y});

    for (auto y{ymin}; y <= ymax; ++y) {
        for (auto x{xmin}; x <= xmax; ++x) {
            if (!point_in_bounds(x, y)) continue;

            const point p{x, y};

            const auto bounds_a = get_determinant(b, c, p);
            const auto bounds_b = get_determinant(c, a, p);
            const auto bounds_c = get_determinant(a, b, p);

            if (bounds_a >= 0 && bounds_b >= 0 && bounds_c >= 0) {
                _buffer[static_cast<std::size_t>(y) * _width +
                static_cast<std::size_t>(x)] = MFB_RGB(color._r, color._g,
                color._b);
            }
        }
    }
}
