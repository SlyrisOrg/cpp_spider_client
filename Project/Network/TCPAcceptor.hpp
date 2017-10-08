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
        TCPAcceptor(net::IOManager &io) noexcept : _acc(io.get())
        {
        }

        void bind(unsigned short port)
        {
            _acc.open(asio::ip::tcp::v4());
            _acc.bind(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
            _acc.listen(5);
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
