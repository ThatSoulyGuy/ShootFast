#include "Client/Core/GameStates.hpp"

#include <print>

#include "Client/ClientApplication.hpp"
#include "Client/Core/InputManager.hpp"
#include "Client/Core/Window.hpp"
#include "Client/Entity/PlayerFactory.hpp"
#include "Client/Entity/Systems/CameraFollowSystem.hpp"
#include "Client/Entity/Systems/CameraLookSystem.hpp"
#include "Client/Entity/Systems/CharacterMotorSystem.hpp"
#include "Client/Entity/Systems/PlayerInputSystem.hpp"
#include "Client/Network/ClientNetwork.hpp"
#include "Client/Render/DefaultRenderSystem.hpp"
#include "Independent/Math/TransformSystem.hpp"

using namespace ShootFast::Client::Network;
using namespace ShootFast::Client::Render;
using namespace ShootFast::Client::Entity::Systems;
using namespace ShootFast::Client::Entity;
using namespace ShootFast::Independent::Math;
using namespace ShootFast::Independent::Network;

namespace ShootFast::Client::Core::States
{
    void ConnectingState::Initialize(ClientApplication& application)
    {
        std::print("Entering connecting state.\n");

        ClientNetwork::GetInstance().Poll(1);
        InputManager::GetInstance().Update();
    }

    void ConnectingState::Update(ClientApplication& application, float deltaSeconds)
    {
        ClientNetwork::GetInstance().Poll(1);
        InputManager::GetInstance().Update();
    }

    void ConnectingState::Render(ClientApplication& application)
    {
        Window::Clear();
        Window::GetInstance().Present();
    }

    void GameplayState::Initialize(ClientApplication& application)
    {
        std::print("Entering gameplay state.\n");

        application.BuildTestResources();
        application.CreateTestEntity();

        auto [playerHandle, cameraHandle] = PlayerFactory::CreateLocalPlayer(application.renderContext, application.world, false, { 0.0f, 0.0f, 0.0f });

        application.renderContext.cameraHandle = cameraHandle;

        application.localPlayerHandle = playerHandle;

        application.world.Add<Replication>(playerHandle, Replication{ .id = kInvalidReplicationHandle, .ownedByLocal = true });

        InputManager::GetInstance().SetMouseMode(MouseMode::LOCKED);
    }

    void GameplayState::Update(ClientApplication& application, const float deltaSeconds)
    {
        ClientNetwork::GetInstance().Poll(1);

        PlayerInputSystem::Run(application.world);

        const CharacterMotorSystem motorSystem(deltaSeconds);
        motorSystem.Run(application.world);

        CameraFollowSystem::Run(application.world);
        application.cameraLookSystem.Run(application.world);
        TransformSystem::Run(application.world);

        application.transformSynchronizationSystem.Run(application.world, deltaSeconds);

        if (InputManager::GetInstance().GetKeyState(KeyCode::ESCAPE, KeyState::PRESSED))
            InputManager::GetInstance().SetMouseMode(!InputManager::GetInstance().GetMouseMode());

        InputManager::GetInstance().Update();

        application.frameIndex += 1;
    }

    void GameplayState::Render(ClientApplication& application)
    {
        Window::Clear();

        DefaultRenderSystem{ application.renderContext }.Run(application.world);

        Window::GetInstance().Present();
    }

    void DisconnectedState::Initialize(ClientApplication& application)
    {
        std::print("Entering disconnected state.\n");
    }

    void DisconnectedState::Update(ClientApplication& application, float deltaSeconds)
    {
        InputManager::GetInstance().Update();
    }

    void DisconnectedState::Render(ClientApplication& application)
    {
        Window::Clear();
        Window::GetInstance().Present();
    }
}
