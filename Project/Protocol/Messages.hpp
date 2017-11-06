//
// Created by doom on 28/09/17.
//

#ifndef SPIDER_PROTO_MESSAGES_HPP
#define SPIDER_PROTO_MESSAGES_HPP

#include <cstring>
#include <chrono>
#include <vector>
#include <algorithm>
#include <utils/Utils.hpp>
#include <utils/Endian.hpp>
#include <utils/MD5.hpp>
#include <Utils/ILoggable.hpp>
#include <net/MACAddress.hpp>
#include <Protocol/MessagesEnums.hpp>
#include <utils/StringHelpers.hpp>

namespace spi::proto
{
    static constexpr const size_t MessageHeaderSize = sizeof(uint32_t);

    struct ReplyCode : public ILoggable
    {
        ReplyType code;

        static constexpr const size_t SerializedSize = 4;

        ReplyCode() noexcept = default;

        ReplyCode(const Buffer &buff)
        {
            code = static_cast<ReplyType::EnumType>(Serializer::unserializeInt(buff, 0));
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(code));
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::ReplyCode));
        }

        std::string stringify() const noexcept override
        {
            return "[ReplyCode] " + code.toString();
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::ReplyCode)), ", ",
                                         "\"code\": ", JSON::quote(code.toString()),
                                         " }");
        }
    };

    struct Bye : public ILoggable
    {
        static constexpr const size_t SerializedSize = 0;

        void serialize([[maybe_unused]] Buffer &out) const noexcept override
        {
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::Bye));
        }

        std::string stringify() const noexcept override
        {
            return "[Bye]";
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::Bye)),
                                         " }");
        }
    };

    struct RawData : public ILoggable
    {
        static constexpr const size_t SerializedSize = 4;

        std::vector<Byte> bytes;

        RawData() = default;

        explicit RawData(const Buffer &buff)
        {
            bytes.resize(Serializer::unserializeInt(buff, 0));
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + bytes.size() + 4);

            Serializer::serializeInt(out, static_cast<uint32_t>(bytes.size()));
            out.insert(out.end(), bytes.begin(), bytes.end());
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::RawData));
        }

        std::string stringify() const noexcept override
        {
            return "[RawData] size: " + std::to_string(bytes.size());
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::RawData)),
                                         " }");
        }
    };

    struct Hello : public ILoggable
    {
        ::net::MACAddress macAddress;
        utils::MD5 md5;
        uint16_t version;
        uint16_t port;

        static constexpr const size_t SerializedSize = 26;

        Hello() = default;

        Hello(const Buffer &buff)
        {
            macAddress = Serializer::unserializeMACAddress(buff, 0, 6);

            version = Serializer::unserializeShort(buff, 6);

            auto md5Bytes = Serializer::unserializeBytes(buff, 8, 16);
            utils::MD5::RawMD5 raw;
            std::copy(md5Bytes.begin(), md5Bytes.end(), raw.begin());
            md5.setRaw(raw);

            port = Serializer::unserializeShort(buff, 24);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize);

            std::vector<Byte> macAddrBytes(macAddress.raw().begin(), macAddress.raw().end());
            Serializer::serializeBytes(out, macAddrBytes);

            Serializer::serializeShort(out, version);

            std::vector<Byte> bytes(md5.raw().begin(), md5.raw().end());
            Serializer::serializeBytes(out, bytes);

            Serializer::serializeShort(out, port);
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::Hello));
        }

        std::string stringify() const noexcept override
        {
            std::stringstream ss;

            ss << "[Hello] ";
            ss << "MAC: " << macAddress.toString() << ", ";
            ss << "version: " << version << ", ";
            ss << "MD5: " << md5.toString() << ", ";
            ss << "port: " << port;
            return ss.str();
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::Hello)),
                                         " }");
        }
    };

    struct KeyEvent : public ILoggable
    {
        std::chrono::steady_clock::time_point timestamp;
        KeyCode code;
        KeyState state;

        static constexpr const size_t SerializedSize = 8 + 4 + 4;

        KeyEvent() = default;

        KeyEvent(const Buffer &buff)
        {
            timestamp = Serializer::unserializeTimestamp(buff, 0);
            code = static_cast<KeyCode::EnumType>(Serializer::unserializeInt(buff, 8));
            state = static_cast<KeyState::EnumType>(Serializer::unserializeInt(buff, 12));
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize);

            Serializer::serializeTimestamp(out, timestamp);
            Serializer::serializeInt(out, static_cast<uint32_t>(code));
            Serializer::serializeInt(out, static_cast<uint32_t>(state));
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::KeyEvent));
        }

        std::string stringify() const noexcept override
        {
            std::stringstream ss;

            ss << "[KeyEvent] ";
            ss << "timestamp: " << timestamp.time_since_epoch().count() << ", ";
            ss << "code: " << code.toString() << ", ";
            ss << "state: " << state.toString();
            return ss.str();
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::KeyEvent)), ", ",
                                         "\"timestamp\": ", timestamp.time_since_epoch().count(), ", ",
                                         "\"code\": ", JSON::quote(code.toString()), ", ",
                                         "\"state\": ", JSON::quote(state.toString()),
                                         " }");
        }
    };

    struct MouseClick : public ILoggable
    {
        std::chrono::steady_clock::time_point timestamp;
        uint32_t x;
        uint32_t y;
        KeyState state;
        MouseButton button;

        static constexpr const size_t SerializedSize = 8 + 4 + 4 + 4 + 4;

        MouseClick() = default;

        MouseClick(const Buffer &buff)
        {
            timestamp = Serializer::unserializeTimestamp(buff, 0);
            x = Serializer::unserializeInt(buff, 8);
            y = Serializer::unserializeInt(buff, 12);
            state = static_cast<KeyState::EnumType>(Serializer::unserializeInt(buff, 16));
            button = static_cast<MouseButton::EnumType>(Serializer::unserializeInt(buff, 20));
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize);

            Serializer::serializeTimestamp(out, timestamp);
            Serializer::serializeInt(out, x);
            Serializer::serializeInt(out, y);
            Serializer::serializeInt(out, static_cast<uint32_t>(state));
            Serializer::serializeInt(out, static_cast<uint32_t>(button));
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::MouseClick));
        }

        std::string stringify() const noexcept override
        {
            std::stringstream ss;

            ss << "[MouseClick] ";
            ss << "timestamp: " << timestamp.time_since_epoch().count() << ", ";
            ss << "x: " << x << ", ";
            ss << "y: " << y << ", ";
            ss << "state: " << state.toString() << ", ";
            ss << "button: " << button.toString();
            return ss.str();
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::MouseClick)),
                                         ", ",
                                         "\"timestamp\": ", timestamp.time_since_epoch().count(), ", ",
                                         "\"x\": ", x, ", ",
                                         "\"y\": ", y, ", ",
                                         "\"state\": ", JSON::quote(state.toString()), ", ",
                                         "\"button\": ", JSON::quote(button.toString()),
                                         " }");
        }
    };

    struct MouseMove : public ILoggable
    {
        std::chrono::steady_clock::time_point timestamp;
        uint32_t x;
        uint32_t y;

        static constexpr const size_t SerializedSize = 8 + 4 + 4;

        MouseMove() = default;

        MouseMove(const Buffer &buff)
        {
            timestamp = Serializer::unserializeTimestamp(buff, 0);
            x = Serializer::unserializeInt(buff, 8);
            y = Serializer::unserializeInt(buff, 12);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(SerializedSize);

            Serializer::serializeTimestamp(out, timestamp);
            Serializer::serializeInt(out, x);
            Serializer::serializeInt(out, y);
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::MouseMove));
        }

        std::string stringify() const noexcept override
        {
            std::stringstream ss;

            ss << "[MouseMove] ";
            ss << "timestamp: " << timestamp.time_since_epoch().count() << ", ";
            ss << "x: " << x << ", ";
            ss << "y: " << y << ", ";
            return ss.str();
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::MouseMove)), ", ",
                                         "\"timestamp\": ", timestamp.time_since_epoch().count(), ", ",
                                         "\"x\": ", x, ", ",
                                         "\"y\": ", y, ", ",
                                         " }");
        }
    };

    struct ImageData : public ILoggable
    {
        static constexpr const size_t SerializedSize = 4;

        std::vector<Byte> bytes;

        ImageData() = default;

        explicit ImageData(const Buffer &buff)
        {
            bytes.resize(Serializer::unserializeInt(buff, 0));
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + bytes.size() + sizeof(uint32_t));

            Serializer::serializeBuff(out, bytes);
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::ImageData));
        }

        std::string stringify() const noexcept override
        {
            return "[ImageData] size: " + std::to_string(bytes.size());
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::ImageData)),
                                         " }");
        }
    };

    struct StealthMode : public ILoggable
    {
        static constexpr const size_t SerializedSize = 0;

        void serialize([[maybe_unused]] Buffer &out) const noexcept override
        {
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::StealthMode));
        }

        std::string stringify() const noexcept override
        {
            return "[StealthMode] requested switch to stealth mode";
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::ImageData)),
                                         " }");
        }
    };

    struct ActiveMode : public ILoggable
    {
        static constexpr const size_t SerializedSize = 0;

        void serialize([[maybe_unused]] Buffer &out) const noexcept override
        {
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::ActiveMode));
        }

        std::string stringify() const noexcept override
        {
            return "[ActiveMode] requested switch to active mode";
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::ActiveMode)),
                                         " }");
        }
    };

    struct Screenshot : public ILoggable
    {
        static constexpr const size_t SerializedSize = 0;

        void serialize([[maybe_unused]] Buffer &out) const noexcept override
        {
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::Screenshot));
        }

        std::string stringify() const noexcept override
        {
            return "[Screenshot] requested screenshot";
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::Screenshot)),
                                         " }");
        }
    };

    struct RList : public ILoggable
    {
        static constexpr const size_t SerializedSize = 0;

        void serialize([[maybe_unused]] Buffer &out) const noexcept override
        {
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::RList));
        }

        std::string stringify() const noexcept override
        {
            return "[RList] remote shell requested list of connected clients";
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::RList)),
                                         " }");
        }
    };

    struct RListReply : public ILoggable
    {
        std::vector<::net::MACAddress> connectedClients;
        uint32_t nbClients{0};

        static constexpr const size_t SerializedSize = 4;

        RListReply() noexcept = default;

        RListReply(const Buffer &buff) : nbClients(Serializer::unserializeInt(buff, 0))
        {
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize + connectedClients.size() * 6);

            Serializer::serializeInt(out, static_cast<uint32_t>(connectedClients.size()));
            for (const auto &cur : connectedClients) {
                Serializer::serializeBytes(out, cur.raw());
            }
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::RListReply));
        }

        std::string stringify() const noexcept override
        {
            return "[RList] replying with a list of clients";
        }

    private:
        std::string __buildJSONArray() const noexcept
        {
            std::stringstream ss;

            ss << "[ ";
            std::for_each(connectedClients.begin(), connectedClients.end(), [this, &ss](const ::net::MACAddress &mac) {
                if (mac != connectedClients.front()) {
                    ss << ", ";
                }
                ss << "\"" << mac.toString() << "\"";
            });
            ss << " ]";
            return ss.str();
        }

    public:
        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::RListReply)),
                                         ", ",
                                         "\"clients\": ", __buildJSONArray(),
                                         " }");
        }
    };

    struct RStealthMode : public ILoggable
    {
        ::net::MACAddress addr;

        static constexpr const size_t SerializedSize = 6;

        RStealthMode() = default;

        RStealthMode(const Buffer &buff)
        {
            addr = Serializer::unserializeMACAddress(buff, 0, 6);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize);

            Serializer::serializeBytes(out, addr.raw());
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::RStealthMode));
        }

        std::string stringify() const noexcept override
        {
            return "[RStealthMode] remote shell requested switching to stealth mode";
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::RStealthMode)),
                                         " }");
        }
    };

    struct RActiveMode : public ILoggable
    {
        ::net::MACAddress addr;

        static constexpr const size_t SerializedSize = 6;

        RActiveMode() = default;

        RActiveMode(const Buffer &buff)
        {
            addr = Serializer::unserializeMACAddress(buff, 0, 6);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize);

            Serializer::serializeBytes(out, addr.raw());
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::RActiveMode));
        }

        std::string stringify() const noexcept override
        {
            return "[RActiveMode] remote shell requested switching to active mode";
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::RActiveMode)),
                                         " }");
        }
    };

    struct RScreenshot : public ILoggable
    {
        ::net::MACAddress addr;

        static constexpr const size_t SerializedSize = 6;

        RScreenshot() = default;

        RScreenshot(const Buffer &buff)
        {
            addr = Serializer::unserializeMACAddress(buff, 0, 6);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize);

            Serializer::serializeBytes(out, addr.raw());
        }

        void serializeTypeInfo(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + MessageHeaderSize);

            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::RScreenshot));
        }

        std::string stringify() const noexcept override
        {
            return "[RScreenshot] remote shell requested taking a screenshot";
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::RScreenshot)),
                                         " }");
        }
    };
}

#endif //SPIDER_PROTO_MESSAGES_HPP
