#include <stdio.h>
#include "raylib.h"

#include "circuit.hpp"
#include "render.hpp"

#define WINDOW_WIDTH (16*80)
#define WINDOW_HEIGHT (9*80)

Circuit_Info circuit_1 = {
    .available_runes = RUNE_MOVE | RUNE_ROTATE | RUNE_TWO | RUNE_THREE | RUNE_FOUR | RUNE_INTERACT,
    .max_runes_allowed = 12,
    .width = 10,
    .height = 5,
    .robot = {
        .position = { -1, 0 },
        .direction = { 1, 0 }
    },
    .power_emitter = { 
        .position = { 6, -1 },
        .direction = { 0, 1 }
    },
    .cells = {
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },

        Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
        Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
        Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
        Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
        Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },

        Cell{ .kind = Cell::WALL },
        Cell{ .kind = Cell::WALL },
        Cell{ .kind = Cell::WALL },
        Cell{ .kind = Cell::WALL },
        Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },

        Cell{ .kind = Cell::WALL },
        Cell{ .kind = Cell::WALL },
        Cell{ .kind = Cell::WALL },
        Cell{ .kind = Cell::WALL },
        Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::REFLECTOR, .reflector = { .flipped = true } },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::SOCKET },

        Cell{ .kind = Cell::WALL },
        Cell{ .kind = Cell::WALL },
        Cell{ .kind = Cell::WALL },
        Cell{ .kind = Cell::WALL },
        Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
        Cell{ .kind = Cell::PLATE },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
        Cell{ .kind = Cell::AIR },
    }
};


int main(int argc, const char **argv) {
    UNUSED(argc, argv);

#ifdef DEBUG
    SetTraceLogLevel(LOG_ALL);
#else
    SetTraceLogLevel(LOG_ERROR);
#endif

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Doors");

    Circuit_State circuit{};
    circuit.set(circuit_1);

    circuit.sentence.push_back(RUNE_FOUR);
    circuit.sentence.push_back(RUNE_MOVE);
    circuit.sentence.push_back(RUNE_TWO);
    circuit.sentence.push_back(RUNE_MOVE);
    circuit.sentence.push_back(RUNE_ROTATE);
    circuit.sentence.push_back(RUNE_THREE);
    circuit.sentence.push_back(RUNE_MOVE);
    circuit.sentence.push_back(RUNE_THREE);
    circuit.sentence.push_back(RUNE_ROTATE);
    circuit.sentence.push_back(RUNE_INTERACT);
    circuit.sentence.push_back(RUNE_ROTATE);
    circuit.sentence.push_back(RUNE_MOVE);

    Rectangle circuit_area = {
        .x = WINDOW_WIDTH / 3,
        .y = WINDOW_HEIGHT / 5 * 2,
        .width = WINDOW_WIDTH - (WINDOW_WIDTH / 3) - render_ui_margin,
        .height = WINDOW_HEIGHT - (WINDOW_HEIGHT / 5 * 2) - render_ui_margin
    };

    Rectangle rune_table_area = {
        .x = render_ui_margin,
        .y = WINDOW_HEIGHT / 5 * 2,
        .width = circuit_area.x - render_ui_margin - render_ui_margin,
        .height = WINDOW_HEIGHT - (WINDOW_HEIGHT / 5 * 2) - render_ui_margin
    };

    Rectangle scratchpad_area = {
        .x = render_ui_margin,
        .y = render_ui_margin,
        .width = WINDOW_WIDTH - render_ui_margin * 2.f,
        .height = (rune_table_area.y - render_ui_margin) - render_ui_margin
    };

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) {
            circuit.tick();
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            render_circuit(circuit, circuit_area);
            render_rune_table(circuit.original.available_runes, rune_table_area);
            render_scratchpad(circuit.original.max_runes_allowed, circuit.sentence, scratchpad_area);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
