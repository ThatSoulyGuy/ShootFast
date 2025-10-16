#include "Client/Entity/PlayerFactory.hpp"
#include "Client/Entity/Health.hpp"
#include "Client/Entity/Player.hpp"
#include "Client/Entity/PlayerInput.hpp"
#include "Client/Render/Camera.hpp"
#include "Client/Render/RenderMesh.hpp"
#include "Independent/ECS/GameObject.hpp"
#include "Independent/ECS/Hierarchy.hpp"
#include "Independent/ECS/World.hpp"
#include "Independent/Math/Transform.hpp"
#include "Independent/Network/CommonNetwork.hpp"

using namespace ShootFast::Client::Render;
using namespace ShootFast::Independent::Math;

#ifndef IS_SERVER_FLAG
#include "Client/Render/ClientRenderContext.hpp"
#include "Client/Render/RenderComponents.hpp"
#include "Client/Render/Vertices/VertexDefault.hpp"
#include "Client/Render/Mesh.hpp"
#include "Client/Render/Material.hpp"
#include "Client/Render/Texture2d.hpp"
#include "Client/Render/Shader.hpp"

using Vertices::VertexDefault;

namespace
{
    struct RemoteVisuals
    {
        Shader::Handle shader{};
        Texture2d::Handle white{};
        Material::Handle material{};
        Mesh<VertexDefault>::Handle cube{};

        [[nodiscard]]
        bool IsReady() const
        {
            return shader && white && material && cube;
        }
    };

    RemoteVisuals& GetRemoteVisuals()
    {
        static RemoteVisuals visuals;
        return visuals;
    }

    void EnsureRemoteVisuals(ClientRenderContext& context)
    {
        auto& remoteVisuals = GetRemoteVisuals();

        if (remoteVisuals.IsReady())
            return;

        if (!remoteVisuals.shader)
        {
            const Shader specification
            {
                ShootFast::Independent::Utility::AssetPath{ "ShootFast", "Shader/Default.vert" },
                ShootFast::Independent::Utility::AssetPath{ "ShootFast", "Shader/Default.frag" }
            };

            remoteVisuals.shader = RenderComponents<Shader>::Create(context, specification);
        }

        if (!remoteVisuals.white)
        {
            Texture2d tex{};

            tex.width = tex.height = 1;
            tex.format = TextureFormat::RGBA8;
            tex.generateMips = false;
            tex.sRGB = false;
            tex.pixels = { 255, 255, 255, 255 };

            remoteVisuals.white = RenderComponents<Texture2d>::Upload(context, tex);
        }

        if (!remoteVisuals.material)
            remoteVisuals.material = RenderComponents<Material>::Create(context, Material{ &remoteVisuals.shader, &remoteVisuals.white });

        if (!remoteVisuals.cube)
        {
            Mesh<VertexDefault> m{};

            auto pushFace = [&](const glm::vec3 n, const glm::vec3 a, const glm::vec3 b, const glm::vec3 c, const glm::vec3 d)
            {
                constexpr glm::vec3 col{1.f,1.f,1.f};

                m.vertices.push_back(VertexDefault{ .position=a, .color=col, .normal=n, .uvs={0.f,0.f} });
                m.vertices.push_back(VertexDefault{ .position=b, .color=col, .normal=n, .uvs={1.f,0.f} });
                m.vertices.push_back(VertexDefault{ .position=c, .color=col, .normal=n, .uvs={1.f,1.f} });
                m.vertices.push_back(VertexDefault{ .position=d, .color=col, .normal=n, .uvs={0.f,1.f} });

                const uint32_t base = static_cast<uint32_t>(m.vertices.size()) - 4;

                m.indices.insert(m.indices.end(),
                {
                    base+0, base+1, base+2,
                    base+0, base+2, base+3
                });
            };

            constexpr float h = 0.5f;

            pushFace({+1,0,0}, {+h,-h,-h}, {+h,-h,+h}, {+h,+h,+h}, {+h,+h,-h});
            pushFace({-1,0,0}, {-h,-h,+h}, {-h,-h,-h}, {-h,+h,-h}, {-h,+h,+h});
            pushFace({0,+1,0}, {-h,+h,-h}, {+h,+h,-h}, {+h,+h,+h}, {-h,+h,+h});
            pushFace({0,-1,0}, {-h,-h,+h}, {+h,-h,+h}, {+h,-h,-h}, {-h,-h,-h});
            pushFace({0,0,+1}, {-h,-h,+h}, {+h,-h,+h}, {+h,+h,+h}, {-h,+h,+h});
            pushFace({0,0,-1}, {+h,-h,-h}, {-h,-h,-h}, {-h,+h,-h}, {+h,+h,-h});

            remoteVisuals.cube = RenderComponents<Mesh<VertexDefault>>::Upload(context, m);
        }
    }

    void AttachRemoteCube(ClientRenderContext& context, ShootFast::Independent::ECS::World& world, const ShootFast::Independent::ECS::GameObject gameObject)
    {
        EnsureRemoteVisuals(context);

        const auto& visuals = GetRemoteVisuals();
        world.Add<RenderMesh<VertexDefault>>(gameObject, RenderMesh<VertexDefault>{ visuals.cube, visuals.material });
    }
}
#endif

namespace ShootFast::Client::Entity
{
    PlayerFactory PlayerFactory::CreateLocalPlayer(ClientRenderContext& context, Independent::ECS::World& world, const bool isRemote, const glm::vec3& startPosition)
    {
        PlayerFactory result{};

        result.player = world.CreateGameObject();
        
        world.Add<Transform>(result.player, Transform{ .position = startPosition });
        world.Add<Player>(result.player, Player{ .team = Team::RED, .moveSpeed = 5.5f, .runMultiplier = 1.75f, .jumpHeight = 0.0f });
        world.Add<Health>(result.player, Health{ .current = 100, .maximum = 100 });
        world.Add<PlayerInput>(result.player, PlayerInput{});

        if (isRemote)
        {
            world.Add<Independent::Network::Remote>(result.player, Independent::Network::Remote{});

#ifndef IS_SERVER_FLAG
            AttachRemoteCube(context, world, result.player);
#endif
        }
        else
        {
            result.camera = world.CreateGameObject();

            world.Add<Transform>(result.camera, Transform{ .position = { 0.0f, 1.7f, 0.0f } });
            world.Add<Camera>(result.camera, Camera{ .fovDegrees = 70.0f, .active = true, .offsetY = 0.0f });
            world.Add<LookAngles>(result.camera);

            world.Get<Player>(result.player).cameraHandle = result.camera;

            SetParent(world, result.camera, result.player);
        }

        return result;
    }
}
