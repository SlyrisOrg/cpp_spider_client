//
// Created by doom on 28/09/17.
//

#ifndef SPIDER_SERVER_SSLCONTEXT_HPP
#define SPIDER_SERVER_SSLCONTEXT_HPP

#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;

namespace spi::net
{
    class SSLContext
    {
    public:
        enum Version
        {
            SSLv23 = asio::ssl::context::sslv23,
            SSLv23Server = asio::ssl::context::sslv23_server,
            SSLv23Client = asio::ssl::context::sslv23_client,
        };

        auto &get() noexcept
        {
            return _ctx;
        }

        SSLContext(Version v) noexcept : _ctx(static_cast<asio::ssl::context::method>(v))
        {
        }

        void usePrivateKeyFile(const std::string &path) noexcept
        {
            _ctx.use_private_key_file(path, asio::ssl::context::pem);
        }

        void useCertificateFile(const std::string &path) noexcept
        {
            _ctx.use_certificate_file(path, asio::ssl::context::pem);
        }

    private:
        asio::ssl::context _ctx;
    };
}

#endif //SPIDER_SERVER_SSLCONTEXT_HPP
