#pragma once

#include <array>
#include "raylib.h"

#include "utils.hpp"
#include "runes.hpp"

#define CIRCUIT_CLOCK_SPEED (1.0)

struct Cell_Data_Barrier {
    bool active;
};

struct Cell_Data_Reflector {
    bool flipped;
};

struct Cell_Data_Redirector {
    Vector2i direction;
};

struct Cell_Data_Portal {
    bool active;
    int link;
};

struct Cell {
    enum Kind {
        AIR,           // Empty spcae that can be freely moved through.
        WALL,          // A wall that cannot be moved through or turned off.
        BARRIER,       // A barrier that cannot be moved through while active.
        SOCKET,        // The destination for the power.
        PLATE,         // The pressure plate that turns on the power.
        REFLECTOR,     // Redirects the robot and power 90°.
        REDIRECTOR,    // Redirects the robot and power to a certain direction.
        PORTAL,        // Teleports the robot and power to another location on the circuit.
        TRAP,          // Robot is destroyed if it touches a trap cell.
    } kind;

    union {
        Cell_Data_Barrier barrier;
        Cell_Data_Reflector reflector;
        Cell_Data_Redirector redirector;
        Cell_Data_Portal portal;
    };
};

struct Robot {
    Vector2i position, direction;
};

struct Power_Emitter {
    Vector2i position, direction;
};

#define MAX_CIRCUIT_WIDTH (25)
#define MAX_CIRCUIT_HEIGHT (20)
#define MAX_CIRCUIT_SIZE (MAX_CIRCUIT_WIDTH * MAX_CIRCUIT_HEIGHT)

// Defintion of a circuit puzzle.
struct Circuit {
    Rune_Set available_runes;
    int max_runes_allowed;
    int width, height;
    Robot robot;
    Power_Emitter power_emitter;
    std::array<Cell, MAX_CIRCUIT_SIZE> cells;
};

enum Execution_Mode {
    EXE_NORMAL,
    EXE_REPEAT,
    EXE_REVERSE,
};

enum Step_Result {
    STEP_ERROR_INVALID_SENTENCE,
    STEP_OK,
    STEP_FAIL,
    STEP_COMPLETE,
    STEP_COMPLETE_PLATE_ACTIVATED,
};

enum Step_Power_Result {
    STEP_POWER_ERROR,
    STEP_POWER_PENDING,
    STEP_POWER_COMPLETE,
    STEP_POWER_COMPLETE_SOCKET_ACTIVATED,
};

struct Motherboard {
    bool playing;
    double time_of_last_step;
    Robot robot;
    Vector2i power_direction;
    Execution_Mode mode;
    int ip, multiplier;
    std::vector<Rune> sentence;
    std::vector<Vector2i> powered_cells;
    std::array<Cell, MAX_CIRCUIT_SIZE> cells;
    Circuit circuit;

    void load(const Circuit& circuit);
    void reload();

    bool robot_on_plate();
    bool power_reached_socket();

    Step_Result step();
    Step_Power_Result step_power();
};
