#include "Client/Entity/Systems/PlayerInputSystem.hpp"
#include <glm/vec2.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include "Client/Core/InputManager.hpp"
#include "Client/Entity/PlayerInput.hpp"
#include "Independent/ECS/World.hpp"

using namespace ShootFast::Client::Core;
using namespace ShootFast::Independent::ECS;

namespace ShootFast::Client::Entity::Systems
{
    void PlayerInputSystem::Run(World& world)
    {
        glm::vec2 axis{ 0.0f, 0.0f };

        const bool forward = InputManager::GetInstance().GetKeyState(KeyCode::W, KeyState::HELD);
        const bool back = InputManager::GetInstance().GetKeyState(KeyCode::S, KeyState::HELD);
        const bool left = InputManager::GetInstance().GetKeyState(KeyCode::A, KeyState::HELD);
        const bool right = InputManager::GetInstance().GetKeyState(KeyCode::D, KeyState::HELD);
        const bool run = InputManager::GetInstance().GetKeyState(KeyCode::LEFT_SHIFT, KeyState::HELD);

        if (forward)
            axis.y -= 1.0f;

        if (back)
            axis.y += 1.0f;

        if (right)
            axis.x += 1.0f;

        if (left)
            axis.x -= 1.0f;

        if (axis.x != 0.0f || axis.y != 0.0f)
        {
            if (const float length = glm::length(axis); length > 0.0f)
                axis /= length;
        }

        for (auto [gameObject, input] : world.View<PlayerInput>())
        {
            input.moveAxis = axis;
            input.run = run;
        }
    }
}
