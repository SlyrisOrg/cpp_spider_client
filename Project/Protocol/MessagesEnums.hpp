//
// Created by doom on 29/09/17.
//

#ifndef SPIDER_SERVER_MESSAGESENUMS_HPP
#define SPIDER_SERVER_MESSAGESENUMS_HPP

#include <utils/Enums.hpp>

namespace spi::proto
{
    ENUM(MessageType,
         Unknown,
         ReplyCode,
         Bye,
         RawData,
         Hello,
         KeyEvent,
         MouseClick,
         MouseMove,
         ImageData,
         Screenshot,
         StealthMode,
         ActiveMode
    );

    ENUM(KeyCode,
         a, b, c, d, e,
         f, g, h, i, j,
         k, l, m, n, o,
         p, q, r, s, t,
         u, v, w, x, y,
         z,

         A, B, C, D, E,
         F, G, H, I, J,
         K, L, M, N, O,
         P, Q, R, S, T,
         U, V, W, X, Y,
         Z,

         _0, _1, _2, _3, _4,
         _5, _6, _7, _8, _9,

         F1, F2, F3, F4, F5, F6,
         F7, F8, F9, F10, F11, F12,

         Escape,

         Tab, CapsLock, Shift, Ctrl,
         Fn, Home, Alt, Space, AltGr,
         Left, Right, Up, Down,
         PageDown, PageUp,
         Backspace, Delete
    );

    ENUM(KeyState, Up, Down);

    ENUM(MouseButton,
         Left,
         Middle,
         Right
    );
}

#endif //SPIDER_SERVER_MESSAGESENUMS_HPP
