//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_CONFIGURATION_HPP
#define SPIDER_CLIENT_CONFIGURATION_HPP

#include <config/PlatformConfig.hpp>

#ifdef USING_UNIX
# define FILENAME "spider_client"
#endif
#ifdef USING_WINDOWS
# define FILENAME "spider_client.exe"
#endif

namespace spi::cfg
{
    struct Config
    {
        unsigned short port;
        unsigned short portAcceptor;
        std::string address;
        std::string logDir;
        std::string keyFile;
        std::string certFile;
        long retryTime;
    };

    constexpr const char *filename = FILENAME;
}

#endif //SPIDER_CLIENT_CONFIGURATION_HPP
