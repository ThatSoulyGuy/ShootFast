#ifndef SHOOTFAST_CLIENT_APPLICATION_HPP
#define SHOOTFAST_CLIENT_APPLICATION_HPP

#include "Independent/ECS/GameObject.hpp"
#include "Independent/ECS/World.hpp"
#include "Render/Camera.hpp"
#include "Render/ClientRenderContext.hpp"
#include "Render/Handles.hpp"
#include "Render/Registries.hpp"

namespace ShootFast::Client
{
    class ClientApplication final
    {

    public:

        ClientApplication(const ClientApplication&) = delete;
        ClientApplication(ClientApplication&&) = delete;
        ClientApplication& operator=(const ClientApplication&) = delete;
        ClientApplication& operator=(ClientApplication&&) = delete;

        ~ClientApplication();

        void Preinitialize();
        void Initialize();

        [[nodiscard]]
        static bool IsRunning();

        void Update();
        void Render();

        static ClientApplication& GetInstance();

    private:

        ClientApplication() = default;

        Render::MeshRegistry meshRegistry;
        Render::ShaderRegistry shaderRegistry;
        Render::TextureRegistry textureRegistry;
        Render::MaterialRegistry materialRegistry;
        Render::ClientRenderContext renderContext{ &meshRegistry, &materialRegistry, &shaderRegistry, &textureRegistry };

        Independent::ECS::World world;

        Render::ShaderHandle testShader{};
        Render::TextureHandle testTexture{};
        Render::MeshHandle testMesh{};
        Render::MaterialHandle testMaterial{};

        Independent::ECS::GameObject testEntity{ 0 };

        void BuildTestResources();
        void CreateTestEntity();

        float deltaSeconds{ 1.0f / 60.0f };
        uint64_t frameIndex{ 0 };

    };
}

#endif