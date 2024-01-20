#include "render.hpp"

#include "circuit.hpp"
#include "raymath.h"

void render_circuit(const Motherboard& mb, Rectangle area) {
    render_bordered_pane(area);
    
    float cell_width = area.width / ((float)mb.circuit.width + 2.f);
    float cell_height = area.height / ((float)mb.circuit.height + 2.f);

    for (int y = 0; y < mb.circuit.height; y++) {
        for (int x = 0; x < mb.circuit.width; x++) {
            Cell cell = mb.cells[x + y * mb.circuit.width];

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
                case Cell::TRAP: {
                    DrawRectangleRounded(inner_area, render_roundness, render_segments, GRAY);
                    Vector2 center = { symbol_area.x + symbol_area.width / 2.f, symbol_area.y + symbol_area.height / 2.f };

                    float left = symbol_area.x + symbol_area.width / 3.f;
                    float right = symbol_area.x + symbol_area.width / 3.f * 2.f;
                    float top = symbol_area.y + render_thickness;
                    float bottom = symbol_area.y + symbol_area.height - render_thickness;

                    DrawLineEx({ left, bottom }, { left, center.y }, render_thickness, render_border_color);
                    DrawLineEx({ center.x, bottom }, { center.x, top }, render_thickness, render_border_color);
                    DrawLineEx({ right, bottom }, { right, center.y }, render_thickness, render_border_color);

                    break;   
                }
            }
        }
    }

    for (int i = 1; i < (int)mb.powered_cells.size(); i++) {
        Vector2i p1 = mb.powered_cells[i-1];
        Vector2i p2 = mb.powered_cells[i];

        Vector2 v1 = {
            .x = area.x + cell_width * (p1.x + 1) + cell_width / 2.f,
            .y = area.y + cell_height * (p1.y + 1) + cell_height / 2.f,
        };

        Vector2 v2 = {
            .x = area.x + cell_width * (p2.x + 1) + cell_width / 2.f,
            .y = area.y + cell_height * (p2.y + 1) + cell_height / 2.f,
        };

        DrawLineEx(v1, v2, render_thickness, BLUE);
    }

    float robot_x = area.x + cell_width * (mb.robot.position.x + 1) + cell_width / 2.f;
    float robot_y = area.y + cell_height * (mb.robot.position.y + 1) + cell_height / 2.f;
    float power_x = area.x + cell_width * (mb.circuit.power_emitter.position.x + 1) + cell_width / 2.f;
    float power_y = area.y + cell_height * (mb.circuit.power_emitter.position.y + 1) + cell_height / 2.f;

    DrawCircle(robot_x, robot_y, cell_height / 2.f - 10.f, YELLOW);
    DrawCircle(power_x, power_y, cell_height / 2.f - 10.f, BLUE);

#ifdef DEBUG
    Vector2 arrow_start = { robot_x, robot_y };
    Vector2 arrow_end = Vector2Add(arrow_start, Vector2Scale(mb.robot.direction, cell_height / 2.f - 10.f));
    DrawLineEx(arrow_start, arrow_end, 1.f, MAGENTA);
#endif
}

void render_bordered_pane(Rectangle area) {
    DrawRectangleRec(area, render_bg_color);
    DrawRectangleLinesEx(area, render_thickness, render_border_color);
}

void render_button(float x, float y, float width, float height, bool highlighted) {
    render_button({ x, y, width, height }, highlighted);
}

void render_button(Rectangle button_area, bool highlighted) {
    DrawRectangleRounded(button_area, render_roundness, render_segments, render_fg_color);
    DrawRectangleRoundedLines(button_area, render_roundness, render_segments, render_thickness, highlighted ? render_highlight_color : render_border_color);
}

