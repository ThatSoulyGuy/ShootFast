#include "Client/Core/GameStates.hpp"

#include <print>

#include "Client/ClientApplication.hpp"

namespace ShootFast::Client::Core::States
{
    void ConnectingState::Initialize(ClientApplication& application)
    {
        std::print("Entering connecting state.\n");
        application.UpdateConnecting(0.0f);
    }

    void ConnectingState::Update(ClientApplication& application, float deltaSeconds)
    {
        application.UpdateConnecting(deltaSeconds);
    }

    void ConnectingState::Render(ClientApplication& application)
    {
        application.RenderConnecting();
    }

    void GameplayState::Initialize(ClientApplication& application)
    {
        std::print("Entering gameplay state.\n");
    }

    void GameplayState::Update(ClientApplication& application, float deltaSeconds)
    {
        application.UpdateGameplay(deltaSeconds);
    }

    void GameplayState::Render(ClientApplication& application)
    {
        application.RenderGameplay();
    }

    void DisconnectedState::Initialize(ClientApplication& application)
    {
        std::print("Entering disconnected state.\n");
    }

    void DisconnectedState::Update(ClientApplication& application, float deltaSeconds)
    {
        application.UpdateDisconnected(deltaSeconds);
    }

    void DisconnectedState::Render(ClientApplication& application)
    {
        application.RenderDisconnected();
    }
}
