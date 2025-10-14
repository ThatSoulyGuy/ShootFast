#include "Server/Network/ServerNetwork.hpp"

#include <iostream>
#include <optional>
#include <ranges>
#include <stdexcept>

#ifdef ENET_FEATURE_IPV6
#error "ENET_FEATURE_IPV6 is defined – IPv6 is still enabled in ENet build!"
#endif

using namespace ShootFast::Independent::Network;

namespace ShootFast::Server::Network
{
    int& ServerNetwork::EnetRefCount()
    {
        static int count = 0;

        return count;
    }

    void ServerNetwork::InitializeEnet()
    {
        if (EnetRefCount() == 0)
        {
            if (enet_initialize() != 0)
                throw std::runtime_error("Failed to initialize ENet");
        }

        EnetRefCount() += 1;
    }

    void ServerNetwork::DeinitializeEnet()
    {
        EnetRefCount() -= 1;

        if (EnetRefCount() == 0)
            enet_deinitialize();
    }

    ENetPacket* ServerNetwork::MakePacket(const SerializationBuffer& buffer, const PacketReliability reliability)
    {
        const enet_uint32 flags = reliability == PacketReliability::RELIABLE ? ENET_PACKET_FLAG_RELIABLE : 0u;

        return enet_packet_create(buffer.data(), buffer.size(), flags);
    }

    SerializationBuffer ServerNetwork::BufferFromPacket(const ENetPacket* packet)
    {
        SerializationBuffer buffer;

        buffer.assign(reinterpret_cast<const char*>(packet->data), packet->dataLength);

        return buffer;
    }

    NetworkId ServerNetwork::AttachId(ENetPeer* peer)
    {
        const NetworkId id = nextId++;

        peer->data = reinterpret_cast<void*>(static_cast<uintptr_t>(id));
        clients.emplace(id, ClientEntry{ peer });

        return id;
    }

    std::optional<NetworkId> ServerNetwork::GetId(ENetPeer* peer)
    {
        if (peer == nullptr)
            return std::nullopt;

        const auto raw = reinterpret_cast<uintptr_t>(peer->data);

        if (raw == 0)
            return std::nullopt;

        return raw;
    }

    void ServerNetwork::DetachId(ENetPeer* peer)
    {
        if (const auto id = GetId(peer))
        {
            clients.erase(*id);
            peer->data = nullptr;
        }
    }

    void ServerNetwork::Start(const uint16_t port, const size_t maxClients)
    {
        InitializeEnet();

        ENetAddress address{};

        if (enet_address_set_host_ip(&address, "127.0.0.1") != 0)
            throw std::runtime_error("ENet: failed to set bind address (127.0.0.1).");

        address.port = port;

        constexpr int attempts = 1;
        ENetHost* created = nullptr;

        for (int i = 0; i < attempts; ++i)
        {
            constexpr int channelLimit = 2;

            address.port = static_cast<uint16_t>(port + i);
            created = enet_host_create(&address, maxClients, channelLimit, 0, 0);

            if (created != nullptr)
                break;
        }

        if (created == nullptr)
        {
#ifdef _WIN32
            throw std::runtime_error("Failed to create ENet server host: could not bind UDP port " + std::to_string(port) + ". Is another server running or firewall blocking?");
#else
            throw std::runtime_error("Failed to create ENet server host on port " + std::to_string(port));
#endif
        }

        host = created;
    }

    void ServerNetwork::Stop()
    {
        if (host != nullptr)
        {
            enet_host_destroy(host);

            host = nullptr;

            clients.clear();
        }
    }

    void ServerNetwork::Poll(const int timeoutMilliseconds)
    {
        if (host == nullptr)
            return;

        ENetEvent event;

        while (enet_host_service(host, &event, timeoutMilliseconds) > 0)
        {
            if (event.type == ENET_EVENT_TYPE_CONNECT)
            {
                const NetworkId id = AttachId(event.peer);

                for (const auto& function : OnClientConnected)
                {
                    if (function)
                        function(id);
                }
            }
            else if (event.type == ENET_EVENT_TYPE_RECEIVE)
            {
                if (auto maybeId = GetId(event.peer))
                {
                    SerializationBuffer payload = BufferFromPacket(event.packet);

                    for (const auto& function : OnPacketReceived)
                    {
                        if (function)
                        {
                            try
                            {
                                function(*maybeId, CommonNetwork::PeekType(payload), SerializationBuffer(payload.begin() + 1, payload.end()));
                            }
                            catch (std::exception& exception)
                            {
                                std::cerr << "Server packet error: " << exception.what() << "\n";
                            }
                            catch (...)
                            {
                                std::cerr << "Unknown server packet error!\n";
                            }
                        }
                    }
                }

                enet_packet_destroy(event.packet);
            }
            else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
            {
                if (auto maybeId = GetId(event.peer))
                {
                    for (const auto& function : OnClientDisconnected)
                    {
                        if (function)
                            function(*maybeId);
                    }

                    DetachId(event.peer);
                }
            }
        }

        enet_host_flush(host);
    }

    void ServerNetwork::Broadcast(const SerializationBuffer& buffer, const MessageType type, const PacketReliability reliability, const uint8_t channel) const
    {
        if (host == nullptr)
            return;

        SerializationBuffer bufferOut;

        CommonNetwork::WriteU8(bufferOut, static_cast<std::uint8_t>(type));
        bufferOut.append(buffer);

        enet_host_broadcast(host, channel, MakePacket(bufferOut, reliability));
    }

    void ServerNetwork::BroadcastExcluding(const SerializationBuffer& buffer, const MessageType type, const PacketReliability reliability, const NetworkId excludee, const uint8_t channel)
    {
        if (host == nullptr)
            return;

        for (const auto& client: clients | std::views::keys)
        {
            if (client == excludee)
                continue;

            SendTo(client, buffer, type, reliability, channel);
        }
    }

    void ServerNetwork::SendTo(const NetworkId id, const SerializationBuffer& buffer, const MessageType type, const PacketReliability reliability, const uint8_t channel)
    {
        const auto iterator = clients.find(id);

        if (iterator == clients.end())
            return;

        SerializationBuffer bufferOut;

        CommonNetwork::WriteU8(bufferOut, static_cast<std::uint8_t>(type));
        bufferOut.append(buffer);

        enet_peer_send(iterator->second.peer, channel, MakePacket(bufferOut, reliability));
    }

    ServerNetwork& ServerNetwork::GetInstance()
    {
        static ServerNetwork instance;

        return instance;
    }
}