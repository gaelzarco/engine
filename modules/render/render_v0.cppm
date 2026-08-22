/**
 * @file render_v0.cppm
 * @brief C++23 module providing primitive 2D rendering utilities for a MiniFB pixel buffer.
 * @version 0.0
 */

module;

#include "../minifb/include/MiniFB_cpp.h"
#include <algorithm>
#include <print>

import std;

export module render_v0;

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
    rgb_color(color_t r = 0, color_t g = 0, color_t b = 0) : _r(r), _g(g), _b(b) {
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
* @brief Helper function to check if point is within bounds
*
* @param col    X value of point
* @param row    Y value of point
* @param width  Total pixel width of MiniFB window
* @param height Total pixel height of MiniFB window
*
* @return Boolean value dictating if point is within bounds or not
*/
auto in_bounds(const int& col, const int& row, const std::size_t& width, const std::size_t& height) -> bool {
    return col >= 0 && col < static_cast<int>(width) && row >= 0
        && row < static_cast<int>(height);
};

/**
 * @brief Draws an anti-aliasing-free straight line into a MiniFB pixel buffer with bounds checking.
 *
 * @param a       Start point of the line (screen-space pixels).
 * @param b       End point of the line (screen-space pixels).
 * @param buff    Flat pixel buffer of size @p width * @p height, modified in place.
 * @param color   RGB colour to paint the line with.
 * @param width   Total pixel width of MiniFB window
 * @param height  Total pixel height of MiniFB window
 *
 * @note @p a and @p b are taken by value; the algorithm may swap their
 *       coordinates internally without affecting the caller's variables.
 *
 * @par Example
 * @code
 * std::vector<std::size_t> buffer(WIDTH * HEIGHT, 0);
 * rgb_color red(255, 0, 0);
 * line({0, 0}, {WIDTH - 1, HEIGHT - 1}, buffer, red, WIDTH, HEIGHT);
 * @endcode
 */
export auto line(point a, point b, std::vector<std::uint32_t>& buff, const rgb_color& color,
const std::size_t& width, const std::size_t& height) -> void {
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

        if (!in_bounds(col, row, width, height)) {
            std::println("[LOG] skipped point out of bounds");
        } else {
            buff[static_cast<std::size_t>(row) * width + static_cast<std::size_t>(col)] =
            MFB_RGB(color._r, color._g, color._b);
        }

        err += 2 * std::abs(b.y - a.y);
        if (err > b.x - a.x) {
            y += b.y > a.y ? 1 : -1;
            err -= 2 * (b.x - a.x);
        }
    }
}

/*
 * @brief Helper function that builds two edge vectors from a common origin @c a to compute 2D
 * cross-product for triangle rasterization. 
 *
 * @param a First triangle vertex point (screen-space pixels).
 * @param b Second triangle vertex point (screen-space pixels).
 * @param c Third triangle vertex point (screen-space pixels).
 *
 * @return  Determinant value
 */
auto get_determinant(const point& a, const point& b, const point& c) -> int {
    const point ab = {b.x - a.x, b.y - a.y};
    const point ac = {c.x - a.x, c.y - a.y};

    const int result = ab.y * ac.x - ab.x * ac.y;

    return result;
}

/*
 * @brief Rasterizes triangle within MiniFB pixel buffer using provided color
 * and points. Utilizes bounding box algorithm to fill in triangle.
 *
 * @param a      First triangle vertex point (screen-space pixels).
 * @param b      Second triangle vertex point (screen-space pixels).
 * @param c      Third triangle vertex point (screen-space pixels).
 * @param buff   Flat pixel buffer of size @p width * @p height, modified in place.
 * @param color  RGB color to paint the triangle.
 * @param width  Total pixel width of MiniFB window.
 * @param height Total pixel height of MiniFB window.
 *
 * @par Example
 * @code
 * const static std::size_t WIDTH = 1080;
 * const static std::size_t HEIGHT = 1920;
 * point a{7, 3};
 * point b{12, 37};
 * point c{62, 53};
 * rgb_color color{};
 * triangle(a, b, c, buffer, color, WIDTH, HEIGHT);
 * @endcode
 */
export auto triangle(point a, point b, point c,std::vector<std::uint32_t>& buff,
const rgb_color& color, const std::size_t& width, const std::size_t& height) -> void {
    if (get_determinant(a, b, c) == 0) return;
    if (get_determinant(a, b, c) < 0) std::swap(b, c);

    // Create triangle bounding box
    const int xmin = std::min({a.x, b.x, c.x});
    const int ymin = std::min({a.y, b.y, c.y});

    const int xmax = std::max({a.x, b.x, c.x});
    const int ymax = std::max({a.y, b.y, c.y});

    for (auto y{ymin}; y <= ymax; ++y) {
        for (auto x{xmin}; x <= xmax; ++x) {
            if (!in_bounds(x, y, width, height)) continue;

            const point p{x, y};

            const auto bounds_a = get_determinant(b, c, p);
            const auto bounds_b = get_determinant(c, a, p);
            const auto bounds_c = get_determinant(a, b, p);

            if (bounds_a >= 0 && bounds_b >= 0 && bounds_c >= 0) {
                buff[static_cast<std::size_t>(y) * width + static_cast<std::size_t>(x)] =
                MFB_RGB(color._r, color._g, color._b);
            }
        }
    }
}
