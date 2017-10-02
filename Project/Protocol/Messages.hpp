//
// Created by doom on 28/09/17.
//

#ifndef SPIDER_SERVER_MESSAGES_HPP
#define SPIDER_SERVER_MESSAGES_HPP

#include <cstring>
#include <chrono>
#include <vector>
#include <algorithm>
#include <utils/Utils.hpp>
#include <utils/Endian.hpp>
#include <utils/MD5.hpp>
#include <Protocol/MessagesEnums.hpp>

namespace spi::proto
{
    using Byte = uint8_t;
    using BufferT = std::vector<Byte>;

    class UnserializationError : public std::exception
    {
    public:
        const char *what() const noexcept
        {
            return "Data could not be unserialized";
        }
    };

    struct ISerializable
    {
    public:
        virtual BufferT serialize() const noexcept = 0;
    };

    struct IStringifiable
    {
    public:
        virtual std::string toString() const noexcept = 0;
    };

    class Serializer
    {
    public:
        template <typename T>
        static void serializeRaw(BufferT &v, T n) noexcept
        {
            auto oldSize = v.size();

            v.resize(oldSize + sizeof(T));
            std::memcpy(v.data() + oldSize, &n, sizeof(T));

            if (utils::endian() == utils::Endian::Big) {
                std::reverse(v.begin() + oldSize, v.end());
            }
        }

        static void serializeTimestamp(BufferT &v, const std::chrono::steady_clock::time_point &tp) noexcept
        {
            auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(tp);
            uint64_t ts = static_cast<uint64_t>(ms.time_since_epoch().count());

            serializeRaw(v, ts);
        }

        static always_inline void serializeInt(BufferT &v, uint32_t n) noexcept
        {
            serializeRaw(v, n);
        }

        static always_inline void serializeShort(BufferT &v, uint16_t n) noexcept
        {
            serializeRaw(v, n);
        }

        static void serializeBytes(BufferT &v, const std::vector<Byte> &buff) noexcept
        {
            v.insert(v.end(), buff.begin(), buff.end());
        }

        static void serializeBuff(BufferT &v, const std::vector<Byte> &buff) noexcept
        {
            serializeInt(v, static_cast<uint32_t>(buff.size()));
            serializeBytes(v, buff);
        }

        template <typename T>
        static T unserializeRaw(const BufferT &v, size_t startPos)
        {
            if (unlikely(v.size() - startPos < sizeof(T))) {
                throw UnserializationError();
            }

            T ret;
            BufferT sub;

            sub.insert(sub.begin(), v.begin() + startPos, v.begin() + startPos + sizeof(T));
            if (utils::endian() == utils::Endian::Big) {
                std::reverse(sub.begin(), sub.end());
            }
            std::memcpy(&ret, sub.data(), sizeof(T));
            return ret;
        }

        static always_inline uint32_t unserializeInt(const BufferT &v, size_t startPos)
        {
            return unserializeRaw<uint32_t>(v, startPos);
        }

        static always_inline uint16_t unserializeShort(const BufferT &v, size_t startPos)
        {
            return unserializeRaw<uint16_t>(v, startPos);
        }

        static std::chrono::steady_clock::time_point unserializeTimestamp(const BufferT &v, size_t startPos)
        {
            uint64_t conv = unserializeRaw<uint64_t>(v, startPos);

            std::chrono::milliseconds dur(conv);
            return std::chrono::steady_clock::time_point(dur);
        }

        static std::vector<Byte> unserializeBytes(const BufferT &v, size_t startPos, size_t size)
        {
            if (unlikely(v.size() - startPos < size)) {
                throw UnserializationError();
            }

            std::vector<Byte> ret;

            ret.insert(ret.begin(), v.begin() + startPos, v.begin() + startPos + size);
            return ret;
        }

        static std::vector<Byte> unserializeBuff(const BufferT &v, size_t startPos)
        {
            unsigned int size = unserializeInt(v, startPos);

            return unserializeBytes(v, startPos + sizeof(uint32_t), size);
        }
    };

    struct ReplyCode : public ISerializable, public IStringifiable
    {
        uint32_t code;

