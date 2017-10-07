//
// Created by Blowa-XPS on 06-Oct-17.
//

#ifndef SPIDER_CLIENT_CLIENTSESSION_HPP
#define SPIDER_CLIENT_CLIENTSESSION_HPP

#include <boost/bind.hpp>
#include <utils/NonCopyable.hpp>
#include <Configuration.hpp>
#include <Protocol/Messages.hpp>
#include "SSLConnection.hpp"

namespace spi
{
    class ClientSession : utils::NonCopyable
    {
    public:
        ClientSession(net::SSLContext &ctx, net::IOManager &service) : _ctx(ctx), _ioManager(service)
        {
            if (!_ctx.usePrivateKeyFile("p.pem") || !_ctx.useCertificateFile("s.pem")) {
                _log(logging::Error) << "SSL Context loading error" << std::endl;
                close();
            }
        }

        ~ClientSession() override = default;

        void connect()
        {
            _sslConnection.asyncConnect(spi::cfg::address, spi::cfg::port, boost::bind(&ClientSession::handshakeSSL, this, net::ErrorPlaceholder));
        }

    private:
        void idontknow()
        {
        }

        void close()
        {
                _sslConnection.rawSocket().close();
                _log(logging::Info) << "Closing socket ..." << std::endl;
                _log(logging::Info) << "Closing input filedescriptor ..." << std::endl;
                _ioManager.stop();
                _log(logging::Info) << "Stopping IOService ..." << std::flush;
        }

        void auth(const ErrorCode &errorCode)
        {
            proto::Hello hello;

            hello.macAddress = "FFFFFFFF"; // GET MAC ADDRESS
            hello.md5 = utils::MD5(spi::cfg::filename);
            hello.port = spi::cfg::portAcceptor;
            hello.version = 1;
            spi::Buffer buffer(hello.serialize());
            _sslConnection.asyncWriteSome(buffer, boost::bind(&ClientSession::idontknow, this));
        }

        void handshakeSSL(const ErrorCode &errorCode)
        {
            if (errorCode)
            {
                // SCHEDULE CONNECT IN 10MIN
                return;
            }
            _sslConnection.asyncHandshake(net::SSLConnection::HandshakeType::Client, boost::bind(&ClientSession::auth, this, net::ErrorPlaceholder));
        }


        net::SSLContext &_ctx;
        net::IOManager &_ioManager;
        net::SSLConnection _sslConnection{_ioManager, _ctx};
        logging::Logger _log{"ClientSession", logging::Level::Info};
    };
}

#endif //SPIDER_CLIENT_CLIENTSESSION_HPP
