#include "render.hpp"

#include "circuit.hpp"
#include "raymath.h"

void render_circuit(const Circuit_State& circuit, Rectangle area) {
    DrawRectangleRec(area, ColorAlphaBlend(BLACK, GRAY, LIGHTGRAY));
    
    float cell_width = area.width / ((float)circuit.original.width + 2.f);
    float cell_height = area.height / ((float)circuit.original.height + 2.f);

    for (int y = 0; y < circuit.original.height; y++) {
        for (int x = 0; x < circuit.original.width; x++) {
            Cell cell = circuit.cells[x + y * circuit.original.width];

            Rectangle cell_rec = {
                .x = area.x + cell_width * (x + 1),
                .y = area.y + cell_height * (y + 1),
                .width = cell_width,
                .height = cell_height
            };

            Rectangle inner_area = {
                .x = cell_rec.x + render_thickness,
                .y = cell_rec.y + render_thickness,
                .width = cell_rec.width - render_thickness * 2.f,
                .height = cell_rec.height - render_thickness * 2.f
            };

            Rectangle symbol_area = {
                .x = inner_area.x + 5.f,
                .y = inner_area.y + 5.f,
                .width = inner_area.width - 10.f,
                .height = inner_area.height - 10.f,
            };

            if (cell.kind != Cell::WALL) {
                DrawRectangleRounded(cell_rec, render_roundness, render_segments, DARKGRAY);
            }

            switch (cell.kind) {
                case Cell::AIR: {
                    DrawRectangleRounded(inner_area, render_roundness, render_segments, GRAY);
                    break;
                }
                case Cell::WALL: {
                    break;
                }
                case Cell::BARRIER: {
                    DrawRectangleRounded(inner_area, render_roundness, render_segments, GRAY);

                    DrawRectangleRoundedLines(symbol_area, render_roundness, render_segments, render_thickness, DARKGRAY);

                    if (cell.barrier.active) {
                        Vector2 top_left     = { symbol_area.x, symbol_area.y };
                        Vector2 top_right    = { symbol_area.x + symbol_area.width, symbol_area.y };
                        Vector2 bottom_right = { symbol_area.x + symbol_area.width, symbol_area.y + symbol_area.height };
                        Vector2 bottom_left  = { symbol_area.x, symbol_area.y + symbol_area.height };
                        DrawLineEx(top_left, bottom_right, render_thickness, DARKGRAY);
                        DrawLineEx(bottom_left, top_right, render_thickness, DARKGRAY);
                    }
                    break;
                }
                case Cell::SOCKET: {
                    DrawRectangleRounded(inner_area, render_roundness, render_segments, GRAY);

                    Vector2 center = { symbol_area.x + symbol_area.width / 2.f, symbol_area.y + symbol_area.height / 2.f };
                    float radius = symbol_area.height / 2.f;
                    DrawEllipse(center.x, center.y, radius, radius, DARKGRAY);
                    DrawEllipse(center.x, center.y, radius - render_thickness, radius - render_thickness, GRAY);
                    DrawLineEx(Vector2Subtract(center, { 0, radius / 2.f }), Vector2Add(center, { 0, radius / 2.f }), render_thickness, DARKGRAY);

                    break;
                }
                case Cell::PLATE: {
                    DrawRectangleRounded(inner_area, render_roundness, render_segments, GRAY);

                    Vector2 center = { symbol_area.x + symbol_area.width / 2.f, symbol_area.y + symbol_area.height / 2.f };
                    float radius = symbol_area.height / 2.f;
                    DrawEllipse(center.x, center.y, radius, radius, DARKGRAY);
                    DrawEllipse(center.x, center.y, radius - render_thickness, radius - render_thickness, GRAY);
                    DrawCircle(center.x, center.y, radius / 2.f, DARKGRAY);

                    break;
                }
                case Cell::REFLECTOR: {
                    DrawRectangleRounded(inner_area, render_roundness, render_segments, GRAY);

                    Vector2 top_left     = { symbol_area.x, symbol_area.y };
                    Vector2 top_right    = { symbol_area.x + symbol_area.width, symbol_area.y };
                    Vector2 bottom_right = { symbol_area.x + symbol_area.width, symbol_area.y + symbol_area.height };
                    Vector2 bottom_left  = { symbol_area.x, symbol_area.y + symbol_area.height };
                    
                    if (cell.reflector.flipped) {
                        DrawLineEx(bottom_left, top_right, render_thickness, DARKGRAY);
                    } else {
                        DrawLineEx(top_left, bottom_right, render_thickness, DARKGRAY);
                    }
                    break;   
                }
                case Cell::REDIRECTOR: {
                    TODO("Render REDIRECTOR cells.");
                    break;
                }
                case Cell::PORTAL: {
                    TODO("Render PORTAL cells.");
                    break;
                }
            }
        }
    }

    float robot_x = area.x + cell_width * (circuit.robot.position.x + 1) + cell_width / 2.f;
    float robot_y = area.y + cell_height * (circuit.robot.position.y + 1) + cell_height / 2.f;
    float power_x = area.x + cell_width * (circuit.original.power_emitter.position.x + 1) + cell_width / 2.f;
    float power_y = area.y + cell_height * (circuit.original.power_emitter.position.y + 1) + cell_height / 2.f;

    DrawCircle(robot_x, robot_y, cell_height / 2.f - 10.f, YELLOW);
    DrawCircle(power_x, power_y, cell_height / 2.f - 10.f, BLUE);

#ifdef DEBUG
    Vector2 arrow_start = { robot_x, robot_y };
    Vector2 arrow_end = Vector2Add(arrow_start, Vector2Scale(circuit.robot.direction, cell_height / 2.f - 10.f));
    DrawLineEx(arrow_start, arrow_end, 1.f, MAGENTA);
#endif

    DrawRectangleLinesEx(area, render_thickness, DARKGRAY);
}

