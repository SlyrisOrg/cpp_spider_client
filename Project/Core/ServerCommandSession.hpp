//
// Created by doom on 08/10/17.
//

#ifndef SPIDER_CLIENT_SERVERCOMMANDSESSION_HPP
#define SPIDER_CLIENT_SERVERCOMMANDSESSION_HPP

#include <Core/CommandableSession.hpp>

namespace spi
{
    class ServerCommandSession : public CommandableSession
    {
    public:
        ServerCommandSession(net::IOManager &io, net::SSLContext &ctx) : CommandableSession(io, ctx, "server-cmd")
        {
            _cmdHandler.onMessages(boost::bind(&ServerCommandSession::__handleStealthMode, this, _1),
                                   proto::MessageType::StealthMode);
        }

        ~ServerCommandSession() noexcept override
        {
        }

        void startSession() noexcept
        {
            _log(logging::Level::Debug) << "Ready to receive commands" << std::endl;
            asyncHandshake(net::SSLConnection::HandshakeType::Server,
                           boost::bind(&ServerCommandSession::handleHandshake, this, net::ErrorPlaceholder));
        }

    private:
        void __handleStealthMode([[maybe_unused]] const ILoggable &s)
        {
            _log(logging::Level::Debug) << "Got stealth mode" << std::endl;
            //Do stuff to switch to stealth mode (call Viral class, etc)
        }
    };
}

#endif //SPIDER_CLIENT_SERVERCOMMANDSESSION_HPP
