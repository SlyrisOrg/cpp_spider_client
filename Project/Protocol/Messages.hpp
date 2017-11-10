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
    struct ReplyCode : public ILoggable
    {
        ReplyType code;

        static constexpr const size_t SerializedSize = 4;

        ReplyCode() noexcept = default;

        void unserialize(const Buffer &in) override
        {
            code = static_cast<ReplyType::EnumType>(Serializer::unserializeInt(in, TypeInfoSize + 0));
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize);

            serializeHeader(out);
            Serializer::serializeInt(out, static_cast<uint32_t>(code));
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize));
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

        void unserialize([[maybe_unused]] const Buffer &buffer) override
        {
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize);
            serializeHeader(out);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize));
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

        void unserialize(const Buffer &buff) override
        {
            bytes = Serializer::unserializeBuff(buff, TypeInfoSize + 0);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize + bytes.size());

            serializeHeader(out);
            Serializer::serializeBuff(out, bytes);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize + bytes.size()));
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

        void unserialize(const Buffer &buff) override
        {
            macAddress = Serializer::unserializeMACAddress(buff, TypeInfoSize + 0);

            version = Serializer::unserializeShort(buff, TypeInfoSize + 6);

            auto md5Bytes = Serializer::unserializeBytes(buff, TypeInfoSize + 8, 16);
            utils::MD5::RawMD5 raw;
            std::copy(md5Bytes.begin(), md5Bytes.end(), raw.begin());
            md5.setRaw(raw);

            port = Serializer::unserializeShort(buff, TypeInfoSize + 24);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize);

            serializeHeader(out);

            Serializer::serializeMACAddress(out, macAddress);
            Serializer::serializeShort(out, version);
            Serializer::serializeBytes(out, md5.raw());
            Serializer::serializeShort(out, port);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize));
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
        std::chrono::system_clock::time_point timestamp;
        KeyCode code;
        KeyState state;

        static constexpr const size_t SerializedSize = 8 + 4 + 4;

        KeyEvent() = default;

        void unserialize(const Buffer &buff) override
        {
            timestamp = Serializer::unserializeTimestamp(buff, TypeInfoSize + 0);
            code = static_cast<KeyCode::EnumType>(Serializer::unserializeInt(buff, TypeInfoSize + 8));
            state = static_cast<KeyState::EnumType>(Serializer::unserializeInt(buff, TypeInfoSize + 12));
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize);

            serializeHeader(out);

            Serializer::serializeTimestamp(out, timestamp);
            Serializer::serializeInt(out, static_cast<uint32_t>(code));
            Serializer::serializeInt(out, static_cast<uint32_t>(state));
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize));
            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::KeyEvent));
        }

        std::string stringify() const noexcept override
        {
            std::stringstream ss;

            ss << "[KeyEvent] ";
            auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(timestamp);
            auto epoch = ms.time_since_epoch();
            uint64_t ts = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count());
            ss << "timestamp: " << ts << ", ";
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
        std::chrono::system_clock::time_point timestamp;
        uint32_t x;
        uint32_t y;
        KeyState state;
        MouseButton button;

        static constexpr const size_t SerializedSize = 8 + 4 + 4 + 4 + 4;

        MouseClick() = default;

        void unserialize(const Buffer &buff) override
        {
            timestamp = Serializer::unserializeTimestamp(buff, TypeInfoSize + 0);
            x = Serializer::unserializeInt(buff, TypeInfoSize + 8);
            y = Serializer::unserializeInt(buff, TypeInfoSize + 12);
            state = static_cast<KeyState::EnumType>(Serializer::unserializeInt(buff, TypeInfoSize + 16));
            button = static_cast<MouseButton::EnumType>(Serializer::unserializeInt(buff, TypeInfoSize + 20));
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize);

            serializeHeader(out);

            Serializer::serializeTimestamp(out, timestamp);
            Serializer::serializeInt(out, x);
            Serializer::serializeInt(out, y);
            Serializer::serializeInt(out, static_cast<uint32_t>(state));
            Serializer::serializeInt(out, static_cast<uint32_t>(button));
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize));
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
        std::chrono::system_clock::time_point timestamp;
        uint32_t x;
        uint32_t y;

        static constexpr const size_t SerializedSize = 8 + 4 + 4;

        MouseMove() = default;

        void unserialize(const Buffer &buff) override
        {
            timestamp = Serializer::unserializeTimestamp(buff, TypeInfoSize + 0);
            x = Serializer::unserializeInt(buff, TypeInfoSize + 8);
            y = Serializer::unserializeInt(buff, TypeInfoSize + 12);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize);

            serializeHeader(out);

            Serializer::serializeTimestamp(out, timestamp);
            Serializer::serializeInt(out, x);
            Serializer::serializeInt(out, y);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + HeaderSize));
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

        void unserialize(const Buffer &buff) override
        {
            bytes = Serializer::unserializeBuff(buff, TypeInfoSize + 0);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize + bytes.size());

            serializeHeader(out);
            Serializer::serializeBuff(out, bytes);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize + bytes.size()));
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

        void unserialize([[maybe_unused]] const Buffer &buffer) override
        {
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize);

            serializeHeader(out);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize));
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

        void unserialize([[maybe_unused]] const Buffer &buffer) override
        {
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize);

            serializeHeader(out);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize));
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

        void unserialize([[maybe_unused]] const Buffer &buffer) override
        {
        }

        void serialize([[maybe_unused]] Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize);

            serializeHeader(out);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize));
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

        void unserialize([[maybe_unused]] const Buffer &buffer) override
        {
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize);

            serializeHeader(out);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize));
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
        std::vector<::net::MACAddress> clients;

        static constexpr const size_t SerializedSize = 4;

        RListReply() noexcept = default;

        void unserialize(const Buffer &buff) override
        {
            uint32_t nb = Serializer::unserializeInt(buff, TypeInfoSize + 0);
            for (uint32_t i = 0; i < nb; ++i) {
                clients.push_back(Serializer::unserializeMACAddress(buff, TypeInfoSize + 4 + 6 * i));
            }
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize + clients.size() * 6);

            serializeHeader(out);

            Serializer::serializeInt(out, static_cast<uint32_t>(clients.size()));
            for (const auto &cur : clients) {
                Serializer::serializeMACAddress(out, cur);
            }
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize + 6 * clients.size()));
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
            std::for_each(clients.begin(), clients.end(), [this, &ss](const ::net::MACAddress &mac) {
                if (mac != clients.front()) {
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

        void unserialize(const Buffer &buff) override
        {
            addr = Serializer::unserializeMACAddress(buff, TypeInfoSize + 0);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize);

            serializeHeader(out);
            Serializer::serializeMACAddress(out, addr);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize));
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

        void unserialize(const Buffer &buff) override
        {
            addr = Serializer::unserializeMACAddress(buff, TypeInfoSize + 0);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize);

            serializeHeader(out);
            Serializer::serializeMACAddress(out, addr);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize));
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

        void unserialize(const Buffer &buff) override
        {
            addr = Serializer::unserializeMACAddress(buff, TypeInfoSize + 0);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize);

            serializeHeader(out);
            Serializer::serializeMACAddress(out, addr);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize));
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

    struct WindowChanged : public ILoggable
    {
        std::string windowName;

        static constexpr const size_t SerializedSize = 4;

        WindowChanged() = default;

        void unserialize(const Buffer &buff) override
        {
            windowName = Serializer::unserializeString(buff, TypeInfoSize + 0);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize + windowName.size());

            serializeHeader(out);
            Serializer::serializeString(out, windowName);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize + windowName.size()));
            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::WindowChange));
        }

        std::string stringify() const noexcept override
        {
            std::stringstream ss;

            ss << "[WindowChange] ";
            ss << "cmd: " << windowName;
            return ss.str();
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::WindowChange)),
                                         ", ",
                                         "\"title\": ", JSON::quote(windowName),
                                         " }");
        }
    };

    struct RunShell : public ILoggable
    {
        std::string cmd;

        static constexpr const size_t SerializedSize = 4;

        RunShell() = default;

        void unserialize(const Buffer &buff) override
        {
            cmd = Serializer::unserializeString(buff, TypeInfoSize + 0);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + HeaderSize + SerializedSize + cmd.size());

            Buffer buff(cmd.begin(), cmd.end());

            out.reserve(SerializedSize + cmd.size());
            Serializer::serializeBuff(out, buff);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            Serializer::serializeInt(out, static_cast<uint32_t>(TypeInfoSize + SerializedSize + cmd.size()));
            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::RunShell));
        }

        std::string stringify() const noexcept override
        {
            std::stringstream ss;

            ss << "[RunShell] ";
            ss << "command: " << cmd;
            return ss.str();
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::RunShell)), ", ",
                                         "\"command\": ", JSON::quote(cmd),
                                         " }");
        }
    };

    struct RRunShell : public ILoggable
    {
        ::net::MACAddress target;
        std::string cmd;

        static constexpr const size_t SerializedSize = 6 + 4;

        RRunShell() = default;

        void unserialize(const Buffer &buff) override
        {
            target = Serializer::unserializeMACAddress(buff, TypeInfoSize + 0);
            cmd = Serializer::unserializeString(buff, TypeInfoSize + 6);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize + cmd.size());
            Serializer::serializeMACAddress(out, target);

            Buffer buff(cmd.begin(), cmd.end());
            Serializer::serializeBuff(out, buff);
        }

        void serializeHeader(Buffer &out) const noexcept
        {
            out.reserve(out.size() + HeaderSize);
            Serializer::serializeInt(out, static_cast<uint32_t>(MessageType::RRunShell));
        }

        std::string stringify() const noexcept override
        {
            std::stringstream ss;

            ss << "[RRunShell] ";
            ss << "target: " << target.toString();
            ss << "command: " << cmd;
            return ss.str();
        }

        std::string JSONify() const noexcept override
        {
            return utils::unpackToString("{ ",
                                         "\"type\": ", JSON::quote(MessageType::toString(MessageType::RRunShell)), ", ",
                                         "\"target\": ", JSON::quote(target.toString()), ", ",
                                         "\"command\": ", JSON::quote(cmd),
                                         " }");
        }
    };
}

#endif //SPIDER_PROTO_MESSAGES_HPP
