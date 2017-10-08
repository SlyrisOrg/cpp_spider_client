//
// Created by baptiste montagliani on 02/10/2017.
//

#ifndef OSX_KEY_MATCHING_TABLE_HPP
#define OSX_KEY_MATCHING_TABLE_HPP

#include <unordered_map>
#include <Protocol/Messages.hpp>

namespace spi {

    static const std::unordered_map<char, proto::KeyCode> toBinds = {
            {0,   proto::KeyCode::a},
            {1,   proto::KeyCode::s},
            {2,   proto::KeyCode::d},
            {3,   proto::KeyCode::f},
            {4,   proto::KeyCode::h},
            {5,   proto::KeyCode::g},
            {6,   proto::KeyCode::z},
            {7,   proto::KeyCode::x},
            {8,   proto::KeyCode::c},
            {9,   proto::KeyCode::v},
            {11,  proto::KeyCode::b},
            {12,  proto::KeyCode::q},
            {13,  proto::KeyCode::w},
            {14,  proto::KeyCode::e},
            {15,  proto::KeyCode::r},
            {16,  proto::KeyCode::y},
            {17,  proto::KeyCode::t},
            {18,  proto::KeyCode::_1},
            {19,  proto::KeyCode::_2},
            {20,  proto::KeyCode::_3},
            {21,  proto::KeyCode::_4},
            {22,  proto::KeyCode::_6},
            {23,  proto::KeyCode::_5},
//       {24, proto::KeyCode}, //=
            {25,  proto::KeyCode::_9},
            {26,  proto::KeyCode::_7},
//       {27, proto::KeyCode},//-
            {28,  proto::KeyCode::_8},
            {29,  proto::KeyCode::_0},
//       {30, proto::KeyCode}, //]
            {31,  proto::KeyCode::o},
            {32,  proto::KeyCode::u},
//       {33, proto::KeyCode}, //[
            {34,  proto::KeyCode::i},
            {35,  proto::KeyCode::p},
//       {36, proto::KeyCode}, //RETURN
            {37,  proto::KeyCode::l},
            {38,  proto::KeyCode::j},
//       {39, proto::KeyCode}, //"
            {40,  proto::KeyCode::k},
//       {41, proto::KeyCode}, //;
//       {42, proto::KeyCode}, //\
//       {43, proto::KeyCode}, //,
//       {44, proto::KeyCode}, // /
            {45,  proto::KeyCode::n},
            {46,  proto::KeyCode::m},
//       {47, proto::KeyCode}, //.
            {48,  proto::KeyCode::Tab},
            {49,  proto::KeyCode::Space},
//       {50, proto::KeyCode}, //`
//       {51, proto::KeyCode}, //DELETE
            {53,  proto::KeyCode::Escape},
//            {55,  proto::KeyCode}, // COMMAND
            {56,  proto::KeyCode::Shift},
            {57,  proto::KeyCode::CapsLock},
            {58,  proto::KeyCode::Alt},
            {59,  proto::KeyCode::Ctrl},
            {60,  proto::KeyCode::Shift}, //right
            {61,  proto::KeyCode::Alt}, //right
            {62,  proto::KeyCode::Ctrl}, //right
            {63,  proto::KeyCode::Fn},
//    {64, proto::KeyCode::F17},
//    {65, proto::KeyCode::F17}, //keypad-decimal
//    {67, proto::KeyCode::F17}, //keypad-multiply
//    {69, proto::KeyCode::F17}, //keypad-plus
//    {71, proto::KeyCode::F17}, //keypad-clear
//    {72, proto::KeyCode::F17}, //volume-up
//    {73, proto::KeyCode::F17}, //volume-down
//    {74, proto::KeyCode::F17}, //mute
//    {75, proto::KeyCode::F17}, //keypad-divide
//    {76, proto::KeyCode::F17}, //keypad-enter
//    {78, proto::KeyCode::F17}, //keypad-minus
//    {79, proto::KeyCode::F18},
//    {80, proto::KeyCode::F19},
//    {81, proto::KeyCode::F17}, //keypad-equals
            {82,  proto::KeyCode::_0}, //keypad
            {83,  proto::KeyCode::_1}, //keypad
            {84,  proto::KeyCode::_2}, //keypad
            {85,  proto::KeyCode::_3}, //keypad
            {86,  proto::KeyCode::_4}, //keypad
            {87,  proto::KeyCode::_5}, //keypad
            {88,  proto::KeyCode::_6}, //keypad
            {89,  proto::KeyCode::_7}, //keypad
//    {90, proto::KeyCode::F20}, //keypad
            {91,  proto::KeyCode::_8}, //keypad
            {92,  proto::KeyCode::_9}, //keypad
            {96,  proto::KeyCode::F5},
            {97,  proto::KeyCode::F6},
            {98,  proto::KeyCode::F7},
            {99,  proto::KeyCode::F3},
            {100, proto::KeyCode::F8},
            {101, proto::KeyCode::F9},
            {103, proto::KeyCode::F11},
//    {105, proto::KeyCode::F13},
//    {106, proto::KeyCode::F16},
//    {107, proto::KeyCode::F14},
            {109, proto::KeyCode::F10},
            {111, proto::KeyCode::F12},
//    {113, proto::KeyCode::F15},
//    {114, proto::KeyCode}, //help
            {115, proto::KeyCode::Home},
            {116, proto::KeyCode::PageUp},
//    {117, proto::KeyCode}, //forward-delete
            {118, proto::KeyCode::F4},
//    {119, proto::KeyCode}, //end
            {120, proto::KeyCode::F2},
            {121, proto::KeyCode::PageDown},
            {122, proto::KeyCode::F1},
            {123, proto::KeyCode::Left},
            {124, proto::KeyCode::Right},
            {125, proto::KeyCode::Down},
            {126, proto::KeyCode::Up}
    };
}

#endif //OSX_KEY_MATCHING_TABLE_HPP
