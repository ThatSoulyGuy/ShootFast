#include "Server/ServerApplication.hpp"
#include <iostream>
#include <print>
#include "Server/Network/ServerNetwork.hpp"

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

        ServerNetwork::GetInstance().OnClientConnected.emplace_back([](Independent::Network::NetworkId playerId)
        {
            std::print(std::cout, "Client with id '{0}' has connected to the server.\n", playerId);
        });

        ServerNetwork::GetInstance().OnClientTimeout.emplace_back([](Independent::Network::NetworkId playerId)
        {
            std::print(std::cout, "Client with id '{0}' has timed out!\n", playerId);
        });

        ServerNetwork::GetInstance().OnClientDisconnected.emplace_back([](Independent::Network::NetworkId playerId)
        {
            std::print(std::cout, "Client with id '{0}' has disconnected from the server.\n", playerId);
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