#pragma once

#include "runes.hpp"

constexpr float render_roundness = 0.25f;
constexpr int render_segments = 16;
constexpr float render_thickness = 3.f;
constexpr int render_ui_margin = 10;

inline const Color render_border_color = DARKGRAY;
inline const Color render_highlight_color = WHITE;
inline const Color render_fg_color = GRAY;
inline const Color render_bg_color = ColorAlphaBlend(BLACK, GRAY, LIGHTGRAY);

struct Motherboard;

void render_circuit(const Motherboard& mb, Rectangle area);

void render_bordered_pane(Rectangle area);

void render_button(float x, float y, float width, float height, bool highlighted = false);
void render_button(Rectangle button_area, bool highlighted = false);
void render_play_button(Rectangle area, bool playing, bool highlighted);
void render_pause_button(Rectangle area, bool highlighted);
void render_step_button(Rectangle area, bool highlighted);

void render_rune(Rune rune, float x, float y, float width, float height, bool highlighted = false);
void render_rune(Rune rune, Rectangle rune_area, bool highlighted = false);

typedef void (*Render_Rune_Func)(float x, float y, float width, float height, bool highlighted);
extern const Render_Rune_Func RENDER_RUNE_FUNCS[RUNE_COUNT];
