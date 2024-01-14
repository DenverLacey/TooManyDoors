#include <math.h>
#include <stdio.h>
#include "raylib.h"

#include "circuit.hpp"
#include "update.hpp"
#include "render.hpp"

#define WINDOW_WIDTH (16*80)
#define WINDOW_HEIGHT (9*80)

enum Game_State {
    GAME_STATE_MENU,
    GAME_STATE_LEVEL,
};

#define CIRCUIT_COUNT (2)
constexpr std::array<Circuit, CIRCUIT_COUNT> circuits = {
    Circuit{
        .available_runes = RUNE_MOVE | RUNE_INTERACT | RUNE_FOUR,
        .max_runes_allowed = 8,
        .width = 5,
        .height = 5,
        .robot = {
            .position = { 0, 0 },
            .direction = { 1, 0 }
        },
        .power_emitter = {
            .position = { 2, -1 },
            .direction = { 0, 1 }
        },
        .cells = {
            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::REFLECTOR },
            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::WALL },
            Cell{ .kind = Cell::WALL },

            Cell{ .kind = Cell::WALL },
            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::REFLECTOR, .reflector = { .flipped = true } },
            Cell{ .kind = Cell::REFLECTOR },

            Cell{ .kind = Cell::WALL },
            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::AIR },

            Cell{ .kind = Cell::WALL },
            Cell{ .kind = Cell::REFLECTOR },
            Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::REFLECTOR, .reflector = { .flipped = true } },

            Cell{ .kind = Cell::WALL },
            Cell{ .kind = Cell::WALL },
            Cell{ .kind = Cell::SOCKET },
            Cell{ .kind = Cell::PLATE },
            Cell{ .kind = Cell::WALL },
        }
    },
    Circuit{
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

    Game_State game_state = GAME_STATE_LEVEL;
    int current_level = 0;

    Motherboard mb{};

    // circuit.load(test_circuit);
    mb.load(circuits[current_level]);

    // // Solution circuit_1
    // circuit.sentence.push_back(RUNE_FOUR);
    // circuit.sentence.push_back(RUNE_MOVE);
    // circuit.sentence.push_back(RUNE_TWO);
    // circuit.sentence.push_back(RUNE_MOVE);
    // circuit.sentence.push_back(RUNE_ROTATE);
    // circuit.sentence.push_back(RUNE_THREE);
    // circuit.sentence.push_back(RUNE_MOVE);
    // circuit.sentence.push_back(RUNE_THREE);
    // circuit.sentence.push_back(RUNE_ROTATE);
    // circuit.sentence.push_back(RUNE_INTERACT);
    // circuit.sentence.push_back(RUNE_ROTATE);
    // circuit.sentence.push_back(RUNE_MOVE);

    // // Solution test_circuit
    // circuit.sentence.push_back(RUNE_FOUR);
    // circuit.sentence.push_back(RUNE_MOVE);
    // circuit.sentence.push_back(RUNE_INTERACT);
    // circuit.sentence.push_back(RUNE_FOUR);
    // circuit.sentence.push_back(RUNE_MOVE);
    // circuit.sentence.push_back(RUNE_FOUR);
    // circuit.sentence.push_back(RUNE_MOVE);
    // circuit.sentence.push_back(RUNE_MOVE);

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
        BeginDrawing();
            ClearBackground(RAYWHITE);
            switch (game_state) {
                case GAME_STATE_MENU: {
                    update_main_menu(mb);
                    break;
                }
                case GAME_STATE_LEVEL: {
                    Update_Level_Result result = update_level(mb, circuit_area, rune_table_area, scratchpad_area);
                    if (result == UPDATE_LEVEL_ERROR) {
                        goto SHUTDOWN;
                    } else if (result == UPDATE_LEVEL_PASSED) {
                        current_level++;
                        if (current_level >= (int)circuits.size()) {
                            TraceLog(LOG_INFO, "You've Won!!!");
                            goto SHUTDOWN;
                        } else {
                            mb.load(circuits[current_level]);
                        }
                    }
                    break;
                }
            }
        EndDrawing();
    }

SHUTDOWN:
    CloseWindow();
    return 0;
}