void render_rune_table(Rune_Set available_runes, Rectangle area) {
    DrawRectangleRec(area, ColorAlphaBlend(BLACK, GRAY, LIGHTGRAY));
    DrawRectangleLinesEx(area, render_thickness, DARKGRAY);

    Rectangle inner_area = {
        .x = area.x + render_ui_margin * 2,
        .y = area.y + render_ui_margin,
        .width = area.width - render_ui_margin * 4,
        .height = area.height - render_ui_margin * 2
    };

    int num_rows = 4;
    int num_cols = 4;
    float rune_slot_width = inner_area.width / (float)num_cols;
    float rune_slot_height = inner_area.height / (float)num_rows;

    int g = 0;
    for (int i = 0; i < RUNE_COUNT; i++) {
        Rune rune = (Rune)(1 << i);
        if (!(rune & available_runes)) {
            continue;
        }

        Rectangle rune_slot_area = {
            .x = inner_area.x + rune_slot_width * (float)(g % num_cols),
            .y = inner_area.y + rune_slot_height * (float)(g / num_rows),
            .width = rune_slot_width,
            .height = rune_slot_height
        };

        float rune_area_y = rune_slot_area.y + rune_slot_area.height * 0.1f;
        float rune_area_height = rune_slot_area.height * 0.8f;
        float rune_area_width = rune_area_height / 3.f * 2.f;
        float rune_area_x = (rune_slot_area.x + rune_slot_area.width / 2.f) - rune_area_width / 2.f;

        Render_Rune_Func render_func = RENDER_RUNE_FUNCS[i];
        render_func(rune_area_x, rune_area_y, rune_area_width, rune_area_height);
        g++;
    }
}

static void render_rune_background(float x, float y, float width, float height) {
    Rectangle rec = { x, y, width, height };
    DrawRectangleRounded(rec, 0.25f, 16, GRAY);
    DrawRectangleRoundedLines(rec, 0.25f, 16, 3.f, DARKGRAY);
}

void render_scratchpad(int max_runes_allowed, Const_Span<Rune> sentence, Rectangle area) {
    DrawRectangleRec(area, ColorAlphaBlend(BLACK, GRAY, LIGHTGRAY));
    DrawRectangleLinesEx(area, render_thickness, DARKGRAY);

    int num_rows = 1;
    float slot_width;
    for (;;) {
        slot_width = (area.height / (float)num_rows) / 3.f * 2.f;
        if (slot_width * (max_runes_allowed / num_rows) <= area.width) {
            break;
        }
        num_rows++;
    }

    float slot_height = area.height / (float)num_rows;
    int num_cols = area.width / (float)slot_width;

    int i;
    for (i = 0; i < (int)sentence.length; i++) {
        int col = i % num_cols;
        int row = i / num_cols;
        float x = area.x + col * slot_width + slot_height * 0.1f;
        float y = area.y + row * slot_height + slot_height * 0.1f;
        render_rune(sentence[i], x, y, slot_width - slot_height * 0.2f, slot_height - slot_height * 0.2f);
    }

    for (; i < max_runes_allowed; i++) {
        int col = i % num_cols;
        int row = i / num_cols;
        float x = area.x + col * slot_width + slot_height * 0.1f;
        float y = area.y + row * slot_height + slot_height * 0.1f;
        render_rune_background(x, y, slot_width - slot_height * 0.2f, slot_height - slot_height * 0.2f);
    }
}

void render_rune(Rune rune, float x, float y, float width, float height) {
    for (int i = 0; i < RUNE_COUNT; i++) {
        if (rune == (Rune)(1 << i)) {
            Render_Rune_Func func = RENDER_RUNE_FUNCS[i];
            func(x, y, width, height);
        }
    }
}

static void render_rune_move(float x, float y, float width, float height) {
    float line_length = width / 2.f - (width * 0.1f);
    Vector2 center = { x + width / 2.f, y + height / 2.f };
    Vector2 top = { center.x, center.y - line_length };
    Vector2 right = { center.x + line_length, center.y };
    Vector2 bottom = { center.x, center.y + line_length };
    Vector2 left = { center.x - line_length, center.y };

    render_rune_background(x, y, width, height);
    DrawLineEx(top, right, render_thickness, DARKGRAY);
    DrawLineEx(right, bottom, render_thickness, DARKGRAY);
    DrawLineEx(bottom, left, render_thickness, DARKGRAY);
    DrawLineEx(left, top, render_thickness, DARKGRAY);
}

