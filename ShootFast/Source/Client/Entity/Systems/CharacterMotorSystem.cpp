#include "Client/Entity/Systems/CharacterMotorSystem.hpp"
#include <glm/vec3.hpp>
#include "Client/Entity/Player.hpp"
#include "Client/Entity/PlayerInput.hpp"
#include "Independent/ECS/World.hpp"
#include "Independent/Math/Transform.hpp"
#include "Independent/Network/CommonNetwork.hpp"

using namespace ShootFast::Independent::ECS;
using namespace ShootFast::Independent::Math;

namespace ShootFast::Client::Entity::Systems
{
    void CharacterMotorSystem::Run(World& world) const
    {
        for (auto [gameObject, transform, player, input] : world.View<Transform, Player, PlayerInput>())
        {
            if (world.Has<Independent::Network::Remote>(gameObject))
                return;

            float speed = player.moveSpeed;

            if (input.run)
                speed *= player.runMultiplier;

            constexpr glm::vec3 forward{ 0.0f, 0.0f, 1.0f };
            constexpr glm::vec3 right{ 1.0f, 0.0f, 0.0f };

            transform.position += (right * input.moveAxis.x + forward * input.moveAxis.y) * speed * deltaSeconds;
        }
    }
}
