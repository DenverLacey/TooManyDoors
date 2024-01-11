#pragma once

#include <array>
#include "raylib.h"

#include "utils.hpp"
#include "runes.hpp"

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
struct Circuit_Info {
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

enum Tick_Result {
    TICK_ERROR_INVALID_SENTENCE,
    TICK_OK,
    TICK_COMPLETE,
    TICK_COMPLETE_PLATE_ACTIVATED,
};

struct Circuit_State {
    Robot robot;
    Execution_Mode mode;
    int ip, multiplier;
    std::vector<Rune> sentence;
    std::array<Cell, MAX_CIRCUIT_SIZE> cells;
    Circuit_Info original;

    void set(Circuit_Info circuit);
    void reset();
    Tick_Result tick();
};
