#include "runes.hpp"

// Result<Sentence> Sentence::make(Span<Rune> runes) {
//     std::vector<Command> cmds;

//     int num_repeats = 0;
//     for (Rune rune : runes) {
//         switch (rune) {
//             case RUNE_MOVE: {
//                 cmds.push_back(Command{ Command::MOVE, num_repeats });
//                 num_repeats = 0;
//                 break;
//             }
//             case RUNE_CHANGE_DIRECTION: {
//                 if (num_repeats == 0) {
//                     return {};
//                 }
//                 cmds.push_back(Command{ Command::CHANGE_DIRECTION, num_repeats });
//                 num_repeats = 0;
//                 break;
//             }
//             case RUNE_ROTATE: {
//                 cmds.push_back(Command{ Command::ROTATE, num_repeats });
//                 num_repeats = 0;
//                 break;
//             }
//             case RUNE_INTERACT: {
//                 if (num_repeats != 0) {
//                     return {};
//                 }
//                 cmds.push_back(Command{ Command::INTERACT });
//                 break;
//             }
//             case RUNE_REPEAT: {
//                 if (num_repeats != 0) {
//                     return {};
//                 }
//                 cmds.push_back(Command{ Command::REPEAT });
//                 break;
//             }
//             case RUNE_REVERSE: {
//                 if (num_repeats != 0) {
//                     return {};
//                 }
//                 cmds.push_back(Command{ Command::REVERSE });
//                 break;
//             }
//             case RUNE_FLOW_END: {
//                 if (num_repeats != 0) {
//                     return {};
//                 }
//                 cmds.push_back(Command{ Command::FLOW_END });
//                 break;
//             }
//             case RUNE_TWO: {
//                 if (num_repeats != 0) {
//                     return {};
//                 }
//                 num_repeats = 1;
//                 break;
//             }
//             case RUNE_THREE: {
//                 if (num_repeats != 0) {
//                     return {};
//                 }
//                 num_repeats = 2;
//                 break;
//             }
//             case RUNE_FOUR: {
//                 if (num_repeats != 0) {
//                     return {};
//                 }
//                 num_repeats = 3;
//                 break;
//             }
//         }
//     }

//     return { { std::move(cmds) } };
// }
