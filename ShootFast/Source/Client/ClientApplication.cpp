#include "Client/ClientApplication.hpp"

#include <iostream>
#include <print>

#include "Client/Core/GameStates.hpp"
#include "Client/Core/InputManager.hpp"
#include "Client/Core/Window.hpp"
#include "Client/Entity/PlayerFactory.hpp"
#include "Client/Entity/Systems/CameraFollowSystem.hpp"
#include "Client/Entity/Systems/CharacterMotorSystem.hpp"
#include "Client/Entity/Systems/PlayerInputSystem.hpp"
#include "Client/Network/ClientNetwork.hpp"
#include "Client/Render/DefaultRenderSystem.hpp"
#include "Client/Render/Handles.hpp"
#include "Client/Render/Material.hpp"
#include "Client/Render/Mesh.hpp"
#include "Client/Render/RenderComponents.hpp"
#include "Client/Render/RenderMesh.hpp"
#include "Client/Render/Shader.hpp"
#include "Client/Render/Texture2d.hpp"
#include "Client/Render/Vertices/VertexDefault.hpp"
#include "Independent/Math/Transform.hpp"
#include "Independent/Math/TransformSystem.hpp"
#include "Independent/Network/Packet.hpp"

using namespace ShootFast::Client::Core;
using namespace ShootFast::Client::Entity::Systems;
using namespace ShootFast::Client::Entity;
using namespace ShootFast::Client::Render::Vertices;
using namespace ShootFast::Client::Render;
using namespace ShootFast::Client::Network;
using namespace ShootFast::Independent::Math;
using namespace ShootFast::Independent::Network;
using namespace ShootFast::Independent::Utility;

namespace ShootFast::Client
{
    static constexpr int WINDOW_WIDTH = 750;
    static constexpr int WINDOW_HEIGHT = 450;
    static constexpr int CONNECTION_PORT = 7777;

    ClientApplication::~ClientApplication()
    {
        currentState.reset();

        ClientNetwork::GetInstance().Disconnect();

        RenderComponents<Material>::Destroy(renderContext, testMaterial);
        RenderComponents<Mesh<VertexDefault>>::Destroy(renderContext, testMesh);
        RenderComponents<Texture2d>::Destroy(renderContext, testTexture);
        RenderComponents<Shader>::Destroy(renderContext, testShader);
    }

    void ClientApplication::Preinitialize()
    {
        Window::GetInstance().Initialize("ShootFast* 1.8.5", { WINDOW_WIDTH, WINDOW_HEIGHT });

        InputManager::GetInstance().Initialize();

        ClientNetwork::GetInstance().OnConnected.emplace_back([this]
        {
            std::print(std::cout, "Successfully connected to the server.\n");
            SetStage(GameStage::Gameplay);
        });

        ClientNetwork::GetInstance().OnTimeout.emplace_back([this]
        {
            std::print(std::cout, "Server timed out!\n");
            SetStage(GameStage::Disconnected);
        });

        ClientNetwork::GetInstance().OnDisconnected.emplace_back([this]
        {
            std::print(std::cout, "Successfully disconnected from server.\n");
            SetStage(GameStage::Disconnected);
        });

        ClientNetwork::GetInstance().OnPacketReceived.emplace_back([this](const MessageType type, const SerializationBuffer& payload)
        {
            if (type == MessageType::S2C_GameObjectPose)
            {
                S2C_GameObjectPose message;

                message.Deserialize(payload);

                for (const auto& [gameObject, transform, replication] : world.View<Transform, Replication>())
                {
                    if (replication.id != message.handle)
                        continue;

                    transform.position = message.position;
                    transform.rotation = glm::quat(glm::vec3(message.rotation.x, message.rotation.y, 0.0f));

                    break;
                }
            }
        });

        ClientNetwork::GetInstance().OnPacketReceived.emplace_back([this](const MessageType type, const SerializationBuffer& payload)
        {
            if (type == MessageType::S2C_AssignSelf)
            {
                S2C_AssignSelf message{};
                message.Deserialize(payload);

                if (world.Has<Transform>(localPlayerHandle))
                    world.Get<Transform>(localPlayerHandle).position = message.position;

                handleToGameObject[message.handle] = localPlayerHandle;

                if (world.Has<Replication>(localPlayerHandle))
                    world.Get<Replication>(localPlayerHandle).id = message.handle;

                transformSynchronizationSystem.AddObject(localPlayerHandle);
            }
        });

        ClientNetwork::GetInstance().OnPacketReceived.emplace_back([this](const MessageType type, const SerializationBuffer& payload)
        {
            if (type == MessageType::S2C_Spawn)
            {
                S2C_Spawn message{};

                message.Deserialize(payload);

                Independent::ECS::GameObject gameObject = 0;

                if (const auto iterator = handleToGameObject.find(message.handle); iterator != handleToGameObject.end())
                    gameObject = iterator->second;
                else
                {
                    auto [remoteHandle, cameraHandle] = PlayerFactory::CreateLocalPlayer(renderContext, world, true, message.position);
                    gameObject = remoteHandle;

                    world.Add<Replication>(gameObject, Replication{ .id = message.handle, .ownedByLocal = false });

                    handleToGameObject[message.handle] = gameObject;
                }

                if (world.Has<Transform>(gameObject))
                    world.Get<Transform>(gameObject).position = message.position;
            }
        });

        ClientNetwork::GetInstance().OnPacketReceived.emplace_back([this](const MessageType type, const SerializationBuffer& payload)
        {
            if (type == MessageType::S2C_Despawn)
            {
                S2C_Despawn message{};

                message.Deserialize(payload);

                if (const auto iterator = handleToGameObject.find(message.handle); iterator != handleToGameObject.end())
                    world.DestroyGameObject(iterator->second);
            }
        });

        SetStage(GameStage::Connecting);

        ClientNetwork::GetInstance().Connect("127.0.0.1", CONNECTION_PORT);
    }

