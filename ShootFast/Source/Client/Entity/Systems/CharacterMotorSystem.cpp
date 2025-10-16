#include "Client/Entity/Systems/CharacterMotorSystem.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Client/Entity/Player.hpp"
#include "Client/Entity/PlayerInput.hpp"
#include "Independent/ECS/Hierarchy.hpp"
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
                continue;

            float speed = player.moveSpeed;

            if (input.run)
                speed *= player.runMultiplier;

            const auto& cameraTransform = world.Get<Transform>(player.cameraHandle);

            glm::vec3 forward = cameraTransform.rotation * glm::vec3(0.f, 0.f, 1.f);
            glm::vec3 right = cameraTransform.rotation * glm::vec3(1.f, 0.f, 0.f);

            forward.y = 0.f;
            right.y = 0.f;

            if (glm::length2(forward) > 0.f)
                forward = glm::normalize(forward);

            if (glm::length2(right) > 0.f)
                right = glm::normalize(right);

            transform.position += (right * input.moveAxis.x + forward * input.moveAxis.y) * speed * deltaSeconds;
        }
    }
}
