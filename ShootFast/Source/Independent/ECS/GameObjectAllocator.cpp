#include "Independent/ECS/GameObjectAllocator.hpp"
#include "Independent/ECS/GameObject.hpp"

namespace ShootFast::Independent::ECS
{
    GameObject GameObjectAllocator::Create()
    {
        if (!freeList.empty())
        {
            const GameObject gameObject = freeList.back();

            freeList.pop_back();

            allocated[gameObject] = true;

            return gameObject;
        }

        const GameObject gameObject = nextId++;

        if (allocated.size() <= gameObject)
            allocated.resize(static_cast<std::size_t>(nextId), false);

        allocated[gameObject] = true;

        return gameObject;
    }

    void GameObjectAllocator::Destroy(const GameObject gameObject)
    {
        if (gameObject == 0u)
            return;

        if (gameObject >= allocated.size() || !allocated[gameObject])
            return;

        allocated[gameObject] = false;
        freeList.push_back(gameObject);
    }
}
