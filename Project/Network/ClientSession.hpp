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
        }

        ~ClientSession() override = default;

        template <typename LogHandleCallback>
        void onConnectSuccess(LogHandleCallback&& callback)
        {
            _connectSuccessCallback = callback;
        }

        template <typename LogHandleCallback>
        void onConnectFailure(LogHandleCallback&& callback)
        {
            _connectFailureCallback = callback;
        }

        void connect() noexcept
        {
            _sslConnection.asyncConnect(_conf.address, _conf.port,
                                        boost::bind(&ClientSession::__handleConnect, this, net::ErrorPlaceholder));
        }

        net::SSLConnection &getConnection()
        {
            return _sslConnection;
        }

        void close() noexcept
        {
            _log(logging::Info) << "Disconnecting from server..." << std::endl;
            _sslConnection.rawSocket().close();
        }

    private:

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
                _connectSuccessCallback();
            } else {
                _connectFailureCallback();
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
                hello.serialize(buff);
                _sslConnection.asyncWriteSome(buff, boost::bind(&ClientSession::__handleAuthentication,
                                                                this, net::ErrorPlaceholder));
            } else {
                _connectFailureCallback();
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
                _connectFailureCallback();
            } else {
                _log(logging::Info) << "Connected to server" << std::endl;
                _sslConnection.asyncHandshake(net::SSLConnection::HandshakeType::Client,
                                              boost::bind(&ClientSession::__handleHandshake,
                                                          this, net::ErrorPlaceholder));
            }
        }

        std::function<void()> _connectSuccessCallback{};
        std::function<void()> _connectFailureCallback{};
        net::SSLContext &_ctx;
        net::IOManager &_ioManager;
        net::Timer _timer;
        net::SSLConnection _sslConnection{_ioManager, _ctx};
        logging::Logger _log{"server-session", logging::Level::Info};
        cfg::Config _conf;
    };
}

#endif //SPIDER_CLIENT_CLIENTSESSION_HPP
