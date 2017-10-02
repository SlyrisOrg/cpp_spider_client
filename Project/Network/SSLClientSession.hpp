//
// Created by doom on 29/09/17.
//

#ifndef SPIDER_SERVER_SSLCLIENTSESSION_HPP
#define SPIDER_SERVER_SSLCLIENTSESSION_HPP

#include <Network/IOManager.hpp>
#include <Network/SSLConnection.hpp>

namespace spi::net
{
    class SSLClientSession
    {
    public:
        SSLClientSession(IOManager &io, net::SSLContext &ctx) : _io(io), _conn(io, ctx)
        {
        }

        virtual ~SSLClientSession() noexcept = default;

        template <typename CallBackT>
        void asyncHandshake(CallBackT &&cb)
        {
            _conn.asyncHandshake(net::SSLConnection::HandshakeType::Server,
                                 std::forward<CallBackT>(cb));
        }

        net::SSLConnection &connection() noexcept
        {
            return _conn;
        }

    protected:
        IOManager &_io;
        SSLConnection _conn;
    };
}

#endif //SPIDER_SERVER_SSLCLIENTSESSION_HPP
