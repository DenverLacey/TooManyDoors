#include "update.hpp"

#include "circuit.hpp"
#include "render.hpp"

void update_main_menu(Motherboard& mb) {
    UNUSED(mb);
    TODO("%s", __func__);
}

Update_Level_Result update_level(
    Motherboard& mb,
    Rectangle circuit_area,
    Rectangle runes_area,
    Rectangle scratchpad_area)
{
    bool step_requested = false;

    render_circuit(mb, circuit_area);

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
            if (!(rune & mb.circuit.available_runes)) {
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
                if ((int)mb.sentence.size() < mb.circuit.max_runes_allowed) {
                    mb.sentence.push_back(rune);
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

            render_play_button(play_button_area, mb.playing, button_hovered(play_button_area));
            if (button_pressed(play_button_area)) {
                mb.playing = true;
            }

            Rectangle pause_button_area = play_button_area;
            pause_button_area.x += play_button_area.width + render_ui_margin;

            render_pause_button(pause_button_area, button_hovered(pause_button_area));
            if (button_pressed(pause_button_area)) {
                mb.playing = false;
            }

            Rectangle step_button_area = pause_button_area;
            step_button_area.x += pause_button_area.width + render_ui_margin;

            render_step_button(step_button_area, button_hovered(step_button_area));
            if (button_pressed(step_button_area)) {
                mb.playing = false;
                step_requested = true;
            }

            Rectangle clear_button_area = step_button_area;
            clear_button_area.x += step_button_area.width + render_ui_margin;

            render_clear_button(clear_button_area, button_hovered(clear_button_area));
            if (button_pressed(clear_button_area)) {
                mb.reload();
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
            if (slot_width * (mb.circuit.max_runes_allowed / num_rows) <= inner_area.width) {
                break;
            }
            num_rows++;
        }

        float slot_height = inner_area.height / (float)num_rows;
        int num_cols = inner_area.width / (float)slot_width;

        int i;
        for (i = 0; i < (int)mb.sentence.size(); i++) {
            int col = i % num_cols;
            int row = i / num_cols;
            float x = inner_area.x + col * slot_width + slot_height * 0.1f;
            float y = inner_area.y + row * slot_height + slot_height * 0.1f;
            bool current_rune = i == mb.ip;
            render_rune(
                mb.sentence[i],
                x,
                y,
                slot_width - slot_height * 0.2f,
                slot_height - slot_height * 0.2f,
                current_rune
            );
        }

        for (; i < mb.circuit.max_runes_allowed; i++) {
            int col = i % num_cols;
            int row = i / num_cols;
            float x = inner_area.x + col * slot_width + slot_height * 0.1f;
            float y = inner_area.y + row * slot_height + slot_height * 0.1f;
            render_button(x, y, slot_width - slot_height * 0.2f, slot_height - slot_height * 0.2f);
        }
    }

    // Update circuit
    if ((mb.playing || step_requested) && GetTime() - mb.time_of_last_step >= CIRCUIT_CLOCK_SPEED) {
        mb.time_of_last_step = GetTime();
        Step_Result result = mb.step();
        if (result != STEP_OK) {
            mb.playing = false;
        }

        if (result == STEP_FAIL) {
            return UPDATE_LEVEL_FAILED;
        }
    }

    Update_Level_Result result = UPDATE_LEVEL_OK; 
    if (mb.robot_on_plate() && GetTime() - mb.time_of_last_step >= CIRCUIT_CLOCK_SPEED) {
        mb.time_of_last_step = GetTime();

        switch (mb.step_power()) {
            case STEP_POWER_ERROR: {
                result = UPDATE_LEVEL_ERROR;
                break;
            }
            case STEP_POWER_PENDING: {
                // do nothing
                break;
            }
            case STEP_POWER_COMPLETE: {
                // do nothing
                break;
            }
            case STEP_POWER_COMPLETE_SOCKET_ACTIVATED: {
                result = UPDATE_LEVEL_PASSED;
                break;
            }
        }
    }

    return result;
}

bool button_hovered(Rectangle button_area) {
    return CheckCollisionPointRec(GetMousePosition(), button_area);
}

bool button_pressed(Rectangle button_area) {
    return IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
           button_hovered(button_area);
}
