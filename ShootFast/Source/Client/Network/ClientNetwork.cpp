#include "Client/Network/ClientNetwork.hpp"

using namespace ShootFast::Independent::Network;

namespace ShootFast::Client::Network
{
    int& ClientNetwork::EnetRefCount()
    {
        static int count = 0;
        return count;
    }

    void ClientNetwork::InitializeEnet()
    {
        if (EnetRefCount() == 0)
        {
            if (enet_initialize() != 0)
                throw std::runtime_error("Failed to initialize ENet");
        }

        EnetRefCount() += 1;
    }

    void ClientNetwork::DeinitializeEnet()
    {
        EnetRefCount() -= 1;

        if (EnetRefCount() == 0)
            enet_deinitialize();
    }

    ENetPacket* ClientNetwork::MakePacket(const SerializationBuffer& buffer, const PacketReliability reliability)
    {
        const enet_uint32 flags = reliability == PacketReliability::RELIABLE ? ENET_PACKET_FLAG_RELIABLE : 0u;

        return enet_packet_create(buffer.data(), buffer.size(), flags);
    }

    SerializationBuffer ClientNetwork::BufferFromPacket(const ENetPacket* packet)
    {
        SerializationBuffer buffer;

        buffer.assign(reinterpret_cast<const char*>(packet->data), packet->dataLength);

        return buffer;
    }

    void ClientNetwork::Connect(const std::string& hostname, const uint16_t port)
    {
        InitializeEnet();

        host = enet_host_create(nullptr, 1, 2, 0, 0);

        if (host == nullptr)
            throw std::runtime_error("Failed to create ENet client host");

        ENetAddress address{};

        if (enet_address_set_host_ip(&address, hostname.c_str()) != 0)
        {
            enet_host_destroy(host);
            host = nullptr;

            throw std::runtime_error("ENet: failed to resolve host (IPv4): " + hostname);
        }

        address.port = port;

        peer = enet_host_connect(host, &address, 2, 0);

        if (peer == nullptr)
        {
            enet_host_destroy(host); host = nullptr;
            throw std::runtime_error("ENet: no available peers to connect");
        }

        constexpr int totalTimeoutMs = 4000;

        int waited = 0;

        ENetEvent event{};

        bool connected = false;

        while (waited < totalTimeoutMs)
        {
            constexpr int sliceMs = 100;

            while (enet_host_service(host, &event, sliceMs) > 0)
            {
                if (event.type == ENET_EVENT_TYPE_CONNECT)
                {
                    connected = true;

                    for (const auto& function : OnConnected)
                    {
                        if (function)
                            function();
                    }

                    break;
                }

                if (event.type == ENET_EVENT_TYPE_RECEIVE)
                    enet_packet_destroy(event.packet);
                else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
                {
                    enet_peer_reset(peer);
                    peer = nullptr;

                    enet_host_destroy(host);
                    host = nullptr;

                    throw std::runtime_error("ENet: server refused connection");
                }
            }

            if (connected)
                break;

            waited += sliceMs;
        }

        if (!connected)
        {
            enet_peer_reset(peer);
            peer = nullptr;

            enet_host_destroy(host);
            host = nullptr;

            throw std::runtime_error("ENet connection attempt failed or timed out (host=" + hostname + ", port=" + std::to_string(port) + ")");
        }
    }

    void ClientNetwork::Disconnect()
    {
        if (peer != nullptr)
        {
            enet_peer_disconnect(peer, 0);

            ENetEvent event;

            while (enet_host_service(host, &event, 500) > 0)
            {
                if (event.type == ENET_EVENT_TYPE_RECEIVE)
                    enet_packet_destroy(event.packet);
                else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
                    break;
            }

            enet_peer_reset(peer);

            peer = nullptr;
        }

        if (host != nullptr)
        {
            enet_host_destroy(host);

            host = nullptr;
        }
    }

    void ClientNetwork::Poll(const int timeoutMilliseconds)
    {
        if (host == nullptr)
            return;

        ENetEvent event;

        while (enet_host_service(host, &event, timeoutMilliseconds) > 0)
        {
            if (event.type == ENET_EVENT_TYPE_RECEIVE)
            {
                SerializationBuffer payload = BufferFromPacket(event.packet);

                for (const auto& function : OnPacketReceived)
                {
                    if (function)
                        function(payload);
                }

                enet_packet_destroy(event.packet);
            }
            else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
            {
                for (const auto& function : OnDisconnected)
                {
                    if (function)
                        function();
                }

                peer = nullptr;
            }
        }

        enet_host_flush(host);
    }

    void ClientNetwork::Send(const SerializationBuffer& buffer, const PacketReliability reliability, const uint8_t channel) const
    {
        if (peer == nullptr)
            return;

        enet_peer_send(peer, channel, MakePacket(buffer, reliability));
    }

    ClientNetwork& ClientNetwork::GetInstance()
    {
        static ClientNetwork instance;

        return instance;
    }

}
