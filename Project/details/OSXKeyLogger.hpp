//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_OSXKEYLOGGER_HPP
#define SPIDER_CLIENT_OSXKEYLOGGER_HPP

#include <memory>
#include <utils/Config.hpp>
#include "KeyLogger.hpp"

#define OSX_LOG utl::LightMagenta <<"{'OSXKeyLogger'}"  << utl::Reset

namespace spi
{
    class OSXKeyLogger : public KeyLogger
    {
    public:
        explicit OSXKeyLogger(LogHandle &logHandle) noexcept : _logHandle(logHandle)
        {
        };
        ~OSXKeyLogger() override = default;

    public:
        void setup() override
        {
            _log(lg::Info) << OSX_LOG << " successfully initialized." << std::endl;
            onKeyboardEvent([](KeyEvent &&) { ; });
            onMouseClickEvent([](MouseClick &&) { ; });
            onMouseMoveEvent([](MouseMove &&) { ; });
        }

        void run() override
        {
            _log(lg::Info) << OSX_LOG << " virus started." << std::endl;
        }

        void stop() override
        {
            _log(lg::Info) << OSX_LOG << " virus stoped." << std::endl;
        }

    private:
        LogHandle &_logHandle;
    };
}

namespace spi::details
{
    static always_inline KeyLogPtr createKeyLogger()
    {
        return std::make_unique<OSXKeyLogger>();
    }
}

#endif //SPIDER_CLIENT_OSXKEYLOGGER_HPP
