//
// Created by doom on 28/09/17.
//

#ifndef SPIDER_SERVER_TCPACCEPTOR_HPP
#define SPIDER_SERVER_TCPACCEPTOR_HPP

#include <boost/asio.hpp>
#include <Network/IOManager.hpp>

namespace asio = boost::asio;

namespace spi::net
{
    class TCPAcceptor
    {
    public:
        explicit TCPAcceptor(net::IOManager &io) noexcept : _acc(io.get())
        {
        }

        ErrorCode bind(unsigned short port)
        {
            ErrorCode err;

            _acc.open(asio::ip::tcp::v4(), err.get());
            if (!err)
                _acc.bind(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port), err.get());
            if (!err)
                _acc.listen(5, err.get());
            return err;
        }

        template <typename ConnectionT, typename CallBackT>
        void onAccept(ConnectionT &conn, CallBackT &&cb) noexcept
        {
            _acc.async_accept(conn.rawSocket(), std::forward<CallBackT>(cb));
        }

    private:
        asio::ip::tcp::acceptor _acc;
    };
}

#endif //SPIDER_SERVER_TCPACCEPTOR_HPP
