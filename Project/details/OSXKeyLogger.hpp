//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_OSXKEYLOGGER_HPP
#define SPIDER_CLIENT_OSXKEYLOGGER_HPP

#include <memory>
#include <utils/Config.hpp>
#include <Network/IOManager.hpp>
#include "KeyLogger.hpp"

#define OSX_LOG "{'OSXKeyLogger'}"

namespace spi
{
    class OSXKeyLogger : public KeyLogger
    {
    public:
        explicit OSXKeyLogger(net::IOManager &service) : _service(service)
        {}

        ~OSXKeyLogger() override = default;

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
        spi::net::IOManager &_service;
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
