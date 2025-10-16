#include "Server/ServerApplication.hpp"
#include <iostream>
#include <print>
#include <ranges>
#include "Independent/Math/Transform.hpp"
#include "Independent/Math/TransformSystem.hpp"
#include "Independent/Network/Packet.hpp"
#include "Server/Network/ServerNetwork.hpp"

using namespace ShootFast::Independent::ECS;
using namespace ShootFast::Independent::Math;
using namespace ShootFast::Independent::Network;
using namespace ShootFast::Server::Network;

namespace ShootFast::Server
{
    static constexpr int CONNECTION_PORT = 7777;
    static constexpr int MAX_CLIENTS = 256;

    ServerApplication::~ServerApplication()
    {
        ServerNetwork::GetInstance().Stop();
    }

    void ServerApplication::Preinitialize()
    {
        ServerNetwork::GetInstance().Start(CONNECTION_PORT, MAX_CLIENTS);

        ServerNetwork::GetInstance().OnClientConnected.emplace_back([this](NetworkId playerId)
        {
            std::print(std::cout, "Client with id '{0}' has connected to the server.\n", playerId);

            for (const auto& [otherId, otherGO] : playerByNetId)
            {
                SerializationBuffer buffer;

                S2C_Spawn spawn{};

                spawn.handle = otherId;
                spawn.position = world.Get<Transform>(otherGO).position;
                spawn.Serialize(buffer);

                ServerNetwork::GetInstance().SendTo(playerId, buffer, MessageType::S2C_Spawn, PacketReliability::RELIABLE, 0);
            }

            const GameObject gameObject = world.CreateGameObject();

            world.Add<Transform>(gameObject, Transform
            {
                .position = {0,0,0},
                .rotation = glm::quat{1,0,0,0},
                .scale = {1,1,1}
            });

            playerByNetId[playerId] = gameObject;

            {
                SerializationBuffer buffer;

                S2C_AssignSelf assign{};

                assign.handle = playerId;
                assign.position = world.Get<Transform>(gameObject).position;
                assign.Serialize(buffer);

                ServerNetwork::GetInstance().SendTo(playerId, buffer, MessageType::S2C_AssignSelf, PacketReliability::RELIABLE, 0);
            }

            {
                SerializationBuffer buffer;

                S2C_Spawn spawn{};

                spawn.handle = playerId;
                spawn.position = world.Get<Transform>(gameObject).position;
                spawn.Serialize(buffer);

                ServerNetwork::GetInstance().BroadcastExcluding(buffer, MessageType::S2C_Spawn, PacketReliability::RELIABLE, playerId, 0);
            }
        });

        ServerNetwork::GetInstance().OnClientTimeout.emplace_back([](NetworkId playerId)
        {
            std::print(std::cout, "Client with id '{0}' has timed out!\n", playerId);
        });

        ServerNetwork::GetInstance().OnClientDisconnected.emplace_back([this](NetworkId playerId)
        {
            std::print(std::cout, "Client with id '{0}' has disconnected from the server.\n", playerId);

            if (const auto iterator = playerByNetId.find(playerId); iterator != playerByNetId.end())
            {
                {
                    SerializationBuffer buffer;

                    S2C_Despawn spawn{};

                    spawn.handle = playerId;

                    spawn.Serialize(buffer);

                    ServerNetwork::GetInstance().BroadcastExcluding(buffer, MessageType::S2C_Spawn, PacketReliability::RELIABLE, playerId, 0);
                }

                world.DestroyGameObject(iterator->second);

                playerByNetId.erase(iterator);
            }
        });

        ServerNetwork::GetInstance().OnPacketReceived.emplace_back([this](const NetworkId from, const MessageType type, const SerializationBuffer& payload)
        {
            if (type == MessageType::C2S_GameObjectPose)
            {
                C2S_GameObjectPose message;

                message.Deserialize(payload);

                const auto iterator = playerByNetId.find(from);

                if (iterator == playerByNetId.end())
                    return;

                auto& transform = world.Get<Transform>(iterator->second);

                transform.position = message.position;
                transform.rotation = glm::quat(glm::vec3(message.rotation.x, message.rotation.y, 0.0f));

                const S2C_GameObjectPose out
                {
                    .handle = message.handle,
                    .position = message.position,
                    .rotation = message.rotation
                };

                SerializationBuffer buffer;

                out.Serialize(buffer);

                ServerNetwork::GetInstance().BroadcastExcluding(buffer, MessageType::S2C_GameObjectPose,  PacketReliability::UNRELIABLE, from, 0);
            }
        });
    }

    void ServerApplication::Initialize()
    {

    }

    bool ServerApplication::IsRunning() const
    {
        return ServerNetwork::GetInstance().IsRunning();
    }

    void ServerApplication::Update()
    {
        ServerNetwork::GetInstance().Poll(1);

        TransformSystem::Run(world);
    }

    ServerApplication& ServerApplication::GetInstance()
    {
        static ServerApplication instance;

        return instance;
    }
}

int main(int, char**)
{
    ShootFast::Server::ServerApplication::GetInstance().Preinitialize();
    ShootFast::Server::ServerApplication::GetInstance().Initialize();

    while (ShootFast::Server::ServerApplication::GetInstance().IsRunning())
        ShootFast::Server::ServerApplication::GetInstance().Update();

    return 0;
}