    bool ClientApplication::IsRunning()
    {
        return Window::GetInstance().IsRunning();
    }

    void ClientApplication::Update()
    {
        if (currentState)
            currentState->Update(*this, deltaSeconds);
    }

    void ClientApplication::Render()
    {
        if (currentState)
            currentState->Render(*this);
    }

    ClientApplication& ClientApplication::GetInstance()
    {
        static ClientApplication instance;

        return instance;
    }

    void ClientApplication::SetStage(const GameStage stage)
    {
        if (currentStage == stage)
            return;

        currentStage = stage;

        switch (currentStage)
        {
            case GameStage::Connecting:
                ChangeState(std::make_unique<Core::States::ConnectingState>());
                break;
            case GameStage::Gameplay:
                ChangeState(std::make_unique<Core::States::GameplayState>());
                break;
            case GameStage::Disconnected:
                ChangeState(std::make_unique<Core::States::DisconnectedState>());
                break;
            case GameStage::None:
            default:
                ChangeState(nullptr);
                break;
        }
    }

    GameStage ClientApplication::GetStage() const
    {
        return currentStage;
    }

    void ClientApplication::ChangeState(std::unique_ptr<Core::GameState> state)
    {
        currentState = std::move(state);

        if (currentState)
            currentState->Initialize(*this);
    }

    void ClientApplication::BuildTestResources()
    {
        const Shader shaderSpecification{ AssetPath{ "ShootFast", "Shader/Default.vert" }, AssetPath{ "ShootFast", "Shader/Default.frag" } };
        testShader = RenderComponents<Shader>::Create(renderContext, shaderSpecification);

        testTexture = RenderComponents<Texture2d>::Upload(renderContext, Texture2d::LoadFromFileUsingFreeImage({ "ShootFast", "Texture/Test.png" }, false, true, true, true));

        Mesh<VertexDefault> meshSpecification{};

        meshSpecification.vertices =
        {
            VertexDefault{ .position={ -0.5f, -0.5f, 0.0f }, .color={ 1.0f, 0.0f, 0.0f }, .normal={}, .uvs={ 0.0f, 1.0f } },
            VertexDefault{ .position={  0.5f, -0.5f, 0.0f }, .color={ 0.0f, 1.0f, 0.0f }, .normal={}, .uvs={ 1.0f, 1.0f } },
            VertexDefault{ .position={  0.0f,  0.5f, 0.0f }, .color={ 0.0f, 0.0f, 1.0f }, .normal={}, .uvs={ 1.0f, 0.0f } }
        };

        meshSpecification.indices = { 0, 1, 2 };

        testMesh = RenderComponents<Mesh<VertexDefault>>::Upload(renderContext, meshSpecification);

        testMaterial = RenderComponents<Material>::Create(renderContext, { &testShader, &testTexture });
    }

    void ClientApplication::CreateTestEntity()
    {
        testEntity = world.CreateGameObject();

        world.Add<Transform>(testEntity, Transform{ .position = { 0.0f, 0.0f, -2.0f }, .rotation = {  }, .scale = { 1.0f, 1.0f, 1.0f } });
        world.Add<RenderMesh<VertexDefault>>(testEntity, RenderMesh<VertexDefault>{ testMesh, testMaterial });
    }
}

int main(int, char**)
{
    ShootFast::Client::ClientApplication::GetInstance().Preinitialize();

    while (ShootFast::Client::ClientApplication::IsRunning())
    {
        ShootFast::Client::ClientApplication::GetInstance().Update();
        ShootFast::Client::ClientApplication::GetInstance().Render();
    }

    return 0;
}