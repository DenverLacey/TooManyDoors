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

void Motherboard::load(const Circuit& circuit) {
    this->circuit = circuit;
    cells = circuit.cells;
    reload();
}

void Motherboard::reload() {
    playing = false;
    time_of_last_step = -INFINITY;
    robot = circuit.robot;
    power_direction = circuit.power_emitter.direction;
    mode = EXE_NORMAL;
    ip = 0;
    multiplier = 0;
    sentence.clear();
    powered_cells.clear();
    assert(cells.size() == circuit.cells.size());
    memcpy(cells.data(), circuit.cells.data(), (circuit.width * circuit.height) * sizeof(Cell));
}

bool Motherboard::robot_on_plate() {
    return cells[robot.position.x + robot.position.y * circuit.width].kind == Cell::PLATE;
}

bool Motherboard::power_reached_socket() {
    if (powered_cells.empty()) {
        return false;
    }

    Vector2 last_powered_cell = powered_cells.back();
    return cells[last_powered_cell.x + last_powered_cell.y * circuit.width].kind == Cell::SOCKET;
}

Step_Result Motherboard::step() {
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
                if (nx < 0 || nx >= circuit.width ||
                    ny < 0 || ny >= circuit.height)
                {
                    break;
                }

                Cell ncell = cells[nx + ny * circuit.width];
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
                    case Cell::REDIRECTOR: {
                        robot.direction = ncell.redirector.direction;
                        break;
                    }
                    case Cell::PORTAL: {
                        if (ncell.portal.active) {
                            robot.position = {
                                ncell.portal.link % circuit.width,
                                ncell.portal.link / circuit.width
                            };
                        }
                        break;
                    }
                    case Cell::TRAP: {
                        if (i == mult - 1) {
                            return STEP_FAIL;
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
                if (nx < 0 || nx >= circuit.width ||
                    ny < 0 || ny >= circuit.height)
                {
                    break;
                }

                Cell& ncell = cells[nx + ny * circuit.width];
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

Step_Power_Result Motherboard::step_power() {
    if (power_reached_socket()) {
        return STEP_POWER_COMPLETE_SOCKET_ACTIVATED;
    }

    if (powered_cells.empty()) {
        powered_cells.push_back(circuit.power_emitter.position);
        return STEP_POWER_PENDING;
    }

    Vector2i last = powered_cells.back();
    int nx = last.x + power_direction.x;
    int ny = last.y + power_direction.y;

    Cell ncell = cells[nx + ny * circuit.width];
    switch (ncell.kind) {
        case Cell::WALL: {
            return STEP_POWER_COMPLETE;
        }
        case Cell::BARRIER: {
            if (ncell.barrier.active) {
                return STEP_POWER_COMPLETE;
            } else {
                powered_cells.push_back({ nx, ny });
            }
            break;
        }
        case Cell::SOCKET: {
            powered_cells.push_back({ nx, ny });
            return STEP_POWER_COMPLETE_SOCKET_ACTIVATED;
        }
        case Cell::REFLECTOR: {
            if (ncell.reflector.flipped) {
                if (power_direction.x != 0) {
                    float x = power_direction.x;
                    power_direction.x = power_direction.y;
                    power_direction.y = -x;
                } else {
                    float x = power_direction.x;
                    power_direction.x = -power_direction.y;
                    power_direction.y = x;
                }
            } else {
                if (power_direction.x != 0) {
                    float x = power_direction.x;
                    power_direction.x = -power_direction.y;
                    power_direction.y = x;
                } else {
                    float x = power_direction.x;
                    power_direction.x = power_direction.y;
                    power_direction.y = -x;
                }
            }

            powered_cells.push_back({ nx, ny });
            break;
        }
        case Cell::REDIRECTOR: {
            power_direction = ncell.redirector.direction;
            powered_cells.push_back({ nx, ny });
            break;
        }
        case Cell::PORTAL: {
            TODO("power portal cell.");
            break;
        }

        // Not special
        case Cell::AIR:
        case Cell::PLATE:
        case Cell::TRAP:
        {
            powered_cells.push_back({ nx, ny });
            break;
        }
    }

    return STEP_POWER_PENDING;
}
