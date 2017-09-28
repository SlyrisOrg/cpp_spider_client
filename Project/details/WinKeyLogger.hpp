//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_WINKEYLOGGER_HPP
#define SPIDER_CLIENT_WINKEYLOGGER_HPP

#include <memory>
#include <utils/Config.hpp>
#include "KeyLogger.hpp"

namespace spi
{
    class WinKeyLogger : public KeyLogger
    {
    public:
        explicit WinKeyLogger(LogHandle &logHandle) : _logHandle(logHandle)
        {
        };
        virtual ~WinKeyLogger() = default;

    private:
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
