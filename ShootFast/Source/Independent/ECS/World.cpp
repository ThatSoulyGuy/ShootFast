#include "Independent/ECS/World.hpp"

#include <algorithm>
#include <ranges>
#include "Independent/ECS/GameObject.hpp"
#include "Independent/ECS/GameObjectAllocator.hpp"
#include "Independent/ECS/Hierarchy.hpp"

namespace ShootFast::Independent::ECS
{
    World::World()
    {
        allocator = std::make_unique<GameObjectAllocator>();
    }

    World::~World() = default;

    GameObject World::CreateGameObject() const
    {
        return allocator->Create();
    }

    void World::DestroyGameObject(const GameObject gameObject)
    {
        if (Has<Children>(gameObject))
        {
            const auto children = Get<Children>(gameObject).value;

            for (const auto child : children)
                DestroyGameObject(child);
        }

        if (Has<Parent>(gameObject))
        {
            const GameObject parent = Get<Parent>(gameObject).value;

            if (Has<Children>(parent))
            {
                auto& siblings = Get<Children>(parent).value;

                siblings.erase(std::ranges::remove(siblings, gameObject).begin(), siblings.end());
            }
        }

        for (const auto& pool : poolMap | std::views::values)
            pool->Remove(gameObject);

        allocator->Destroy(gameObject);
    }
}
