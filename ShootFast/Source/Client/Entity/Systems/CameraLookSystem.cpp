#include "Client/Entity/Systems/CameraLookSystem.hpp"
#include <algorithm>
#include <glm/gtc/quaternion.hpp>
#include "Client/Core/InputManager.hpp"
#include "Client/Render/Camera.hpp"
#include "Independent/ECS/Hierarchy.hpp"
#include "Independent/ECS/World.hpp"
#include "Independent/Math/Transform.hpp"
#include "Independent/Network/CommonNetwork.hpp"

using namespace ShootFast::Client::Core;
using namespace ShootFast::Client::Render;
using namespace ShootFast::Independent::ECS;
using namespace ShootFast::Independent::Math;

namespace ShootFast::Client::Entity::Systems
{
    void CameraLookSystem::Run(World& world) const
    {
        if (InputManager::GetInstance().GetMouseMode() != MouseMode::LOCKED)
            return;

        const auto mouseDelta = InputManager::GetInstance().GetMouseDelta();

        if (mouseDelta.x == 0.0f && mouseDelta.y == 0.0f)
            return;

        const float yawDelta = mouseDelta.x * sensitivity;
        const float pitchDelta = (invertY ?  mouseDelta.y : -mouseDelta.y) * sensitivity;

        for (auto [cameraHandle, camera, cameraTransform, parent, look] : world.View<Camera, Transform, Parent, LookAngles>())
        {
            look.yawDegrees -= yawDelta;
            look.pitchDegrees += pitchDelta;

            look.pitchDegrees = std::clamp(look.pitchDegrees, -89.0f, 89.0f);

            const glm::quat yawQ = glm::angleAxis(glm::radians(look.yawDegrees), glm::vec3(0.f, 1.f, 0.f));
            const glm::quat pitchQ = glm::angleAxis(glm::radians(look.pitchDegrees), glm::vec3(1.f, 0.f, 0.f));

            cameraTransform.rotation = yawQ * pitchQ;
        }
    }
}
