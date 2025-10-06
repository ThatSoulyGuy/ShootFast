#include "Independent/Math/TransformSystem.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "Independent/ECS/World.hpp"
#include "Independent/Math/Transform.hpp"

using namespace ShootFast::Independent::ECS;

namespace ShootFast::Independent::Math
{
    static glm::mat4 MakeLocalMatrix(const Transform& t)
    {
        const glm::mat4 T = glm::translate(glm::mat4(1.0f), t.position);
        const glm::mat4 R = glm::toMat4(t.rotation);
        const glm::mat4 S = glm::scale(glm::mat4(1.0f), t.scale);

        return T * R * S;
    }

    void TransformSystem::Run(World& world) const
    {
        for (auto [gameObject, transform, localToWorld] : world.View<Transform, LocalToWorld>())
            localToWorld.matrix = MakeLocalMatrix(transform);
    }
}
