//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_CONFIGURATION_HPP
#define SPIDER_CLIENT_CONFIGURATION_HPP

#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <log/Logger.hpp>

#ifdef UNIX
# define FILENAME "spider_client"
#endif
#ifdef WIN32
# define FILENAME "spider_client.exe"
#endif

namespace lg = logging;
namespace utl = utils;
namespace asio = boost::asio;
namespace fs = boost::filesystem;


namespace spi::cfg
{
    constexpr unsigned short port = 31337;
    constexpr unsigned short portAcceptor = 31300;
    constexpr const char *address = "127.0.0.1";
    constexpr const char *filename = FILENAME;
}

namespace spi
{
    class Configuration
    {
    public:
        static void initialize()
        {
        };
    };
}

#endif //SPIDER_CLIENT_CONFIGURATION_HPP
