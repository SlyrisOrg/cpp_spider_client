//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_CONFIGURATION_HPP
#define SPIDER_CLIENT_CONFIGURATION_HPP

#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <log/Logger.hpp>

namespace lg = logging;
namespace utl = utils;
namespace asio = boost::asio;
namespace fs = boost::filesystem;


namespace spi::cfg
{
    constexpr unsigned short port = 31337;
    constexpr const char *address = "127.0.0.1";
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
