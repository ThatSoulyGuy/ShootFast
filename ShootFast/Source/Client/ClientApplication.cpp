#include "Client/ClientApplication.hpp"
#include <iostream>
#include <print>
#include "Client/Core/Window.hpp"
#include "Client/Network/ClientNetwork.hpp"
#include "Client/Render/Handles.hpp"
#include "Client/Render/Material.hpp"
#include "Client/Render/Mesh.hpp"
#include "Client/Render/RenderComponents.hpp"
#include "Client/Render/RenderMesh.hpp"
#include "Client/Render/RenderSystem.hpp"
#include "Client/Render/Shader.hpp"
#include "Client/Render/Texture2d.hpp"
#include "Client/Render/Vertices/VertexDefault.hpp"
#include "Independent/Math/Transform.hpp"
#include "Independent/Math/TransformSystem.hpp"

using namespace ShootFast::Client::Core;
using namespace ShootFast::Client::Render::Vertices;
using namespace ShootFast::Client::Render;
using namespace ShootFast::Client::Network;
using namespace ShootFast::Independent::Math;

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
    }

    bool ClientApplication::IsRunning()
    {
        return Window::GetInstance().IsRunning();
    }

    void ClientApplication::Update()
    {
        ClientNetwork::GetInstance().Poll(1);

        TransformSystem{}.Run(world);

        frameIndex += 1;
    }

    void ClientApplication::Render()
    {
        Window::Clear();

        RenderSystem{ renderContext }.Run(world);

        Window::GetInstance().Present();
    }

    void ClientApplication::BuildTestResources()
    {
        const std::string vertexSource = R"(#version 410 core
            layout(location=0) in vec3 inPosition;
            uniform mat4 uModel;
            void main(){ gl_Position = uModel * vec4(inPosition,1.0); })";

        const std::string fragmentSource = R"(#version 410 core
            out vec4 FragColor;
            uniform sampler2D uAlbedo;
            void main(){ FragColor = texture(uAlbedo, vec2(0.5)); })";

        const Shader shaderSpecification{ vertexSource, fragmentSource };
        testShader = RenderComponents<Shader>::Create(renderContext, shaderSpecification);

        Texture2d textureSpecification{};

        textureSpecification.width = 2;
        textureSpecification.height = 2;
        textureSpecification.format = TextureFormat::RGBA8;
        textureSpecification.generateMips = true;
        textureSpecification.pixels = { 255,0,0,255,  0,255,0,255,  0,0,255,255,  255,255,255,255 };

        testTexture = RenderComponents<Texture2d>::Upload(renderContext, textureSpecification);

        Mesh<VertexDefault> meshSpecification{};

        meshSpecification.vertices =
        {
            VertexDefault{ .position={ -0.5f, -0.5f, 0.0f }, .color={ 1.0f, 0.0f, 0.0f }, .normal={}, .uvs={ 0.0f, 0.0f } },
            VertexDefault{ .position={  0.5f, -0.5f, 0.0f }, .color={ 0.0f, 1.0f, 0.0f }, .normal={}, .uvs={ 1.0f, 1.0f } },
            VertexDefault{ .position={  0.0f,  0.5f, 0.0f }, .color={ 0.0f, 0.0f, 1.0f }, .normal={}, .uvs={ 1.0f, 1.0f } }
        };

        meshSpecification.indices = { 0, 1, 2 };

        testMesh = RenderComponents<Mesh<VertexDefault>>::Upload(renderContext, meshSpecification);

        testMaterial = RenderComponents<Material>::Create(renderContext, { &testShader, &testTexture });
    }

    void ClientApplication::CreateTestEntity()
    {
        testEntity = world.CreateGameObject();

        world.Add<Transform>(testEntity, Transform{ .position = { 0.0f, 0.0f, 0.0f }, .rotation = {  }, .scale = { 1.0f, 1.0f, 1.0f } });
        world.Add<LocalToWorld>(testEntity, LocalToWorld{});
        world.Add<RenderMesh>(testEntity, RenderMesh{ testMesh, testMaterial });
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

    while (ShootFast::Client::ClientApplication::GetInstance().IsRunning())
    {
        ShootFast::Client::ClientApplication::GetInstance().Update();
        ShootFast::Client::ClientApplication::GetInstance().Render();
    }

    return 0;
}