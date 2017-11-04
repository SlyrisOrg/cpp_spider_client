class KeyCode
{
public:
    enum enum_KeyCode
    {
        a,
        b,
        c,
        d,
        e,
        f,
        g,
        h,
        i,
        j,
        k,
        l,
        m,
        n,
        o,
        p,
        q,
        r,
        s,
        t,
        u,
        v,
        w,
        x,
        y,
        z,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        _0,
        _1,
        _2,
        _3,
        _4,
        _5,
        _6,
        _7,
        _8,
        _9,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        Escape,
        Tab,
        CapsLock,
        Shift,
        Ctrl,
        Fn,
        Home,
        Alt,
        Space,
        AltGr,
        Left,
        Right,
        Up,
        Down,
        PageDown,
        PageUp,
        End,
        Start,
        Inser,
        Backspace,
        Delete,
        Enter,
        AcuteE,
        GraveE,
        CedillaC,
        GraveA,
        GraveU,
        Comma,
        Semicolon,
        Colon,
        Exclamation,
        Interrogation,
        Dot,
        Slash,
        Backslash,
        Underscore,
        Pipe,
        Selection,
        Ampersand,
        Tilde,
        Sharp,
        Dollar,
        Euro,
        Pound,
        Currency,
        Caret,
        Trema,
        Arobase,
        Quote,
        DoubleQuote,
        BackQuote,
        Inferior,
        Superior,
        OpeningParenthesis,
        ClosingParenthesis,
        OpeningBracket,
        ClosingBracket,
        OpeningSquareBracket,
        ClosingSquareBracket,
        Plus,
        Dash,
        Star,
        Modulo,
        Equal,
        Mu,
        Round
    }; using EnumType = enum_KeyCode;

    constexpr KeyCode() noexcept : _value(a)
    {}

    constexpr KeyCode(EnumType value) noexcept : _value(value)
    {}

    KeyCode &operator=(EnumType value) noexcept
    {
        _value = value;
        return *this;
    }

    static constexpr const std::array<KeyCode, 140> values() noexcept
    { return {{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, Escape, Tab, CapsLock, Shift, Ctrl, Fn, Home, Alt, Space, AltGr, Left, Right, Up, Down, PageDown, PageUp, End, Start, Inser, Backspace, Delete, Enter, AcuteE, GraveE, CedillaC, GraveA, GraveU, Comma, Semicolon, Colon, Exclamation, Interrogation, Dot, Slash, Backslash, Underscore, Pipe, Selection, Ampersand, Tilde, Sharp, Dollar, Euro, Pound, Currency, Caret, Trema, Arobase, Quote, DoubleQuote, BackQuote, Inferior, Superior, OpeningParenthesis, ClosingParenthesis, OpeningBracket, ClosingBracket, OpeningSquareBracket, ClosingSquareBracket, Plus, Dash, Star, Modulo, Equal, Mu, Round}}; }

    class InvalidConversion : public std::exception
    {
    public:
        const char *what() const noexcept override
        { return "Unable to convert this string to an enum value"; }
    };

private:
    enum_KeyCode __stringToValue(const std::string_view v)
    {
        static constexpr const std::string_view strTab[] = {std::string_view("a", (sizeof("a""") - 1)),
                                                            std::string_view("b", (sizeof("b""") - 1)),
                                                            std::string_view("c", (sizeof("c""") - 1)),
                                                            std::string_view("d", (sizeof("d""") - 1)),
                                                            std::string_view("e", (sizeof("e""") - 1)),
                                                            std::string_view("f", (sizeof("f""") - 1)),
                                                            std::string_view("g", (sizeof("g""") - 1)),
                                                            std::string_view("h", (sizeof("h""") - 1)),
                                                            std::string_view("i", (sizeof("i""") - 1)),
                                                            std::string_view("j", (sizeof("j""") - 1)),
                                                            std::string_view("k", (sizeof("k""") - 1)),
                                                            std::string_view("l", (sizeof("l""") - 1)),
                                                            std::string_view("m", (sizeof("m""") - 1)),
                                                            std::string_view("n", (sizeof("n""") - 1)),
                                                            std::string_view("o", (sizeof("o""") - 1)),
                                                            std::string_view("p", (sizeof("p""") - 1)),
                                                            std::string_view("q", (sizeof("q""") - 1)),
                                                            std::string_view("r", (sizeof("r""") - 1)),
                                                            std::string_view("s", (sizeof("s""") - 1)),
                                                            std::string_view("t", (sizeof("t""") - 1)),
                                                            std::string_view("u", (sizeof("u""") - 1)),
                                                            std::string_view("v", (sizeof("v""") - 1)),
                                                            std::string_view("w", (sizeof("w""") - 1)),
                                                            std::string_view("x", (sizeof("x""") - 1)),
                                                            std::string_view("y", (sizeof("y""") - 1)),
                                                            std::string_view("z", (sizeof("z""") - 1)),
                                                            std::string_view("A", (sizeof("A""") - 1)),
                                                            std::string_view("B", (sizeof("B""") - 1)),
                                                            std::string_view("C", (sizeof("C""") - 1)),
                                                            std::string_view("D", (sizeof("D""") - 1)),
                                                            std::string_view("E", (sizeof("E""") - 1)),
                                                            std::string_view("F", (sizeof("F""") - 1)),
                                                            std::string_view("G", (sizeof("G""") - 1)),
                                                            std::string_view("H", (sizeof("H""") - 1)),
                                                            std::string_view("I", (sizeof("I""") - 1)),
                                                            std::string_view("J", (sizeof("J""") - 1)),
                                                            std::string_view("K", (sizeof("K""") - 1)),
                                                            std::string_view("L", (sizeof("L""") - 1)),
                                                            std::string_view("M", (sizeof("M""") - 1)),
                                                            std::string_view("N", (sizeof("N""") - 1)),
                                                            std::string_view("O", (sizeof("O""") - 1)),
                                                            std::string_view("P", (sizeof("P""") - 1)),
                                                            std::string_view("Q", (sizeof("Q""") - 1)),
                                                            std::string_view("R", (sizeof("R""") - 1)),
                                                            std::string_view("S", (sizeof("S""") - 1)),
                                                            std::string_view("T", (sizeof("T""") - 1)),
                                                            std::string_view("U", (sizeof("U""") - 1)),
                                                            std::string_view("V", (sizeof("V""") - 1)),
                                                            std::string_view("W", (sizeof("W""") - 1)),
                                                            std::string_view("X", (sizeof("X""") - 1)),
                                                            std::string_view("Y", (sizeof("Y""") - 1)),
                                                            std::string_view("Z", (sizeof("Z""") - 1)),
                                                            std::string_view("_0", (sizeof("_0""") - 1)),
                                                            std::string_view("_1", (sizeof("_1""") - 1)),
                                                            std::string_view("_2", (sizeof("_2""") - 1)),
                                                            std::string_view("_3", (sizeof("_3""") - 1)),
                                                            std::string_view("_4", (sizeof("_4""") - 1)),
                                                            std::string_view("_5", (sizeof("_5""") - 1)),
                                                            std::string_view("_6", (sizeof("_6""") - 1)),
                                                            std::string_view("_7", (sizeof("_7""") - 1)),
                                                            std::string_view("_8", (sizeof("_8""") - 1)),
                                                            std::string_view("_9", (sizeof("_9""") - 1)),
                                                            std::string_view("F1", (sizeof("F1""") - 1)),
                                                            std::string_view("F2", (sizeof("F2""") - 1)),
                                                            std::string_view("F3", (sizeof("F3""") - 1)),
                                                            std::string_view("F4", (sizeof("F4""") - 1)),
                                                            std::string_view("F5", (sizeof("F5""") - 1)),
                                                            std::string_view("F6", (sizeof("F6""") - 1)),
                                                            std::string_view("F7", (sizeof("F7""") - 1)),
                                                            std::string_view("F8", (sizeof("F8""") - 1)),
                                                            std::string_view("F9", (sizeof("F9""") - 1)),
                                                            std::string_view("F10", (sizeof("F10""") - 1)),
                                                            std::string_view("F11", (sizeof("F11""") - 1)),
                                                            std::string_view("F12", (sizeof("F12""") - 1)),
                                                            std::string_view("Escape", (sizeof("Escape""") - 1)),
                                                            std::string_view("Tab", (sizeof("Tab""") - 1)),
                                                            std::string_view("CapsLock", (sizeof("CapsLock""") - 1)),
                                                            std::string_view("Shift", (sizeof("Shift""") - 1)),
                                                            std::string_view("Ctrl", (sizeof("Ctrl""") - 1)),
                                                            std::string_view("Fn", (sizeof("Fn""") - 1)),
                                                            std::string_view("Home", (sizeof("Home""") - 1)),
                                                            std::string_view("Alt", (sizeof("Alt""") - 1)),
                                                            std::string_view("Space", (sizeof("Space""") - 1)),
                                                            std::string_view("AltGr", (sizeof("AltGr""") - 1)),
                                                            std::string_view("Left", (sizeof("Left""") - 1)),
                                                            std::string_view("Right", (sizeof("Right""") - 1)),
                                                            std::string_view("Up", (sizeof("Up""") - 1)),
                                                            std::string_view("Down", (sizeof("Down""") - 1)),
                                                            std::string_view("PageDown", (sizeof("PageDown""") - 1)),
                                                            std::string_view("PageUp", (sizeof("PageUp""") - 1)),
                                                            std::string_view("End", (sizeof("End""") - 1)),
                                                            std::string_view("Start", (sizeof("Start""") - 1)),
                                                            std::string_view("Inser", (sizeof("Inser""") - 1)),
                                                            std::string_view("Backspace", (sizeof("Backspace""") - 1)),
                                                            std::string_view("Delete", (sizeof("Delete""") - 1)),
                                                            std::string_view("Enter", (sizeof("Enter""") - 1)),
                                                            std::string_view("AcuteE", (sizeof("AcuteE""") - 1)),
                                                            std::string_view("GraveE", (sizeof("GraveE""") - 1)),
                                                            std::string_view("CedillaC", (sizeof("CedillaC""") - 1)),
                                                            std::string_view("GraveA", (sizeof("GraveA""") - 1)),
                                                            std::string_view("GraveU", (sizeof("GraveU""") - 1)),
                                                            std::string_view("Comma", (sizeof("Comma""") - 1)),
                                                            std::string_view("Semicolon", (sizeof("Semicolon""") - 1)),
                                                            std::string_view("Colon", (sizeof("Colon""") - 1)),
                                                            std::string_view("Exclamation",
                                                                             (sizeof("Exclamation""") - 1)),
                                                            std::string_view("Interrogation",
                                                                             (sizeof("Interrogation""") - 1)),
                                                            std::string_view("Dot", (sizeof("Dot""") - 1)),
                                                            std::string_view("Slash", (sizeof("Slash""") - 1)),
                                                            std::string_view("Backslash", (sizeof("Backslash""") - 1)),
                                                            std::string_view("Underscore",
                                                                             (sizeof("Underscore""") - 1)),
                                                            std::string_view("Pipe", (sizeof("Pipe""") - 1)),
                                                            std::string_view("Selection", (sizeof("Selection""") - 1)),
                                                            std::string_view("Ampersand", (sizeof("Ampersand""") - 1)),
                                                            std::string_view("Tilde", (sizeof("Tilde""") - 1)),
                                                            std::string_view("Sharp", (sizeof("Sharp""") - 1)),
                                                            std::string_view("Dollar", (sizeof("Dollar""") - 1)),
                                                            std::string_view("Euro", (sizeof("Euro""") - 1)),
                                                            std::string_view("Pound", (sizeof("Pound""") - 1)),
                                                            std::string_view("Currency", (sizeof("Currency""") - 1)),
                                                            std::string_view("Caret", (sizeof("Caret""") - 1)),
                                                            std::string_view("Trema", (sizeof("Trema""") - 1)),
                                                            std::string_view("Arobase", (sizeof("Arobase""") - 1)),
                                                            std::string_view("Quote", (sizeof("Quote""") - 1)),
                                                            std::string_view("DoubleQuote",
                                                                             (sizeof("DoubleQuote""") - 1)),
                                                            std::string_view("BackQuote", (sizeof("BackQuote""") - 1)),
                                                            std::string_view("Inferior", (sizeof("Inferior""") - 1)),
                                                            std::string_view("Superior", (sizeof("Superior""") - 1)),
                                                            std::string_view("OpeningParenthesis",
                                                                             (sizeof("OpeningParenthesis""") - 1)),
                                                            std::string_view("ClosingParenthesis",
                                                                             (sizeof("ClosingParenthesis""") - 1)),
                                                            std::string_view("OpeningBracket",
                                                                             (sizeof("OpeningBracket""") - 1)),
                                                            std::string_view("ClosingBracket",
                                                                             (sizeof("ClosingBracket""") - 1)),
                                                            std::string_view("OpeningSquareBracket",
                                                                             (sizeof("OpeningSquareBracket""") - 1)),
                                                            std::string_view("ClosingSquareBracket",
                                                                             (sizeof("ClosingSquareBracket""") - 1)),
                                                            std::string_view("Plus", (sizeof("Plus""") - 1)),
                                                            std::string_view("Dash", (sizeof("Dash""") - 1)),
                                                            std::string_view("Star", (sizeof("Star""") - 1)),
                                                            std::string_view("Modulo", (sizeof("Modulo""") - 1)),
                                                            std::string_view("Equal", (sizeof("Equal""") - 1)),
                                                            std::string_view("Mu", (sizeof("Mu""") - 1)),
                                                            std::string_view("Round", (sizeof("Round""") - 1)),};
        static constexpr const auto vals = values();
        for (size_t i = 0; i < (sizeof(strTab) / sizeof((strTab)[0])); ++i) { if (strTab[i] == v) { return vals[i]; }}
        throw InvalidConversion();
    }

public:
    KeyCode(const std::string_view v) : _value(__stringToValue(v))
    {}

    KeyCode &operator=(const std::string_view v)
    {
        _value = __stringToValue(v);
        return *this;
    }

    operator EnumType() const noexcept
    { return _value; }

    std::string toString() const noexcept
    {
        switch (_value) {
            case a:
                return "a";
            case b:
                return "b";
            case c:
                return "c";
            case d:
                return "d";
            case e:
                return "e";
            case f:
                return "f";
            case g:
                return "g";
            case h:
                return "h";
            case i:
                return "i";
            case j:
                return "j";
            case k:
                return "k";
            case l:
                return "l";
            case m:
                return "m";
            case n:
                return "n";
            case o:
                return "o";
            case p:
                return "p";
            case q:
                return "q";
            case r:
                return "r";
            case s:
                return "s";
            case t:
                return "t";
            case u:
                return "u";
            case v:
                return "v";
            case w:
                return "w";
            case x:
                return "x";
            case y:
                return "y";
            case z:
                return "z";
            case A:
                return "A";
            case B:
                return "B";
            case C:
                return "C";
            case D:
                return "D";
            case E:
                return "E";
            case F:
                return "F";
            case G:
                return "G";
            case H:
                return "H";
            case I:
                return "I";
            case J:
                return "J";
            case K:
                return "K";
            case L:
                return "L";
            case M:
                return "M";
            case N:
                return "N";
            case O:
                return "O";
            case P:
                return "P";
            case Q:
                return "Q";
            case R:
                return "R";
            case S:
                return "S";
            case T:
                return "T";
            case U:
                return "U";
            case V:
                return "V";
            case W:
                return "W";
            case X:
                return "X";
            case Y:
                return "Y";
            case Z:
                return "Z";
            case _0:
                return "_0";
            case _1:
                return "_1";
            case _2:
                return "_2";
            case _3:
                return "_3";
            case _4:
                return "_4";
            case _5:
                return "_5";
            case _6:
                return "_6";
            case _7:
                return "_7";
            case _8:
                return "_8";
            case _9:
                return "_9";
            case F1:
                return "F1";
            case F2:
                return "F2";
            case F3:
                return "F3";
            case F4:
                return "F4";
            case F5:
                return "F5";
            case F6:
                return "F6";
            case F7:
                return "F7";
            case F8:
                return "F8";
            case F9:
                return "F9";
            case F10:
                return "F10";
            case F11:
                return "F11";
            case F12:
                return "F12";
            case Escape:
                return "Escape";
            case Tab:
                return "Tab";
            case CapsLock:
                return "CapsLock";
            case Shift:
                return "Shift";
            case Ctrl:
                return "Ctrl";
            case Fn:
                return "Fn";
            case Home:
                return "Home";
            case Alt:
                return "Alt";
            case Space:
                return "Space";
            case AltGr:
                return "AltGr";
            case Left:
                return "Left";
            case Right:
                return "Right";
            case Up:
                return "Up";
            case Down:
                return "Down";
            case PageDown:
                return "PageDown";
            case PageUp:
                return "PageUp";
            case End:
                return "End";
            case Start:
                return "Start";
            case Inser:
                return "Inser";
            case Backspace:
                return "Backspace";
            case Delete:
                return "Delete";
            case Enter:
                return "Enter";
            case AcuteE:
                return "AcuteE";
            case GraveE:
                return "GraveE";
            case CedillaC:
                return "CedillaC";
            case GraveA:
                return "GraveA";
            case GraveU:
                return "GraveU";
            case Comma:
                return "Comma";
            case Semicolon:
                return "Semicolon";
            case Colon:
                return "Colon";
            case Exclamation:
                return "Exclamation";
            case Interrogation:
                return "Interrogation";
            case Dot:
                return "Dot";
            case Slash:
                return "Slash";
            case Backslash:
                return "Backslash";
            case Underscore:
                return "Underscore";
            case Pipe:
                return "Pipe";
            case Selection:
                return "Selection";
            case Ampersand:
                return "Ampersand";
            case Tilde:
                return "Tilde";
            case Sharp:
                return "Sharp";
            case Dollar:
                return "Dollar";
            case Euro:
                return "Euro";
            case Pound:
                return "Pound";
            case Currency:
                return "Currency";
            case Caret:
                return "Caret";
            case Trema:
                return "Trema";
            case Arobase:
                return "Arobase";
            case Quote:
                return "Quote";
            case DoubleQuote:
                return "DoubleQuote";
            case BackQuote:
                return "BackQuote";
            case Inferior:
                return "Inferior";
            case Superior:
                return "Superior";
            case OpeningParenthesis:
                return "OpeningParenthesis";
            case ClosingParenthesis:
                return "ClosingParenthesis";
            case OpeningBracket:
                return "OpeningBracket";
            case ClosingBracket:
                return "ClosingBracket";
            case OpeningSquareBracket:
                return "OpeningSquareBracket";
            case ClosingSquareBracket:
                return "ClosingSquareBracket";
            case Plus:
                return "Plus";
            case Dash:
                return "Dash";
            case Star:
                return "Star";
            case Modulo:
                return "Modulo";
            case Equal:
                return "Equal";
            case Mu:
                return "Mu";
            case Round:
                return "Round";
            default:
                return "";
        }
    }

    static std::string toString(KeyCode v)
    { return v.toString(); }

private:
    EnumType _value;
};
