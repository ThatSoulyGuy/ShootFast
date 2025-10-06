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

            return gameObject;
        }

        return nextId++;
    }

    void GameObjectAllocator::Destroy(const GameObject gameObject)
    {
        freeList.push_back(gameObject);
    }
}
