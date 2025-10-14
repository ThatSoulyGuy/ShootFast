#ifndef SHOOTFAST_SERVER_NETWORK_HPP
#define SHOOTFAST_SERVER_NETWORK_HPP

#include <functional>
#include <optional>

#include "Independent/Network/CommonNetwork.hpp"

namespace ShootFast::Server::Network
{
    class ServerNetwork final
    {

    public:

        ServerNetwork(const ServerNetwork&) = delete;
        ServerNetwork(ServerNetwork&&) = delete;
        ServerNetwork& operator=(const ServerNetwork&) = delete;
        ServerNetwork& operator=(ServerNetwork&&) = delete;

        ~ServerNetwork()
        {
            Stop();
            DeinitializeEnet();
        }

        void Start(uint16_t port, size_t maxClients);
        void Stop();
        void Poll(int timeoutMilliseconds);

        bool IsRunning() const
        {
            return host != nullptr;
        }

        void Broadcast(const Independent::Network::SerializationBuffer& buffer, Independent::Network::MessageType type, Independent::Network::PacketReliability reliability, uint8_t channel = 0) const;
        void BroadcastExcluding(const Independent::Network::SerializationBuffer& buffer, Independent::Network::MessageType type, Independent::Network::PacketReliability reliability, Independent::Network::NetworkId excludee, uint8_t channel = 0);
        void SendTo(Independent::Network::NetworkId id, const Independent::Network::SerializationBuffer& buffer, Independent::Network::MessageType type, Independent::Network::PacketReliability reliability, uint8_t channel = 0);

        std::vector<std::function<void(Independent::Network::NetworkId)>> OnClientConnected;
        std::vector<std::function<void(Independent::Network::NetworkId)>> OnClientDisconnected;
        std::vector<std::function<void(Independent::Network::NetworkId, Independent::Network::MessageType, const Independent::Network::SerializationBuffer&)>> OnPacketReceived;
        std::vector<std::function<void(Independent::Network::NetworkId)>> OnClientTimeout;

        static ServerNetwork& GetInstance();

    private:

        ServerNetwork() = default;

        struct ClientEntry
        {
            ENetPeer* peer;
        };

        ENetHost* host = nullptr;

        std::unordered_map<Independent::Network::NetworkId, ClientEntry> clients;
        Independent::Network::NetworkId nextId = 1;

        static void InitializeEnet();
        static void DeinitializeEnet();
        static int& EnetRefCount();

        static ENetPacket* MakePacket(const Independent::Network::SerializationBuffer& buffer, Independent::Network::PacketReliability reliability);
        static Independent::Network::SerializationBuffer BufferFromPacket(const ENetPacket* packet);

        Independent::Network::NetworkId AttachId(ENetPeer* peer);

        static std::optional<Independent::Network::NetworkId> GetId(ENetPeer* peer);
        void DetachId(ENetPeer* peer);
    };
}

#endif