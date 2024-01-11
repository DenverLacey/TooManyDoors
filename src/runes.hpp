#pragma once

#include <vector>

#include "utils.hpp"

enum Rune : int {
    RUNE_MOVE = 0x1,
    RUNE_CHANGE_DIRECTION = 0x2,
    RUNE_ROTATE = 0x4,
    RUNE_INTERACT = 0x8,
    RUNE_REPEAT = 0x10,
    RUNE_REVERSE = 0x20,
    RUNE_FLOW_END = 0x40,
    RUNE_TWO = 0x80,
    RUNE_THREE = 0x100,
    RUNE_FOUR = 0x200,
};

constexpr int RUNE_COUNT = 10;

using Rune_Set = int;