void render_play_button(Rectangle area, bool playing, bool highlighted) {
    render_button(area, highlighted);

    Vector2 v1 = {
        .x = area.x + area.width / 3.f,
        .y = area.y + area.height / 3.f
    };

    Vector2 v2 = {
        .x = area.x + area.width / 3.f,
        .y = area.y + area.height / 3.f * 2.f
    };

    Vector2 v3 = {
        .x = area.x + area.width / 3.f * 2.f,
        .y = area.y + area.height / 2.f
    };

    DrawTriangle(v1, v2, v3, playing ? render_highlight_color : render_border_color);
}

void render_pause_button(Rectangle area, bool highlighted) {
    render_button(area, highlighted);

    float top = area.y + area.height / 3.f;
    float left = area.x + area.width / 3.f;
    float bottom = area.y + area.height / 3.f * 2.f;
    float right = area.x + area.width / 3.f * 2.f;

    DrawLineEx({ left, top }, { left, bottom }, render_thickness, render_border_color);
    DrawLineEx({ right, top }, { right, bottom }, render_thickness, render_border_color);
}

void render_step_button(Rectangle area, bool highlighted) {
    render_button(area, highlighted);

    Vector2 v1 = {
        .x = area.x + area.width / 3.f,
        .y = area.y + area.height / 3.f
    };

    Vector2 v2 = {
        .x = area.x + area.width / 3.f,
        .y = area.y + area.height / 3.f * 2.f
    };

    Vector2 v3 = {
        .x = area.x + area.width / 3.f * 2.f,
        .y = area.y + area.height / 2.f
    };

    DrawTriangleLines(v1, v2, v3, render_border_color);
}

void render_clear_button(Rectangle area, bool highlighted) {
    render_button(area, highlighted);

    float top = area.y + area.height / 3.f;
    float left = area.x + area.width / 3.f;
    float bottom = area.y + area.height / 3.f * 2.f;
    float right = area.x + area.width / 3.f * 2.f;

    DrawLineEx({ left, top }, { right, bottom }, render_thickness, render_border_color);
    DrawLineEx({ right, top }, { left, bottom }, render_thickness, render_border_color);
}

void render_rune(Rune rune, float x, float y, float width, float height, bool highlighted) {
    for (int i = 0; i < RUNE_COUNT; i++) {
        if (rune == (Rune)(1 << i)) {
            Render_Rune_Func func = RENDER_RUNE_FUNCS[i];
            func(x, y, width, height, highlighted);
        }
    }
}

void render_rune(Rune rune, Rectangle rune_area, bool highlighted) {
    render_rune(rune, rune_area.x, rune_area.y, rune_area.width, rune_area.height, highlighted);
}

static void render_rune_move(float x, float y, float width, float height, bool highlighted) {
    float line_length = width / 2.f - (width * 0.1f);
    Vector2 center = { x + width / 2.f, y + height / 2.f };
    Vector2 top = { center.x, center.y - line_length };
    Vector2 right = { center.x + line_length, center.y };
    Vector2 bottom = { center.x, center.y + line_length };
    Vector2 left = { center.x - line_length, center.y };

    render_button(x, y, width, height, highlighted);
    DrawLineEx(top, right, render_thickness, render_border_color);
    DrawLineEx(right, bottom, render_thickness, render_border_color);
    DrawLineEx(bottom, left, render_thickness, render_border_color);
    DrawLineEx(left, top, render_thickness, render_border_color);
}

static void render_rune_change_direction(float x, float y, float width, float height, bool highlighted) {
    UNUSED(x, y, width, height, highlighted);
    TODO("%s", __func__);
}

static void render_rune_rotate(float x, float y, float width, float height, bool highlighted) {
    float radius = width / 2.f - (width * 0.1f);
    Vector2 center = { x + width / 2.f, y + height / 2.f };
    Vector2 top = { center.x, center.y - height / 2.f };
    Vector2 bottom = { center.x, center.y + height / 2.f };

    render_button(x, y, width, height, highlighted);
    DrawCircleV(center, radius, render_border_color);
    DrawCircleV(center, radius - render_thickness, render_fg_color);
    DrawLineEx(top, bottom, render_thickness * 4.f, render_fg_color);
}

