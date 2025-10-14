#include "Independent/Network/TransformSyncSystem.hpp"
#include "Client/Network/ClientNetwork.hpp"
#include "Independent/Core/Settings.hpp"
#include "Independent/ECS/World.hpp"
#include "Independent/Math/Transform.hpp"
#include "Independent/Network/Packet.hpp"
#include "Server/Network/ServerNetwork.hpp"

namespace ShootFast::Independent::Network
{
    void TransformSyncSystem::Run(ECS::World& world, const float deltaSeconds)
    {
        accumulator += deltaSeconds;

        if (accumulator < intervalSeconds)
            return;

        accumulator = 0.f;

        for (const auto& gameObject : syncObjectList)
        {
            if (!world.Has<Math::Transform>(gameObject) || !world.Has<Replication>(gameObject))
                continue;

            const auto& transform = world.Get<Math::Transform>(gameObject);

            const glm::vec3 euler = glm::eulerAngles(transform.rotation);

#ifdef IS_SERVER_FLAG
            S2C_GameObjectPose message;
#else
            C2S_GameObjectPose message;
#endif

            message.handle = world.Get<Replication>(gameObject).id;
            message.position = transform.position;
            message.rotation = { euler.x, euler.y };

            SerializationBuffer buffer;

            message.Serialize(buffer);

            if constexpr (Core::Settings::IS_SERVER)
                Server::Network::ServerNetwork::GetInstance().Broadcast(buffer, MessageType::S2C_GameObjectPose, PacketReliability::UNRELIABLE, 0);
            else
                Client::Network::ClientNetwork::GetInstance().Send(buffer, MessageType::C2S_GameObjectPose, PacketReliability::UNRELIABLE, 0);
        }
    }
}
