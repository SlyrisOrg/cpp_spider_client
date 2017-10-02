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
        using InternalT = asio::ssl::context;

        enum Version
        {
            SSLv23 = asio::ssl::context::sslv23,
            SSLv23Server = asio::ssl::context::sslv23_server,
            SSLv23Client = asio::ssl::context::sslv23_client,
        };

        InternalT &get() noexcept
        {
            return _ctx;
        }

        SSLContext(Version v) noexcept : _ctx(static_cast<asio::ssl::context::method>(v))
        {
        }

        bool usePrivateKeyFile(const std::string &path) noexcept
        {
            try {
                _ctx.use_private_key_file(path, asio::ssl::context::pem);
            } catch (const std::exception &e) {
                return false;
            }
            return true;
        }

        bool useCertificateFile(const std::string &path) noexcept
        {
            try {
                _ctx.use_certificate_file(path, asio::ssl::context::pem);
            } catch (const std::exception &e) {
                return false;
            }
            return true;
        }

    private:
        InternalT _ctx;
    };
}

#endif //SPIDER_SERVER_SSLCONTEXT_HPP
