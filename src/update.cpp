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
    render_circuit(circuit, circuit_area);

    // Rune Table
    {
        DrawRectangleRec(runes_area, ColorAlphaBlend(BLACK, GRAY, LIGHTGRAY));
        DrawRectangleLinesEx(runes_area, render_thickness, DARKGRAY);

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

            if (button(rune_slot_area)) {
                if ((int)circuit.sentence.size() < circuit.original.max_runes_allowed) {
                    circuit.sentence.push_back(rune);
                }
            }

            bool hovered = CheckCollisionPointRec(GetMousePosition(), rune_slot_area);

            Render_Rune_Func render_func = RENDER_RUNE_FUNCS[i];
            render_func(rune_area_x, rune_area_y, rune_area_width, rune_area_height, hovered);
            g++;
        }
    }

    // Scratchpad
    {
        DrawRectangleRec(scratchpad_area, ColorAlphaBlend(BLACK, GRAY, LIGHTGRAY));
        DrawRectangleLinesEx(scratchpad_area, render_thickness, DARKGRAY);


        int num_rows = 1;
        float slot_width;
        for (;;) {
            slot_width = (scratchpad_area.height / (float)num_rows) / 3.f * 2.f;
            if (slot_width * (circuit.original.max_runes_allowed / num_rows) <= scratchpad_area.width) {
                break;
            }
            num_rows++;
        }

        float slot_height = scratchpad_area.height / (float)num_rows;
        int num_cols = scratchpad_area.width / (float)slot_width;

        int i;
        for (i = 0; i < (int)circuit.sentence.size(); i++) {
            int col = i % num_cols;
            int row = i / num_cols;
            float x = scratchpad_area.x + col * slot_width + slot_height * 0.1f;
            float y = scratchpad_area.y + row * slot_height + slot_height * 0.1f;
            render_rune(circuit.sentence[i], x, y, slot_width - slot_height * 0.2f, slot_height - slot_height * 0.2f);
        }

        for (; i < circuit.original.max_runes_allowed; i++) {
            int col = i % num_cols;
            int row = i / num_cols;
            float x = scratchpad_area.x + col * slot_width + slot_height * 0.1f;
            float y = scratchpad_area.y + row * slot_height + slot_height * 0.1f;
            render_rune_background(x, y, slot_width - slot_height * 0.2f, slot_height - slot_height * 0.2f);
        }
    }
}

bool button(Rectangle button_area) {
    return IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
           CheckCollisionPointRec(GetMousePosition(), button_area);
}