        static constexpr const size_t SerializedSize = 4;

        ReplyCode() noexcept = default;

        ReplyCode(const BufferT &buff)
        {
            code = Serializer::unserializeInt(buff, 0);
        }

        BufferT serialize() const noexcept override
        {
            BufferT ret;

            ret.reserve(SerializedSize);

            Serializer::serializeInt(ret, code);
            return ret;
        }

        std::string toString() const noexcept override
        {
            return "[ReplyCode] " + std::to_string(code);
        }
    };

    struct Bye : public ISerializable, public IStringifiable
    {
        static constexpr const size_t SerializedSize = 0;

        BufferT serialize() const noexcept override
        {
            return BufferT();
        }

        std::string toString() const noexcept override
        {
            return "[Bye]";
        }
    };

    struct RawData : public ISerializable, public IStringifiable
    {
        std::vector<Byte> bytes;

        RawData() = default;

        RawData(const BufferT &buff)
        {
            bytes = Serializer::unserializeBuff(buff, 0);
        }

        BufferT serialize() const noexcept override
        {
            BufferT ret;

            ret.reserve(bytes.size() + 4);

            Serializer::serializeInt(ret, static_cast<uint32_t>(bytes.size()));
            ret.insert(ret.end(), bytes.begin(), bytes.end());
            return ret;
        }

        std::string toString() const noexcept override
        {
            return "[RawData] size: " + std::to_string(bytes.size());
        }
    };

    struct Hello : public ISerializable, public IStringifiable
    {
        std::string macAddress{"aaaaaa"};
        utils::MD5 md5;
        uint16_t version;
        uint16_t port;

        static constexpr const size_t SerializedSize = 26;

        Hello() = default;

        Hello(const BufferT &buff)
        {
            auto macAddrBytes = Serializer::unserializeBytes(buff, 0, 6);
            macAddress = std::string(macAddrBytes.begin(), macAddrBytes.end());
            version = Serializer::unserializeShort(buff, 6);

            auto md5Bytes = Serializer::unserializeBytes(buff, 8, 16);
            utils::MD5::RawMD5 raw;
            std::copy(md5Bytes.begin(), md5Bytes.end(), raw.begin());
            md5.setRaw(raw);
            port = Serializer::unserializeShort(buff, 24);
        }

        BufferT serialize() const noexcept override
        {
            BufferT ret;

            ret.reserve(SerializedSize);

            std::vector<Byte> macAddrBytes(macAddress.begin(), macAddress.end());
            Serializer::serializeBytes(ret, macAddrBytes);

            Serializer::serializeShort(ret, version);

            std::vector<Byte> bytes(md5.raw().begin(), md5.raw().end());
            Serializer::serializeBytes(ret, bytes);

            Serializer::serializeShort(ret, port);
            return ret;
        }

        std::string toString() const noexcept override
        {
            std::stringstream ss;

            ss << "[Hello] ";
            ss << "MAC: " << macAddress << ", ";
            ss << "version: " << version << ", ";
            ss << "MD5: " << md5.toString() << ", ";
            ss << "port: " << port;
            return ss.str();
        }
    };

    struct KeyEvent : public ISerializable, public IStringifiable
    {
        std::chrono::steady_clock::time_point timestamp;
        KeyCode code;
        KeyState state;

        static constexpr const size_t SerializedSize = 8 + 4 + 4;

        KeyEvent() = default;

        KeyEvent(const BufferT &buff)
        {
            timestamp = Serializer::unserializeTimestamp(buff, 0);
            code = static_cast<KeyCode::EnumType>(Serializer::unserializeInt(buff, 8));
            state = static_cast<KeyState::EnumType>(Serializer::unserializeInt(buff, 12));
        }

        BufferT serialize() const noexcept override
        {
            BufferT ret;

            ret.reserve(SerializedSize);

            Serializer::serializeTimestamp(ret, timestamp);
            Serializer::serializeInt(ret, static_cast<uint32_t>(code));
            Serializer::serializeInt(ret, static_cast<uint32_t>(state));

            return ret;
        }

