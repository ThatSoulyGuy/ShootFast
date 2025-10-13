#include "Client/ClientApplication.hpp"
#include <iostream>
#include <print>

#include "Client/Core/InputManager.hpp"
#include "Client/Core/Window.hpp"
#include "Client/Entity/PlayerFactory.hpp"
#include "Client/Entity/Systems/CameraFollowSystem.hpp"
#include "Client/Entity/Systems/CharacterMotorSystem.hpp"
#include "Client/Entity/Systems/PlayerInputSystem.hpp"
#include "Client/Network/ClientNetwork.hpp"
#include "Client/Render/Handles.hpp"
#include "Client/Render/Material.hpp"
#include "Client/Render/Mesh.hpp"
#include "Client/Render/RenderComponents.hpp"
#include "Client/Render/RenderMesh.hpp"
#include "Client/Render/DefaultRenderSystem.hpp"
#include "Client/Render/Shader.hpp"
#include "Client/Render/Texture2d.hpp"
#include "Client/Render/Vertices/VertexDefault.hpp"
#include "Independent/Math/Transform.hpp"
#include "Independent/Math/TransformSystem.hpp"

using namespace ShootFast::Client::Core;
using namespace ShootFast::Client::Entity::Systems;
using namespace ShootFast::Client::Entity;
using namespace ShootFast::Client::Render::Vertices;
using namespace ShootFast::Client::Render;
using namespace ShootFast::Client::Network;
using namespace ShootFast::Independent::Math;
using namespace ShootFast::Independent::Utility;

namespace ShootFast::Client
{
    static constexpr int WINDOW_WIDTH = 750;
    static constexpr int WINDOW_HEIGHT = 450;
    static constexpr int CONNECTION_PORT = 7777;

    ClientApplication::~ClientApplication()
    {
        ClientNetwork::GetInstance().Disconnect();

        RenderComponents<Material>::Destroy(renderContext, testMaterial);
        RenderComponents<Mesh<VertexDefault>>::Destroy(renderContext, testMesh);
        RenderComponents<Texture2d>::Destroy(renderContext, testTexture);
        RenderComponents<Shader>::Destroy(renderContext, testShader);
    }

    void ClientApplication::Preinitialize()
    {
        Window::GetInstance().Initialize("ShootFast* 1.8.4", { WINDOW_WIDTH, WINDOW_HEIGHT });

        InputManager::GetInstance().Initialize();

        ClientNetwork::GetInstance().OnConnected.emplace_back([]
        {
            std::print(std::cout, "Successfully connected to the server.\n");
        });

        ClientNetwork::GetInstance().OnTimeout.emplace_back([]
        {
            std::print(std::cout, "Server timed out!\n");
        });

        ClientNetwork::GetInstance().OnDisconnected.emplace_back([]
        {
            std::print(std::cout, "Successfully disconnected from server.\n");
        });

        ClientNetwork::GetInstance().Connect("127.0.0.1", CONNECTION_PORT);
    }

    void ClientApplication::Initialize()
    {
        BuildTestResources();
        CreateTestEntity();

        auto [playerHandle, cameraHandle] = PlayerFactory::CreateLocalPlayer(world, { 0.0f, 0.0f, 0.0f });

        renderContext.cameraHandle = cameraHandle;
    }

    bool ClientApplication::IsRunning()
    {
        return Window::GetInstance().IsRunning();
    }

    void ClientApplication::Update()
    {
        ClientNetwork::GetInstance().Poll(1);

        PlayerInputSystem::Run(world);

        const CharacterMotorSystem motorSystem(1.0f / 60.0f);
        motorSystem.Run(world);

        CameraFollowSystem cameraFollow;
        cameraFollow.Run(world);

        TransformSystem::Run(world);

        InputManager::GetInstance().Update();

        frameIndex += 1;
    }

    void ClientApplication::Render()
    {
        Window::Clear();

        DefaultRenderSystem{ renderContext }.Run(world);

        Window::GetInstance().Present();
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

    ClientApplication& ClientApplication::GetInstance()
    {
        static ClientApplication instance;

        return instance;
    }
}

int main(int, char**)
{
    ShootFast::Client::ClientApplication::GetInstance().Preinitialize();
    ShootFast::Client::ClientApplication::GetInstance().Initialize();

    while (ShootFast::Client::ClientApplication::IsRunning())
    {
        ShootFast::Client::ClientApplication::GetInstance().Update();
        ShootFast::Client::ClientApplication::GetInstance().Render();
    }

    return 0;
}