static void render_rune_interact(float x, float y, float width, float height, bool highlighted) {
    render_button(x, y, width, height, highlighted);

    Vector2 center = { x + width / 2.f, y + height / 2.f };
    float line_length = width / 3.5f - (width * 0.1f);
    float right = center.x + line_length;
    float left = center.x - line_length;
    float top = center.y - height / 4.f;
    float bottom = center.y + height / 4.f;

    DrawCircle(left, center.y, line_length, render_border_color);
    DrawCircle(right, center.y, line_length, render_border_color);
    DrawCircle(left, center.y, line_length - render_thickness, render_fg_color);
    DrawCircle(right, center.y, line_length - render_thickness, render_fg_color);
    DrawLineEx({ center.x, top }, { center.x, bottom }, render_thickness, render_border_color);
}

static void render_rune_repeat(float x, float y, float width, float height, bool highlighted) {
    UNUSED(x, y, width, height, highlighted);
    TODO("%s", __func__);
}

static void render_rune_reverse(float x, float y, float width, float height, bool highlighted) {
    UNUSED(x, y, width, height, highlighted);
    TODO("%s", __func__);
}

static void render_rune_flow_end(float x, float y, float width, float height, bool highlighted) {
    float left = x + width / render_thickness;
    float right = x + width / render_thickness * 2.f;
    float center = y + height / 2.f;
    float top = center - height * 0.1f;
    float bottom = center + height * 0.1f;

    float line_length = width / 2.f - (width * 0.1f);

    render_button(x, y, width, height, highlighted);
    DrawLineEx({ left, center - line_length }, { left, center + line_length }, render_thickness, render_border_color);
    DrawCircle(right, top, render_thickness, render_border_color);
    DrawCircle(right, bottom, render_thickness, render_border_color);
}

static void render_rune_two(float x, float y, float width, float height, bool highlighted) {
    float line_length = width / 2.f - (width * 0.1f);
    float center = x + width / 2.f;
    float top = y + height / 3.f;
    float bottom = y + height / 3.f * 2.f;
    float right = center + line_length;
    float left = center - line_length;

    render_button(x, y, width, height, highlighted);
    DrawLineEx({ left, top }, { right, top }, render_thickness, render_border_color);
    DrawLineEx({ left, bottom }, { right, bottom }, render_thickness, render_border_color);
}

static void render_rune_three(float x, float y, float width, float height, bool highlighted) {
    Vector2 center = { x + width / 2.f, y + height / 2.f };
    float top = center.y - height / 4.f;
    float bottom = center.y + height / 4.f;

    float line_length = width / 2.f - (width * 0.1f);
    float right = center.x + line_length;
    float left = center.x - line_length;

    render_button(x, y, width, height, highlighted);
    DrawLineEx({ left, center.y }, { right, center.y }, render_thickness, render_border_color);
    DrawLineEx({ left, top }, { right, top }, render_thickness, render_border_color);
    DrawLineEx({ left, bottom }, { right, bottom }, render_thickness, render_border_color);
}

static void render_rune_four(float x, float y, float width, float height, bool highlighted) {
    Vector2 center = { x + width / 2.f, y + height / 2.f };
    float top = center.y - height / 4.f;
    float bottom = center.y + height / 4.f;

    float line_length = width / 2.f - (width * 0.1f);
    float right = center.x + line_length;
    float left = center.x - line_length;

    render_button(x, y, width, height, highlighted);
    DrawLineEx({ left, center.y }, { right, center.y }, render_thickness, render_border_color);
    DrawLineEx({ left, top }, { right, top }, render_thickness, render_border_color);
    DrawLineEx({ left, bottom }, { right, bottom }, render_thickness, render_border_color);

    DrawLineEx({ center.x + line_length / 3.f, y + (width * 0.1f) }, { center.x - line_length / 3.f, y + height - width * 0.1f }, render_thickness, render_border_color);
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
