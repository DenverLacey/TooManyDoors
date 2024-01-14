#include "update.hpp"

#include "circuit.hpp"
#include "render.hpp"

void update_main_menu(Circuit_State& circuit) {
    UNUSED(circuit);
    TODO("%s", __func__);
}

void update_level(
    Circuit_State& circuit,
    Rectangle circuit_area,
    Rectangle runes_area,
    Rectangle scratchpad_area)
{
    bool step_requested = false;

    render_circuit(circuit, circuit_area);

    // Rune Table
    {
        render_bordered_pane(runes_area);

        Rectangle inner_area = {
            .x = runes_area.x + render_ui_margin * 2,
            .y = runes_area.y + render_ui_margin,
            .width = runes_area.width - render_ui_margin * 4,
            .height = runes_area.height - render_ui_margin * 2
        };

        int num_rows = 4;
        int num_cols = 4;
        float rune_slot_width = inner_area.width / (float)num_cols;
        float rune_slot_height = inner_area.height / (float)num_rows;

        int g = 0;
        for (int i = 0; i < RUNE_COUNT; i++) {
            Rune rune = (Rune)(1 << i);
            if (!(rune & circuit.original.available_runes)) {
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

            if (button_pressed(rune_slot_area)) {
                if ((int)circuit.sentence.size() < circuit.original.max_runes_allowed) {
                    circuit.sentence.push_back(rune);
                }
            }

            bool hovered = button_hovered(rune_slot_area);

            Render_Rune_Func render_func = RENDER_RUNE_FUNCS[i];
            render_func(rune_area_x, rune_area_y, rune_area_width, rune_area_height, hovered);
            g++;
        }
    }

    // Scratchpad
    {
        render_bordered_pane(scratchpad_area);
    
        Rectangle control_bar_area = {
            .x = scratchpad_area.x,
            .y = scratchpad_area.y,
            .width = scratchpad_area.width,
            .height = scratchpad_area.height * 0.2f
        };

        Rectangle inner_area = {
            .x = scratchpad_area.x + render_ui_margin,
            .y = scratchpad_area.y + control_bar_area.height,
            .width = scratchpad_area.width - render_ui_margin * 2,
            .height = scratchpad_area.height - control_bar_area.height
        };

        // Control Bar
        {
            Rectangle play_button_area = {
                .x = control_bar_area.x + render_ui_margin,
                .y = control_bar_area.y + render_ui_margin,
                .width = control_bar_area.height - render_ui_margin * 2,
                .height = control_bar_area.height - render_ui_margin * 2
            };

            render_play_button(play_button_area, circuit.playing, button_hovered(play_button_area));
            if (button_pressed(play_button_area)) {
                circuit.playing = true;
            }

            Rectangle pause_button_area = play_button_area;
            pause_button_area.x += play_button_area.width + render_ui_margin;

            render_pause_button(pause_button_area, button_hovered(pause_button_area));
            if (button_pressed(pause_button_area)) {
                circuit.playing = false;
            }

            Rectangle step_button_area = pause_button_area;
            step_button_area.x += pause_button_area.width + render_ui_margin;

            render_step_button(step_button_area, button_hovered(step_button_area));
            if (button_pressed(step_button_area)) {
                circuit.playing = false;
                step_requested = true;
            }

            DrawLineEx(
                { control_bar_area.x, control_bar_area.y + control_bar_area.height },
                { control_bar_area.x + control_bar_area.width, control_bar_area.y + control_bar_area.height },
                render_thickness,
                render_border_color
            );
        }

        int num_rows = 1;
        float slot_width;
        for (;;) {
            slot_width = (inner_area.height / (float)num_rows) / 3.f * 2.f;
            if (slot_width * (circuit.original.max_runes_allowed / num_rows) <= inner_area.width) {
                break;
            }
            num_rows++;
        }

        float slot_height = inner_area.height / (float)num_rows;
        int num_cols = inner_area.width / (float)slot_width;

        int i;
        for (i = 0; i < (int)circuit.sentence.size(); i++) {
            int col = i % num_cols;
            int row = i / num_cols;
            float x = inner_area.x + col * slot_width + slot_height * 0.1f;
            float y = inner_area.y + row * slot_height + slot_height * 0.1f;
            bool current_rune = i == circuit.ip;
            render_rune(
                circuit.sentence[i],
                x,
                y,
                slot_width - slot_height * 0.2f,
                slot_height - slot_height * 0.2f,
                current_rune
            );
        }

        for (; i < circuit.original.max_runes_allowed; i++) {
            int col = i % num_cols;
            int row = i / num_cols;
            float x = inner_area.x + col * slot_width + slot_height * 0.1f;
            float y = inner_area.y + row * slot_height + slot_height * 0.1f;
            render_button(x, y, slot_width - slot_height * 0.2f, slot_height - slot_height * 0.2f);
        }
    }

    // Update circuit
    if ((circuit.playing || step_requested) && GetTime() - circuit.time_of_last_step >= CIRCUIT_CLOCK_SPEED) {
        circuit.time_of_last_step = GetTime();
        Step_Result result = circuit.step();
        if (result != STEP_OK) {
            circuit.playing = false;
        }
    }

    if (circuit.robot_on_plate()) {

    }
}

bool button_hovered(Rectangle button_area) {
    return CheckCollisionPointRec(GetMousePosition(), button_area);
}

bool button_pressed(Rectangle button_area) {
    return IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
           button_hovered(button_area);
}
