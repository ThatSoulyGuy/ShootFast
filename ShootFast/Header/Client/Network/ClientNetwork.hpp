#ifndef SHOOTFAST_CLIENT_NETWORK_HPP
#define SHOOTFAST_CLIENT_NETWORK_HPP

#include <functional>
#include <string>
#include <enet/enet.h>
#include "Independent/Network/CommonNetwork.hpp"

namespace ShootFast::Client::Network
{
    class ClientNetwork final
    {

    public:

        ClientNetwork(const ClientNetwork&) = delete;
        ClientNetwork(ClientNetwork&&) = delete;
        ClientNetwork& operator=(const ClientNetwork&) = delete;
        ClientNetwork& operator=(ClientNetwork&&) = delete;

        ~ClientNetwork()
        {
            Disconnect();
            DeinitializeEnet();
        }

        void Connect(const std::string& hostname, uint16_t port);
        void Disconnect();

        [[nodiscard]]
        bool IsConnected() const
        {
            return peer != nullptr && peer->state == ENET_PEER_STATE_CONNECTED;
        }

        void Poll(int timeoutMilliseconds);

        void Send(const Independent::Network::SerializationBuffer& buffer, Independent::Network::MessageType type, Independent::Network::PacketReliability reliability, uint8_t channel = 0) const;

        static ClientNetwork& GetInstance();

        std::vector<std::function<void()>> OnConnected;
        std::vector<std::function<void()>> OnDisconnected;
        std::vector<std::function<void(Independent::Network::MessageType, const Independent::Network::SerializationBuffer&)>> OnPacketReceived;
        std::vector<std::function<void()>> OnTimeout;

    private:

        ClientNetwork() = default;

        ENetHost* host = nullptr;
        ENetPeer* peer = nullptr;

        static void InitializeEnet();
        static void DeinitializeEnet();
        static int& EnetRefCount();

        static ENetPacket* MakePacket(const Independent::Network::SerializationBuffer& buffer, Independent::Network::PacketReliability reliability);
        static Independent::Network::SerializationBuffer BufferFromPacket(const ENetPacket* packet);
        
    };
}

#endif