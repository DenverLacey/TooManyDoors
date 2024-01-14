#pragma once

#include "utils.hpp"
#include "raylib.h"

struct Circuit_State;

void update_main_menu(Circuit_State& circuit);
void update_level(Circuit_State& circuit, Rectangle circuit_area, Rectangle runes_area, Rectangle scratchpad_area);

bool button_hovered(Rectangle button_area);
bool button_pressed(Rectangle button_area);
