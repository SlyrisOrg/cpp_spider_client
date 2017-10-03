//
// Created by doom on 03/10/17.
//

#ifndef SPIDER_SERVER_ISERIALIZABLE_HPP
#define SPIDER_SERVER_ISERIALIZABLE_HPP

#include <vector>
#include <exception>

namespace spi
{
    using Byte = uint8_t;
    using Buffer = std::vector<Byte>;

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
        virtual Buffer serialize() const noexcept = 0;
    };

    class Serializer
    {
    public:
        template <typename T>
        static void serializeRaw(Buffer &v, T n) noexcept
        {
            auto oldSize = v.size();

            v.resize(oldSize + sizeof(T));
            std::memcpy(v.data() + oldSize, &n, sizeof(T));

            if (utils::endian() == utils::Endian::Big) {
                std::reverse(v.begin() + oldSize, v.end());
            }
        }

        static void serializeTimestamp(Buffer &v, const std::chrono::steady_clock::time_point &tp) noexcept
        {
            auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(tp);
            uint64_t ts = static_cast<uint64_t>(ms.time_since_epoch().count());

            serializeRaw(v, ts);
        }

        static always_inline void serializeInt(Buffer &v, uint32_t n) noexcept
        {
            serializeRaw(v, n);
        }

        static always_inline void serializeShort(Buffer &v, uint16_t n) noexcept
        {
            serializeRaw(v, n);
        }

        static void serializeBytes(Buffer &v, const std::vector<Byte> &buff) noexcept
        {
            v.insert(v.end(), buff.begin(), buff.end());
        }

        static void serializeBuff(Buffer &v, const std::vector<Byte> &buff) noexcept
        {
            serializeInt(v, static_cast<uint32_t>(buff.size()));
            serializeBytes(v, buff);
        }

        template <typename T>
        static T unserializeRaw(const Buffer &v, size_t startPos)
        {
            if (unlikely(v.size() - startPos < sizeof(T))) {
                throw UnserializationError();
            }

            T ret;
            Buffer sub;

            sub.insert(sub.begin(), v.begin() + startPos, v.begin() + startPos + sizeof(T));
            if (utils::endian() == utils::Endian::Big) {
                std::reverse(sub.begin(), sub.end());
            }
            std::memcpy(&ret, sub.data(), sizeof(T));
            return ret;
        }

        static always_inline uint32_t unserializeInt(const Buffer &v, size_t startPos)
        {
            return unserializeRaw<uint32_t>(v, startPos);
        }

        static always_inline uint16_t unserializeShort(const Buffer &v, size_t startPos)
        {
            return unserializeRaw<uint16_t>(v, startPos);
        }

        static std::chrono::steady_clock::time_point unserializeTimestamp(const Buffer &v, size_t startPos)
        {
            uint64_t conv = unserializeRaw<uint64_t>(v, startPos);

            std::chrono::milliseconds dur(conv);
            return std::chrono::steady_clock::time_point(dur);
        }

        static std::vector<Byte> unserializeBytes(const Buffer &v, size_t startPos, size_t size)
        {
            if (unlikely(v.size() - startPos < size)) {
                throw UnserializationError();
            }

            std::vector<Byte> ret;

            ret.insert(ret.begin(), v.begin() + startPos, v.begin() + startPos + size);
            return ret;
        }

        static std::vector<Byte> unserializeBuff(const Buffer &v, size_t startPos)
        {
            unsigned int size = unserializeInt(v, startPos);

            return unserializeBytes(v, startPos + sizeof(uint32_t), size);
        }
    };
}

#endif //SPIDER_SERVER_ISERIALIZABLE_HPP
