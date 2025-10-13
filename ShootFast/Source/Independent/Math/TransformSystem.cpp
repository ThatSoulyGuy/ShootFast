#include "Independent/Math/TransformSystem.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Independent/ECS/Hierarchy.hpp"
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

    static void ComposeSubtree(World& world, const uint32_t node, const glm::mat4& parentL2W)
    {
        auto& transform  = world.Get<Transform>(node);

        transform.matrix = parentL2W * MakeLocalMatrix(transform);

        if (world.Has<Children>(node))
        {
            for (const uint32_t child : world.Get<Children>(node).value)
            {
                if (world.Has<Transform>(child))
                    ComposeSubtree(world, child, transform.matrix);
            }
        }
    }

    void TransformSystem::Run(World& world)
    {
        for (auto [gameObject, transform] : world.View<Transform>())
        {
            if (!world.Has<Parent>(gameObject))
            {
                transform.matrix = MakeLocalMatrix(transform);

                if (world.Has<Children>(gameObject))
                {
                    for (const uint32_t child : world.Get<Children>(gameObject).value)
                    {
                        if (world.Has<Transform>(child))
                            ComposeSubtree(world, child, transform.matrix);
                    }
                }
            }
        }
    }
}
