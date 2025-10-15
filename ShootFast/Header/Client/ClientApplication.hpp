#ifndef SHOOTFAST_CLIENT_APPLICATION_HPP
#define SHOOTFAST_CLIENT_APPLICATION_HPP

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "Client/Core/GameState.hpp"
#include "Independent/ECS/GameObject.hpp"
#include "Independent/ECS/World.hpp"
#include "Independent/Network/TransformSyncSystem.hpp"
#include "Render/ClientRenderContext.hpp"
#include "Render/Handles.hpp"
#include "Render/Registries.hpp"

namespace ShootFast::Client
{
    namespace Core::States
    {
        class ConnectingState;
        class GameplayState;
        class DisconnectedState;
    }

    enum class GameStage
    {
        None,
        Connecting,
        Gameplay,
        Disconnected
    };

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

        void SetStage(GameStage stage);
        [[nodiscard]] GameStage GetStage() const;

    private:

        ClientApplication() = default;

        void ChangeState(std::unique_ptr<Core::GameState> state);

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

        Independent::Network::TransformSyncSystem transformSynchronizationSystem;
        Independent::ECS::GameObject testEntity{ 0 };

        std::unordered_map<std::uint32_t, Independent::ECS::GameObject> handleToGameObject;
        Independent::ECS::GameObject localPlayerGO{0};

        void BuildTestResources();
        void CreateTestEntity();

        void UpdateConnecting(float deltaSeconds);
        void RenderConnecting();
        void UpdateGameplay(float deltaSeconds);
        void RenderGameplay();
        void UpdateDisconnected(float deltaSeconds);
        void RenderDisconnected();

        float deltaSeconds{ 1.0f / 60.0f };
        uint64_t frameIndex{ 0 };

        std::unique_ptr<Core::GameState> currentState{};
        GameStage currentStage{ GameStage::None };

        friend class Core::States::ConnectingState;
        friend class Core::States::GameplayState;
        friend class Core::States::DisconnectedState;

    };
}

#endif
