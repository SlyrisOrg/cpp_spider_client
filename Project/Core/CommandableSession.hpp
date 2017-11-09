//
// Created by doom on 07/10/17.
//

#ifndef SPIDER_COMMANDABLESESSION_HPP
#define SPIDER_COMMANDABLESESSION_HPP

#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <log/Logger.hpp>
#include <Network/ErrorCode.hpp>
#include <Network/BufferView.hpp>
#include <Network/SSLConnection.hpp>
#include <Protocol/CommandHandler.hpp>

namespace utils
{
    template <typename T>
    class InheritableSharedFromThis : public boost::enable_shared_from_this<InheritableSharedFromThis<T>>
    {
        using Parent = boost::enable_shared_from_this<InheritableSharedFromThis<T>>;

    public:
        boost::shared_ptr<T> shared_from_this()
        {
            return boost::static_pointer_cast<T>(Parent::shared_from_this());
        }

        template <typename To>
        boost::shared_ptr<To> shared_from_this_cast()
        {
            return boost::static_pointer_cast<To>(Parent::shared_from_this());
        }

        template <typename To, typename ...Args>
        static boost::shared_ptr<To> create(Args &&...args)
        {
            return boost::shared_ptr<To>(new To(std::forward<Args>(args)...));
        }

    };
}

namespace spi
{
    class CommandableSession : public utils::InheritableSharedFromThis<CommandableSession>
    {
    public:
        CommandableSession(net::IOManager &io, net::SSLContext &ctx, const std::string &name) :
            _conn(io, ctx), _log(name, logging::Level::Debug)
        {
        }

        virtual ~CommandableSession() noexcept = default;

        void onError(std::function<void(CommandableSession *)> &&fct) noexcept
        {
            _errorCb = std::forward<std::function<void(CommandableSession *)>>(fct);
        }

        template <typename CallBackT>
        void asyncHandshake(net::SSLConnection::HandshakeType type, CallBackT &&cb)
        {
            _conn.asyncHandshake(type, std::forward<CallBackT>(cb));
        }

        net::SSLConnection &connection() noexcept
        {
            return _conn;
        }

    protected:
        void handleHandshake(const ErrorCode &ec)
        {
            if (!ec) {
                __readCommandHeader();
            } else {
                _log(logging::Level::Warning) << "Unable to perform SSL handshake with client: "
                                              << ec.message() << std::endl;
                _errorCb(this);
            }
        }

        /** Command reading / handling */

    private:
        template <typename CallBackT>
        void __readData(CallBackT &&cb) noexcept
        {
            _conn.asyncReadSome(net::BufferView(_readBuff.data() + _nbReadBytes, _readBuff.size() - _nbReadBytes),
                                std::forward<CallBackT>(cb));
        }

        void __readCommandHeader() noexcept
        {
            _nbReadBytes = 0;
            _expectedSize = 4;
            _readBuff.resize(4);
            __readData(boost::bind(&CommandableSession::__handleCommandHeader, shared_from_this(),
                                   net::ErrorPlaceholder, net::BytesTransferredPlaceholder));
        }

        void __handleCommandHeader(const ErrorCode &ec, size_t bytesTransferred)
        {
            if (ec) {
                _log(logging::Level::Warning) << "Unable to read command header: " << ec.message() << std::endl;
                _errorCb(this);
                return;
            }

            _nbReadBytes += bytesTransferred;
            if (_nbReadBytes < _expectedSize) {
                __readData(boost::bind(&CommandableSession::__handleCommandHeader, shared_from_this(),
                                       net::ErrorPlaceholder, net::BytesTransferredPlaceholder));
            } else {
                auto type = _cmdHandler.identifyMessage(_readBuff);

                if (type == spi::proto::MessageType::Unknown) {
                    _log(logging::Level::Warning) << "Ignoring unrecognized command" << std::endl;
                    __readCommandHeader();
                } else {
                    __handleCommandType(type);
                }
            }
        }

        void __handleCommandType(proto::MessageType type)
        {
            size_t size;

            if (!_cmdHandler.canHandleCommand(type)) {
                _log(logging::Level::Warning) << "Rejecting unexpected command " << type.toString() << std::endl;
                _errorCb(this);
            } else if ((size = _cmdHandler.getSerializedSize(type)) == 0) {
                _cmdHandler.handleBinaryCommand(type, Buffer());
                __readCommandHeader();
            } else {
                __readCommandBody(type, size);
            }
        }

        void __readCommandBody(spi::proto::MessageType type, size_t size) noexcept
        {
            _nextCmdType = type;
            _nbReadBytes = 0;
            _expectedSize = size;
            _readBuff.resize(size);
            __readData(boost::bind(&CommandableSession::__handleCommand, shared_from_this(),
                                   net::ErrorPlaceholder, net::BytesTransferredPlaceholder));
        }

        void __handleCommand(const ErrorCode &ec, size_t bytesTransferred)
        {
            if (!ec) {
                _nbReadBytes += bytesTransferred;
                if (_nbReadBytes < _expectedSize) {
                    __readData(boost::bind(&CommandableSession::__handleCommand, shared_from_this(),
                                           net::ErrorPlaceholder, net::BytesTransferredPlaceholder));
                } else {
                    _cmdHandler.handleBinaryCommand(_nextCmdType, _readBuff);
                    __readCommandHeader();
                }
            } else {
                _log(logging::Level::Warning) << "Unable to read command data: " << ec.message() << std::endl;
                _errorCb(this);
            }
        }

    protected:
        net::SSLConnection _conn;
        logging::Logger _log;
        CommandHandler _cmdHandler;
        std::function<void(CommandableSession *)> _errorCb{[](CommandableSession *) {}};

        Buffer _readBuff;
        size_t _nbReadBytes{0};
        size_t _expectedSize{0};
        proto::MessageType _nextCmdType{proto::MessageType::Unknown};
    };
}

#endif //SPIDER_COMMANDABLESESSION_HPP
