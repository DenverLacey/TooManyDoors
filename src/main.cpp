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
    GAME_STATE_AWAITING_CONFIRM
};

#define CIRCUIT_COUNT (3)
constexpr std::array<Circuit, CIRCUIT_COUNT> circuits = {
    //// World 1
    // The Robot Can Move!
    Circuit{
        .available_runes = RUNE_MOVE,
        .max_runes_allowed = 1,
        .width = 2,
        .height = 2,
        .robot = {
            .position = { 0, 1 },
            .direction = { 1, 0 }
        },
        .power_emitter = {
            .position = { 0, 0 },
            .direction = { 1, 0 }
        },
        .cells = {
            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::SOCKET },

            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::PLATE }
        }
    },
    // The Robot Can Turn!
    Circuit{
        .available_runes = RUNE_MOVE | RUNE_ROTATE,
        .max_runes_allowed = 3,
        .width = 2,
        .height = 3,
        .robot = {
            .position = { 0, 1 },
            .direction = { 1, 0 }
        },
        .power_emitter = {
            .position = { 0, 0 },
            .direction = { 1, 0 }
        },
        .cells = {
            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::SOCKET },

            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::AIR },

            Cell{ .kind = Cell::WALL },
            Cell{ .kind = Cell::PLATE }
        }
    },
    // The Robot Can Move Far!
    Circuit{
        .available_runes = RUNE_MOVE | RUNE_TWO,
        .max_runes_allowed = 2,
        .width = 3,
        .height = 3,
        .robot = {
            .position = { 0, 1 },
            .direction = { 1, 0 }
        },
        .power_emitter = {
            .position = { 1, 0 },
            .direction = { 0, 1 }
        },
        .cells = {
            Cell{ .kind = Cell::WALL },
            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::WALL },

            Cell{ .kind = Cell::AIR },
            Cell{ .kind = Cell::TRAP },
            Cell{ .kind = Cell::PLATE },

            Cell{ .kind = Cell::WALL },
            Cell{ .kind = Cell::SOCKET },
            Cell{ .kind = Cell::WALL },
        }
    }

    // Reflextor heavy thing with barrier
    // Circuit{
    //     .available_runes = RUNE_MOVE | RUNE_INTERACT | RUNE_FOUR,
    //     .max_runes_allowed = 8,
    //     .width = 5,
    //     .height = 5,
    //     .robot = {
    //         .position = { 0, 0 },
    //         .direction = { 1, 0 }
    //     },
    //     .power_emitter = {
    //         .position = { 2, -1 },
    //         .direction = { 0, 1 }
    //     },
    //     .cells = {
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::REFLECTOR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::WALL },

    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::REFLECTOR, .reflector = { .flipped = true } },
    //         Cell{ .kind = Cell::REFLECTOR },

    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },

    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::REFLECTOR },
    //         Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::REFLECTOR, .reflector = { .flipped = true } },

    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::SOCKET },
    //         Cell{ .kind = Cell::PLATE },
    //         Cell{ .kind = Cell::WALL },
    //     }
    // },

    // Reflector thing for power
    // Circuit{
    //     .available_runes = RUNE_MOVE | RUNE_ROTATE | RUNE_TWO | RUNE_THREE | RUNE_FOUR | RUNE_INTERACT,
    //     .max_runes_allowed = 12,
    //     .width = 10,
    //     .height = 5,
    //     .robot = {
    //         .position = { -1, 0 },
    //         .direction = { 1, 0 }
    //     },
    //     .power_emitter = { 
    //         .position = { 6, -1 },
    //         .direction = { 0, 1 }
    //     },
    //     .cells = {
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },

    //         Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
    //         Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
    //         Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
    //         Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
    //         Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },

    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },

    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::REFLECTOR, .reflector = { .flipped = true } },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::SOCKET },

    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::WALL },
    //         Cell{ .kind = Cell::BARRIER, .barrier = { .active = true } },
    //         Cell{ .kind = Cell::PLATE },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //         Cell{ .kind = Cell::AIR },
    //     }
    // }
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
    Update_Level_Result game_level_result = UPDATE_LEVEL_OK;
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
                    game_level_result = update_level(mb, circuit_area, rune_table_area, scratchpad_area);
                    switch (game_level_result) {
                        case UPDATE_LEVEL_ERROR: {
                            goto SHUTDOWN;
                        }
                        case UPDATE_LEVEL_PASSED:
                        case UPDATE_LEVEL_FAILED: {
                            game_state = GAME_STATE_AWAITING_CONFIRM;
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }
                case GAME_STATE_AWAITING_CONFIRM: {
                    // @Cleanup: We just want to render the level but right now the 2 are coupled.
                    // Need to find a way to split the two without something as BS as Box_Graph was.
                    if (update_level(mb, circuit_area, rune_table_area, scratchpad_area) == UPDATE_LEVEL_ERROR) {
                        goto SHUTDOWN;
                    }

                    if (game_level_result == UPDATE_LEVEL_PASSED) {
                        DrawText("CIRCUIT COMPLETE: DOOR OPENED", WINDOW_WIDTH/2.f, render_ui_margin * 2, 30, WHITE);
                    } else {
                        DrawText("ROBOT DESTROYED!", WINDOW_WIDTH/2.f, render_ui_margin * 2, 30, WHITE);
                    }

                    if (GetKeyPressed() != 0) {
                        if (game_level_result == UPDATE_LEVEL_PASSED) {
                            current_level++;
                            if (current_level >= (int)circuits.size()) {
                                TraceLog(LOG_INFO, "YOU WIN!");
                                goto SHUTDOWN;
                            } else {
                                mb.load(circuits[current_level]);
                                game_state = GAME_STATE_LEVEL;
                            }
                        } else {
                            mb.reload();
                            game_state = GAME_STATE_LEVEL;
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
