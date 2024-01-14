#include "circuit.hpp"

#include "raymath.h"

void Robot::rotate_clockwise() {
    float x = direction.x;
    direction.x = -direction.y;
    direction.y = x;
}

void Robot::rotate_counter_clockwise() {
    float x = direction.x;
    direction.x = direction.y;
    direction.y = -x;
}

void Circuit_State::load(Circuit_Info circuit) {
    original = circuit;
    cells = original.cells;
    reload();
}

void Circuit_State::reload() {
    robot = original.robot;
    sentence = {};
    assert(cells.size() == original.cells.size());
    memcpy(cells.data(), original.cells.data(), (original.width * original.height) * sizeof(Cell));
}

bool Circuit_State::robot_on_plate() {
    return cells[robot.position.x + robot.position.y * original.width].kind == Cell::PLATE;
}

Step_Result Circuit_State::step() {
    if (ip >= (int)sentence.size()) {
        if (robot_on_plate()) {
            return STEP_COMPLETE_PLATE_ACTIVATED;
        } else {
            return STEP_COMPLETE;
        }
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

                switch (ncell.kind) {
                    case Cell::REFLECTOR: {
                        if (ncell.reflector.flipped) {
                            if (robot.direction.x != 0) {
                                robot.rotate_counter_clockwise();
                            } else {
                                robot.rotate_clockwise();
                            }
                        } else {
                            if (robot.direction.x != 0) {
                                robot.rotate_clockwise();
                            } else {
                                robot.rotate_counter_clockwise();
                            }
                        }
                        break;
                    }
                    case Cell::PLATE: {
                        return STEP_COMPLETE_PLATE_ACTIVATED;
                    }
                    default:
                        break;
                }
                break;
            }
            case RUNE_CHANGE_DIRECTION: {
                if (multiplier == 0) {
                    return STEP_ERROR_INVALID_SENTENCE;
                }
                break;
            }
            case RUNE_ROTATE: {
                robot.rotate_clockwise();
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

    return STEP_OK;
}
