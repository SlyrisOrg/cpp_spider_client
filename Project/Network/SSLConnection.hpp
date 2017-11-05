//
// Created by doom on 27/09/17.
//

#ifndef SPIDER_SERVER_SSLCONNECTION_HPP
#define SPIDER_SERVER_SSLCONNECTION_HPP

#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <Network/SSLContext.hpp>
#include <Network/IOManager.hpp>

namespace asio = boost::asio;

namespace spi::net
{
    class SSLConnection
    {
    public:
        enum HandshakeType
        {
            Client,
            Server,
        };

        using Socket = asio::ssl::stream<asio::ip::tcp::socket>;

        SSLConnection(IOManager &service, SSLContext &ctx) noexcept : _socket(service.get(), ctx.get())
        {
        }

        template <typename CallBackT>
        void asyncConnect(const std::string &host, unsigned short port, CallBackT &&cb) noexcept
        {
            asio::ip::tcp::resolver resolver(_socket.get_io_service());
            asio::ip::tcp::resolver::query q(host, std::to_string(port));
            auto it = resolver.resolve(q);

            asio::async_connect(_socket.lowest_layer(), it, std::forward<CallBackT>(cb));
        }

        ErrorCode connect(const std::string &host, unsigned short port) noexcept
        {
            asio::ip::tcp::resolver resolver(_socket.get_io_service());
            asio::ip::tcp::resolver::query q(host, std::to_string(port));
            auto it = resolver.resolve(q);
            boost::system::error_code ec;

            asio::connect(_socket.lowest_layer(), it, ec);
            return ec;
        }

        template <typename CallBackT>
        void asyncHandshake(HandshakeType type, CallBackT &&cb) noexcept
        {
            _socket.async_handshake(static_cast<asio::ssl::stream_base::handshake_type>(type),
                                    std::forward<CallBackT>(cb));
        }

        ErrorCode handshake(HandshakeType type) noexcept
        {
            boost::system::error_code ec;

            _socket.handshake(static_cast<asio::ssl::stream_base::handshake_type>(type), ec);
            return ec;
        }

        template <typename BufferT, typename CallBackT>
        void asyncReadSome(BufferT buff, CallBackT &&cb) noexcept
        {
            _socket.async_read_some(boost::asio::buffer(buff.data(), buff.size()), std::forward<CallBackT>(cb));
        }

        template <typename BufferT>
        size_t readSome(BufferT buff, ErrorCode &ec) noexcept
        {
            return _socket.read_some(boost::asio::buffer(buff.data(), buff.size()), ec.get());
        }

        template <typename BufferT, typename CallBackT>
        void asyncWriteSome(const BufferT &buff, CallBackT &&cb) noexcept
        {
            _socket.async_write_some(boost::asio::buffer(buff.data(), buff.size()), std::forward<CallBackT>(cb));
        }

        template <typename BufferT>
        size_t writeSome(const BufferT &buff, ErrorCode &ec) noexcept
        {
            return _socket.write_some(boost::asio::buffer(buff.data(), buff.size()), ec.get());
        }

        Socket::lowest_layer_type &rawSocket() noexcept
        {
            return _socket.lowest_layer();
        }

        Socket &socket() noexcept
        {
            return _socket;
        }

        std::string getRemoteAddress() const noexcept
        {
            return _socket.lowest_layer().remote_endpoint().address().to_string();
        }

    private:
        Socket _socket;
    };
}

#endif //SPIDER_SERVER_SSLCONNECTION_HPP
