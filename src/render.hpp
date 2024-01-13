#pragma once

#include "runes.hpp"

constexpr float render_roundness = 0.25f;
constexpr int render_segments = 16;
constexpr float render_thickness = 3.f;
constexpr int render_ui_margin = 10;

struct Circuit_State;

void render_circuit(const Circuit_State& circuit, Rectangle area);
// void render_rune_table(Rune_Set available_runes, Rectangle area);
// void render_scratchpad(int max_runes_allowed, Const_Span<Rune> sentence, Rectangle area);
void render_rune_background(float x, float y, float width, float height, bool highlighted = false);
void render_rune(Rune rune, float x, float y, float width, float height, bool highlighted = false);

typedef void (*Render_Rune_Func)(float x, float y, float width, float height, bool highlighted);
extern const Render_Rune_Func RENDER_RUNE_FUNCS[RUNE_COUNT];
