//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_CONNECTION_HPP
#define SPIDER_CLIENT_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#define SOCKET_LOG utl::Yellow << "{'Connection'}" << utl::Reset

namespace spi
{
    class Connection
    {
    public:
        Connection() = default;
        ~Connection()
        {
            _log(lg::Info) << SOCKET_LOG << " shutting down." << std::endl;
        }
    public:
        void setup()
        {
            _log(lg::Info) << SOCKET_LOG << " successfully initialized." << std::endl;
        }

    private:
        lg::Logger _log{"client-connection", lg::Level::Debug};
    };
}

#endif //SPIDER_CLIENT_CONNECTION_HPP
