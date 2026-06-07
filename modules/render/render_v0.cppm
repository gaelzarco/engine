/**
 * @file render_v0.cppm
 * @brief C++20 module providing primitive 2D rendering utilities for a MiniFB pixel buffer.
 * @version 0.0
 *
 * @note The following clang-tidy checks are suppressed intentionally:
 *       - @c bugprone-easily-swappable-parameters: coordinate parameters are
 *         swapped deliberately as part of the Bresenham steep-line transform.
 *       - @c bugprone-narrowing-conversions: RNG output is truncated to @c uint8_t by design.
 *       - @c bugprone-float-loop-counter: integer loop counter is used; suppression
 *         is present for future-proofing subpixel variants.
 */

module;

#include <vector>
#include <cstdint>
#include <random>
#include <cmath>
#include "../minifb/include/MiniFB_cpp.h"

export module render_v0;

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
// NOLINTBEGIN(bugprone-narrowing-conversions)
// NOLINTBEGIN(bugprone-float-loop-counter)

/**
 * @struct buff_color
 * @brief An RGB colour value for use with a MiniFB pixel buffer.
 *
 * Each channel is an unsigned 8-bit integer in the range [0, 255].
 * If all three channels are 0 (the default), the constructor generates
 * a random colour using a non-deterministic seed, making it convenient
 * for debug visualisation where a unique-looking colour is needed without
 * having to specify one explicitly.
 *
 * @par Example — explicit colour
 * @code
 * buff_color red(255, 0, 0);
 * @endcode
 *
 * @par Example — random colour
 * @code
 * buff_color random_color; // r, g, b chosen randomly at construction
 * @endcode
 */
export struct buff_color {
    using color_t = uint8_t;
    buff_color(color_t r = 0, color_t g = 0, color_t b = 0) : _r(r), _g(g), _b(b) {
        if (r == 0 && g == 0 & b == 0) {
            std::random_device rd;
            std::mt19937 rng(rd());
            _r = rng();
            _g = rng();
            _b = rng();
        }
    }

    color_t _r;
    color_t _g;
    color_t _b;
};

/**
 * @struct point
 * @brief A 2D integer screen-space coordinate.
 * @var point::x  Horizontal position in pixels. Default: 0.
 * @var point::y  Vertical position in pixels.   Default: 0.
 */
export struct point { int x, y{}; };

/**
 * @brief Draws an anti-aliasing-free straight line into a MiniFB pixel buffer.
 *
 * Implements Bresenham's line algorithm with a steep-line correction:
 * when the line is more vertical than horizontal, the axes are swapped so
 * the inner loop always iterates over the longer axis, guaranteeing every
 * pixel along the line is filled without gaps.
 *
 * The buffer is a flat row-major array of packed @c MFB_RGB pixels with
 * dimensions @p WIDTH × @p HEIGHT. Pixel @c (x, y) maps to index
 * @c y * WIDTH + x.
 *
 * @param a       Start point of the line (screen-space pixels).
 * @param b       End point of the line (screen-space pixels).
 * @param buff    Flat pixel buffer of size @p WIDTH * @p HEIGHT, modified in place.
 * @param color   RGB colour to paint the line with.
 * @param WIDTH   Horizontal extent of the pixel buffer in pixels.
 * @param HEIGHT  Vertical extent of the pixel buffer in pixels.
 *
 * @warning No bounds checking is performed. Both endpoints must lie within
 *          [0, WIDTH) × [0, HEIGHT) or the write will access memory outside
 *          @p buff.
 *
 * @note @p a and @p b are taken by value; the algorithm may swap their
 *       coordinates internally without affecting the caller's variables.
 *
 * @par Complexity
 * O(max(|Δx|, |Δy|)) — one write per pixel along the dominant axis.
 *
 * @par Example
 * @code
 * std::vector<std::size_t> buffer(WIDTH * HEIGHT, 0);
 * buff_color red(255, 0, 0);
 * line({0, 0}, {WIDTH - 1, HEIGHT - 1}, buffer, red, WIDTH, HEIGHT);
 * @endcode
 */
export void line(point a, point b, std::vector<std::size_t>& buff, const buff_color& color,
const std::size_t WIDTH, const std::size_t HEIGHT) {
    bool steep = std::abs(a.x - b.x) < std::abs(a.y - b.y);

    if (steep) {
        std::swap(a.x, a.y);
        std::swap(b.x, b.y);
    }

    if (a.x > b.x) std::swap(a, b);

    int y = a.y;
    int err = 0;

    for (int x{a.x}; x <= b.x; x++) {
        if (steep) {
            buff[x * WIDTH + y] = MFB_RGB(color._r, color._g, color._b);
        } else {
            buff[y * WIDTH + x] = MFB_RGB(color._r, color._g, color._b);
        }
        err += 2 * std::abs(b.y - a.y);
        if (err > b.x - a.x) {
            y += b.y > a.y ? 1 : -1;
            err -= 2 * (b.x - a.x);
        }
    }
}
// NOLINTEND(bugprone-float-loop-counter)
// NOLINTEND(bugprone-narrowing-conversions)
// NOLINTEND(bugprone-easily-swappable-parameters)

