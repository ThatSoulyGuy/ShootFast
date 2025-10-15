#ifndef SHOOTFAST_PACKET_HPP
#define SHOOTFAST_PACKET_HPP

#include <cstdint>
#include <stdexcept>
#include <string>
#include "Independent/Network/CommonNetwork.hpp"

namespace ShootFast::Independent::Network
{
    using SerializationBuffer = std::string;
    using NetworkId = uint32_t;

    struct C2S_GameObjectPose final
    {
        ReplicationHandle handle{0};

        glm::vec3 position;
        glm::vec2 rotation;

        void Serialize(SerializationBuffer& out) const
        {
            out.clear();

            CommonNetwork::WriteU32(out, handle);

            CommonNetwork::WriteF32(out, position.x);
            CommonNetwork::WriteF32(out, position.y);
            CommonNetwork::WriteF32(out, position.z);

            CommonNetwork::WriteF32(out, rotation.x);
            CommonNetwork::WriteF32(out, rotation.y);
        }

        void Deserialize(const SerializationBuffer& in)
        {
            size_t offset = 0;

            handle = CommonNetwork::ReadU32(in, offset);

            position.x = CommonNetwork::ReadF32(in, offset);
            position.y = CommonNetwork::ReadF32(in, offset);
            position.z = CommonNetwork::ReadF32(in, offset);

            rotation.x = CommonNetwork::ReadF32(in, offset);
            rotation.y = CommonNetwork::ReadF32(in, offset);

            if (offset != in.size())
                throw std::runtime_error("C2S_PlayerPose: trailing bytes");
        }
    };

    struct S2C_GameObjectPose final
    {
        ReplicationHandle handle{0};

        glm::vec3 position;
        glm::vec2 rotation;

        void Serialize(SerializationBuffer& out) const
        {
            out.clear();

            CommonNetwork::WriteU32(out, handle);

            CommonNetwork::WriteF32(out, position.x);
            CommonNetwork::WriteF32(out, position.y);
            CommonNetwork::WriteF32(out, position.z);

            CommonNetwork::WriteF32(out, rotation.x);
            CommonNetwork::WriteF32(out, rotation.y);
        }

        void Deserialize(const SerializationBuffer& in)
        {
            size_t offset = 0;

            handle = CommonNetwork::ReadU32(in, offset);

            position.x = CommonNetwork::ReadF32(in, offset);
            position.y = CommonNetwork::ReadF32(in, offset);
            position.z = CommonNetwork::ReadF32(in, offset);

            rotation.x = CommonNetwork::ReadF32(in, offset);
            rotation.y = CommonNetwork::ReadF32(in, offset);

            if (offset != in.size())
                throw std::runtime_error("S2C_PlayerPose: trailing bytes");
        }
    };

    struct S2C_AssignSelf final
    {
        ReplicationHandle handle;
        glm::vec3 position;

        void Serialize(SerializationBuffer& out) const
        {
            out.clear();

            CommonNetwork::WriteU32(out, handle);

            CommonNetwork::WriteF32(out, position.x);
            CommonNetwork::WriteF32(out, position.y);
            CommonNetwork::WriteF32(out, position.z);
        }

        void Deserialize(const SerializationBuffer& in)
        {
            size_t offset = 0;

            handle = CommonNetwork::ReadU32(in, offset);

            position.x = CommonNetwork::ReadF32(in, offset);
            position.y = CommonNetwork::ReadF32(in, offset);
            position.z = CommonNetwork::ReadF32(in, offset);
        }
    };

    struct S2C_Spawn final
    {
        ReplicationHandle handle;
        glm::vec3 position;

        void Serialize(SerializationBuffer& out) const
        {
            out.clear();

            CommonNetwork::WriteU32(out, handle);

            CommonNetwork::WriteF32(out, position.x);
            CommonNetwork::WriteF32(out, position.y);
            CommonNetwork::WriteF32(out, position.z);
        }

        void Deserialize(const SerializationBuffer& in)
        {
            size_t offset = 0;

            handle = CommonNetwork::ReadU32(in, offset);

            position.x = CommonNetwork::ReadF32(in, offset);
            position.y = CommonNetwork::ReadF32(in, offset);
            position.z = CommonNetwork::ReadF32(in, offset);
        }
    };

    struct S2C_Despawn final
    {
        ReplicationHandle handle;

        void Serialize(SerializationBuffer& out) const
        {
            out.clear();

            CommonNetwork::WriteU32(out, handle);
        }

        void Deserialize(const SerializationBuffer& in)
        {
            size_t offset = 0;

            handle = CommonNetwork::ReadU32(in, offset);
        }
    };
}

#endif