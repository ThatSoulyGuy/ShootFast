#ifndef SHOOTFAST_COMMON_NETWORK_HPP
#define SHOOTFAST_COMMON_NETWORK_HPP

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <enet/enet.h>

namespace ShootFast::Independent::Network
{
    using NetworkId = uint32_t;
    using SerializationBuffer = std::string;

    enum class PacketReliability : uint8_t
    {
        RELIABLE = 0,
        UNRELIABLE = 1
    };

    template <typename T>
    concept Serializable = requires(T a, const std::string& bufferIn, std::string& bufferOut)
    {
        { a.Serialize(bufferOut) } -> std::same_as<void>;
        { a.Deserialize(bufferIn) } -> std::same_as<void>;
    };

    class CommonNetwork final
    {

    public:

        CommonNetwork(const CommonNetwork&) = delete;
        CommonNetwork(CommonNetwork&&) = delete;
        CommonNetwork& operator=(const CommonNetwork&) = delete;
        CommonNetwork& operator=(CommonNetwork&&) = delete;

        template <Serializable... T>
        static void SerializeInto(SerializationBuffer& bufferOut, T&&... args)
        {
            std::tuple<T...> argsTuple = std::make_tuple(args...);

            bufferOut.clear();

            std::apply([&](const auto&... elements)
            {
                ([&bufferOut, &elements]
                {
                    std::string payload;

                    payload.clear();
                    elements.Serialize(payload);

                    if (payload.size() > static_cast<size_t>(std::numeric_limits<uint32_t>::max()))
                        throw std::runtime_error("Serialize error: payload too large for u32 length prefix.");

                    WriteU32LE(bufferOut, static_cast<uint32_t>(payload.size()));

                    bufferOut.append(payload);
                }(), ...);
            }, argsTuple);
        }

        template <Serializable... T>
        static void DeserializeInto(const SerializationBuffer& bufferIn, std::tuple<T...>& tupleOut)
        {
            size_t offset = 0;

            std::apply([&](auto&... elements)
            {
                ([&bufferIn, &offset, &elements]
                {
                    const uint32_t length = ReadU32LE(bufferIn, offset);

                    if (offset + static_cast<size_t>(length) > bufferIn.size())
                        throw std::runtime_error("Deserialize error: truncated payload for element.");

                    const std::string slice = bufferIn.substr(offset, length);

                    elements.Deserialize(slice);

                    offset += static_cast<size_t>(length);
                }(), ...);
            }, tupleOut);

            if (offset != bufferIn.size())
                throw std::runtime_error("Deserialize warning: extra bytes remain after tuple deserialization.");
        }

        static void BufferToPacketFlags(const bool reliable, enet_uint32& flags)
        {
            flags = reliable ? ENET_PACKET_FLAG_RELIABLE : 0u;
        }

    private:

        CommonNetwork() = default;

        static void WriteU32LE(std::string& bufferOut, uint32_t value)
        {
            char raw[4];

            raw[0] = static_cast<char>(value & 0xFFu);
            raw[1] = static_cast<char>(value >> 8 & 0xFFu);
            raw[2] = static_cast<char>(value >> 16 & 0xFFu);
            raw[3] = static_cast<char>(value >> 24 & 0xFFu);

            bufferOut.append(raw, 4);
        }

        static uint32_t ReadU32LE(const std::string& bufferIn, size_t& offset)
        {
            if (offset + 4 > bufferIn.size())
                throw std::runtime_error("Deserialize error: not enough bytes for length prefix (u32).");

            const auto p = reinterpret_cast<const unsigned char*>(bufferIn.data() + offset);

            const uint32_t value = static_cast<uint32_t>(p[0]) | static_cast<uint32_t>(p[1]) << 8 | static_cast<uint32_t>(p[2]) << 16 | static_cast<uint32_t>(p[3]) << 24;

            offset += 4;

            return value;
        }

    };
}

#endif