        std::string toString() const noexcept override
        {
            std::stringstream ss;

            ss << "[KeyEvent] ";
            ss << "timestamp: " << timestamp.time_since_epoch().count() << ", ";
            ss << "code: " << code.toString() << ", ";
            ss << "state: " << state.toString();
            return ss.str();
        }
    };

    struct MouseClick : public ISerializable, public IStringifiable
    {
        std::chrono::steady_clock::time_point timestamp;
        uint32_t x;
        uint32_t y;
        KeyState state;
        MouseButton button;

        static constexpr const size_t SerializedSize = 8 + 4 + 4 + 4 + 4;

        MouseClick() = default;

        MouseClick(const BufferT &buff)
        {
            timestamp = Serializer::unserializeTimestamp(buff, 0);
            x = Serializer::unserializeInt(buff, 8);
            y = Serializer::unserializeInt(buff, 12);
            state = static_cast<KeyState::EnumType>(Serializer::unserializeInt(buff, 16));
            button = static_cast<MouseButton::EnumType>(Serializer::unserializeInt(buff, 20));
        }

        BufferT serialize() const noexcept override
        {
            BufferT ret;

            ret.reserve(SerializedSize);

            Serializer::serializeTimestamp(ret, timestamp);
            Serializer::serializeInt(ret, x);
            Serializer::serializeInt(ret, y);
            Serializer::serializeInt(ret, static_cast<uint32_t>(state));
            Serializer::serializeInt(ret, static_cast<uint32_t>(button));
            return ret;
        }

        std::string toString() const noexcept override
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
    };

    struct MouseMove : public ISerializable, public IStringifiable
    {
        std::chrono::steady_clock::time_point timestamp;
        uint32_t x;
        uint32_t y;

        static constexpr const size_t SerializedSize = 8 + 4 + 4;

        MouseMove() = default;

        MouseMove(const BufferT &buff)
        {
            timestamp = Serializer::unserializeTimestamp(buff, 0);
            x = Serializer::unserializeInt(buff, 8);
            y = Serializer::unserializeInt(buff, 12);
        }

        BufferT serialize() const noexcept override
        {
            std::vector<Byte> ret;

            ret.reserve(SerializedSize);

            Serializer::serializeTimestamp(ret, timestamp);
            Serializer::serializeInt(ret, x);
            Serializer::serializeInt(ret, y);
            return ret;
        }

        std::string toString() const noexcept override
        {
            std::stringstream ss;

            ss << "[MouseMove] ";
            ss << "timestamp: " << timestamp.time_since_epoch().count() << ", ";
            ss << "x: " << x << ", ";
            ss << "y: " << y << ", ";
            return ss.str();
        }
    };

    struct ImageData : public ISerializable, public IStringifiable
    {
        std::vector<Byte> bytes;

        ImageData(const BufferT &buff)
        {
            bytes = Serializer::unserializeBuff(buff, 0);
        }

        BufferT serialize() const noexcept override
        {
            BufferT ret;

            ret.reserve(bytes.size() + sizeof(uint32_t));

            Serializer::serializeBuff(ret, bytes);
            return ret;
        }

        std::string toString() const noexcept override
        {
            return "[ImageData] size: " + std::to_string(bytes.size());
        }
    };

    struct Screenshot : public ISerializable, public IStringifiable
    {
        static constexpr const size_t SerializedSize = 0;

        BufferT serialize() const noexcept override
        {
            return BufferT();
        }

        std::string toString() const noexcept override
        {
            return "[Screenshot] requested screenshot";
        }
    };

    struct StealthMode : public ISerializable, public IStringifiable
    {
        static constexpr const size_t SerializedSize = 0;

        BufferT serialize() const noexcept override
        {
            return BufferT();
        }

        std::string toString() const noexcept override
        {
            return "[StealthMode] requested switch to stealth mode";
        }
    };

    struct ActiveMode : public ISerializable, public IStringifiable
    {
        static constexpr const size_t SerializedSize = 0;

        BufferT serialize() const noexcept override
        {
            return BufferT();
        }

        std::string toString() const noexcept override
        {
            return "[ActiveMode] requested switch to active mode";
        }
    };
}

#endif //SPIDER_SERVER_MESSAGES_HPP
