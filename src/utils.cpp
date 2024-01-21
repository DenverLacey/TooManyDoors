#include "utils.hpp"

Vector2i Vector2i::rotate_clockwise() const {
    int nx = x;
    return {
        .x = -y,
        .y = nx
    };
}

Vector2i Vector2i::rotate_counter_clockwise() const {
    int nx = x;
    return {
        .x = y,
        .y = -nx
    };
}

Vector2i::operator Vector2() const noexcept {
    return { (float)x, (float)y };
}
