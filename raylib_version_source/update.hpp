#pragma once

#include "utils.hpp"
#include "raylib.h"

struct Motherboard;

enum Update_Level_Result {
    UPDATE_LEVEL_ERROR,
    UPDATE_LEVEL_OK,
    UPDATE_LEVEL_FAILED,
    UPDATE_LEVEL_PASSED
};

void update_main_menu(Motherboard& mb);
Update_Level_Result update_level(Motherboard& mb, Rectangle circuit_area, Rectangle runes_area, Rectangle scratchpad_area);

bool button_hovered(Rectangle button_area);
bool button_pressed(Rectangle button_area);
