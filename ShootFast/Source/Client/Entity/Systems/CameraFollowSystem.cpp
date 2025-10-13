#include "Client/Entity/Systems/CameraFollowSystem.hpp"
#include "Client/Render/Camera.hpp"
#include "Independent/ECS/Hierarchy.hpp"
#include "Independent/ECS/World.hpp"
#include "Independent/Math/Transform.hpp"

using namespace ShootFast::Client::Render;
using namespace ShootFast::Independent::Math;
using namespace ShootFast::Independent::ECS;

namespace ShootFast::Client::Entity::Systems
{
    void CameraFollowSystem::Run(World& world)
    {
        for (auto [gameObject, camera, transform] : world.View<Camera, Transform>())
        {
            if (world.Has<Parent>(gameObject))
                transform.position.y = camera.offsetY;
        }
    }
}
