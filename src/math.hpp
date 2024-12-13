#ifndef MATH_H
#define MATH_H

#include <Vector2.hpp>

namespace bm::math {

struct Transform {
    raylib::Vector2 position;
    raylib::Vector2 direction { 0, -1 };
};

} // namespace bm::math

#endif // MATH_H