static void render_rune_change_direction(float x, float y, float width, float height) {
    UNUSED(x, y, width, height);
    TODO("%s", __func__);
}

static void render_rune_rotate(float x, float y, float width, float height) {
    float radius = width / 2.f - (width * 0.1f);
    Vector2 center = { x + width / 2.f, y + height / 2.f };
    Vector2 top = { center.x, center.y - height / 2.f };
    Vector2 bottom = { center.x, center.y + height / 2.f };

    render_rune_background(x, y, width, height);
    DrawCircleV(center, radius, DARKGRAY);
    DrawCircleV(center, radius - render_thickness, GRAY);
    DrawLineEx(top, bottom, render_thickness * 4.f, GRAY);
}

static void render_rune_interact(float x, float y, float width, float height) {
    render_rune_background(x, y, width, height);

    Vector2 center = { x + width / 2.f, y + height / 2.f };
    float line_length = width / 3.5f - (width * 0.1f);
    float right = center.x + line_length;
    float left = center.x - line_length;
    float top = center.y - height / 4.f;
    float bottom = center.y + height / 4.f;

    DrawCircle(left, center.y, line_length, DARKGRAY);
    DrawCircle(right, center.y, line_length, DARKGRAY);
    DrawCircle(left, center.y, line_length - render_thickness, GRAY);
    DrawCircle(right, center.y, line_length - render_thickness, GRAY);
    DrawLineEx({ center.x, top }, { center.x, bottom }, render_thickness, DARKGRAY);
}

static void render_rune_repeat(float x, float y, float width, float height) {
    UNUSED(x, y, width, height);
    TODO("%s", __func__);
}

static void render_rune_reverse(float x, float y, float width, float height) {
    UNUSED(x, y, width, height);
    TODO("%s", __func__);
}

static void render_rune_flow_end(float x, float y, float width, float height) {
    float left = x + width / render_thickness;
    float right = x + width / render_thickness * 2.f;
    float center = y + height / 2.f;
    float top = center - height * 0.1f;
    float bottom = center + height * 0.1f;

    float line_length = width / 2.f - (width * 0.1f);

    render_rune_background(x, y, width, height);
    DrawLineEx({ left, center - line_length }, { left, center + line_length }, render_thickness, DARKGRAY);
    DrawCircle(right, top, render_thickness, DARKGRAY);
    DrawCircle(right, bottom, render_thickness, DARKGRAY);
}

static void render_rune_two(float x, float y, float width, float height) {
    float line_length = width / 2.f - (width * 0.1f);
    float center = x + width / 2.f;
    float top = y + height / 3.f;
    float bottom = y + height / 3.f * 2.f;
    float right = center + line_length;
    float left = center - line_length;

    render_rune_background(x, y, width, height);
    DrawLineEx({ left, top }, { right, top }, render_thickness, DARKGRAY);
    DrawLineEx({ left, bottom }, { right, bottom }, render_thickness, DARKGRAY);
}

static void render_rune_three(float x, float y, float width, float height) {
    Vector2 center = { x + width / 2.f, y + height / 2.f };
    float top = center.y - height / 4.f;
    float bottom = center.y + height / 4.f;

    float line_length = width / 2.f - (width * 0.1f);
    float right = center.x + line_length;
    float left = center.x - line_length;

    render_rune_background(x, y, width, height);
    DrawLineEx({ left, center.y }, { right, center.y }, render_thickness, DARKGRAY);
    DrawLineEx({ left, top }, { right, top }, render_thickness, DARKGRAY);
    DrawLineEx({ left, bottom }, { right, bottom }, render_thickness, DARKGRAY);
}

static void render_rune_four(float x, float y, float width, float height) {
    Vector2 center = { x + width / 2.f, y + height / 2.f };
    float top = center.y - height / 4.f;
    float bottom = center.y + height / 4.f;

    float line_length = width / 2.f - (width * 0.1f);
    float right = center.x + line_length;
    float left = center.x - line_length;

    render_rune_background(x, y, width, height);
    DrawLineEx({ left, center.y }, { right, center.y }, render_thickness, DARKGRAY);
    DrawLineEx({ left, top }, { right, top }, render_thickness, DARKGRAY);
    DrawLineEx({ left, bottom }, { right, bottom }, render_thickness, DARKGRAY);

    DrawLineEx({ center.x + line_length / 3.f, y + (width * 0.1f) }, { center.x - line_length / 3.f, y + height - width * 0.1f }, render_thickness, DARKGRAY);
}

static_assert(RUNE_COUNT == 10);
const Render_Rune_Func RENDER_RUNE_FUNCS[RUNE_COUNT] = {
    render_rune_move,
    render_rune_change_direction,
    render_rune_rotate,
    render_rune_interact,
    render_rune_repeat,
    render_rune_reverse,
    render_rune_flow_end,
    render_rune_two,
    render_rune_three,
    render_rune_four,
};
