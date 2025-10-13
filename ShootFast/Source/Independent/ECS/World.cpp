#include "Independent/ECS/World.hpp"
#include "Independent/ECS/GameObject.hpp"
#include "Independent/ECS/GameObjectAllocator.hpp"

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
        for (auto& [_, pool] : poolMap)
            pool->Remove(gameObject);

        allocator->Destroy(gameObject);
    }
}
