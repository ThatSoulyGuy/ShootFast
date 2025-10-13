#include "Client/Entity/PlayerFactory.hpp"
#include "Client/Entity/Health.hpp"
#include "Client/Entity/Player.hpp"
#include "Client/Entity/PlayerInput.hpp"
#include "Client/Render/Camera.hpp"
#include "Independent/ECS/Hierarchy.hpp"
#include "Independent/ECS/World.hpp"
#include "Independent/Math/Transform.hpp"

using namespace ShootFast::Client::Render;
using namespace ShootFast::Independent::Math;

namespace ShootFast::Client::Entity
{
    PlayerFactory PlayerFactory::CreateLocalPlayer(Independent::ECS::World& world, const glm::vec3& startPosition)
    {
        PlayerFactory result{};

        result.player = world.CreateGameObject();
        
        world.Add<Transform>(result.player, Transform{ .position = startPosition });
        world.Add<Player>(result.player, Player{ .team = Team::RED, .moveSpeed = 5.5f, .runMultiplier = 1.75f, .jumpHeight = 0.0f });
        world.Add<Health>(result.player, Health{ .current = 100, .maximum = 100 });
        world.Add<PlayerInput>(result.player, PlayerInput{});

        result.camera = world.CreateGameObject();
        
        world.Add<Transform>(result.camera, Transform{ .position = { 0.0f, 1.7f, 0.0f } });
        world.Add<Camera>(result.camera, Camera{ .fovDegrees = 70.0f, .active = true, .offsetY = 0.0f });

        SetParent(world, result.camera, result.player);

        return result;
    }
}
