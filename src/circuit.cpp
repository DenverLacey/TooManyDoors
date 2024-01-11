#include "circuit.hpp"

#include "raymath.h"

void Circuit_State::set(Circuit_Info circuit) {
    original = circuit;
    cells = original.cells;
    reset();
}

void Circuit_State::reset() {
    robot = original.robot;
    sentence = {};
}

Tick_Result Circuit_State::tick() {
    if (ip >= (int)sentence.size()) {
        return TICK_COMPLETE;
    }

    int new_multiplier = 0;
    int mult = multiplier ? multiplier : 1;
    for (int i = 0; i < mult; i++) {
        switch (sentence[ip]) {
            case RUNE_MOVE: {
                float nx = robot.position.x + robot.direction.x;
                float ny = robot.position.y + robot.direction.y;
                if (nx < 0 || nx >= original.width ||
                    ny < 0 || ny >= original.height)
                {
                    break;
                }

                Cell ncell = cells[nx + ny * original.width];
                switch (ncell.kind) {
                    case Cell::WALL: break;
                    case Cell::BARRIER: if (ncell.barrier.active) break;
                    default: {
                        robot.position.x = nx;
                        robot.position.y = ny;
                        break;
                    }
                }
                break;
            }
            case RUNE_CHANGE_DIRECTION: {
                if (multiplier == 0) {
                    return TICK_ERROR_INVALID_SENTENCE;
                }
                break;
            }
            case RUNE_ROTATE: {
                float x = robot.direction.x;
                robot.direction.x = -robot.direction.y;
                robot.direction.y = x;
                break;
            }
            case RUNE_INTERACT: {
                float nx = robot.position.x + robot.direction.x;
                float ny = robot.position.y + robot.direction.y;
                if (nx < 0 || nx >= original.width ||
                    ny < 0 || ny >= original.height)
                {
                    break;
                }

                Cell& ncell = cells[nx + ny * original.width];
                switch (ncell.kind) {
                    case Cell::BARRIER: {
                        ncell.barrier.active = !ncell.barrier.active;
                        break;
                    }
                    case Cell::REFLECTOR: {
                        ncell.reflector.flipped = !ncell.reflector.flipped;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case RUNE_REPEAT: {
                break;
            }
            case RUNE_REVERSE: {
                break;
            }
            case RUNE_FLOW_END: {
                break;
            }
            case RUNE_TWO: {
                new_multiplier = 2;
                break;
            }
            case RUNE_THREE: {
                new_multiplier = 3;
                break;
            }
            case RUNE_FOUR: {
                new_multiplier = 4;
                break;
            }
        }
    }

    multiplier = new_multiplier;
    if (mode == EXE_REVERSE) {
        ip--;
    } else {
        ip++;
    }

    return TICK_OK;
}
