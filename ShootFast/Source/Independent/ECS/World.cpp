#include "Independent/ECS/World.hpp"
#include "Independent/ECS/ComponentPool.hpp"
#include "Independent/ECS/GameObject.hpp"
#include "Independent/ECS/GameObjectAllocator.hpp"

namespace ShootFast::Independent::ECS
{
    World::World()
    {
        allocator = new GameObjectAllocator();
    }

    World::~World()
    {
        for (auto& [key, pointer] : poolMap)
            destructorMap[key](pointer);

        delete allocator;
    }

    GameObject World::CreateGameObject() const
    {
        return allocator->Create();
    }

    void World::DestroyGameObject(const GameObject gameObject)
    {
        for (auto& eraser: eraserMap | std::views::values)
            eraser(gameObject);

        allocator->Destroy(gameObject);
    }

    template <>
    void World::ViewRange<>::Iterator::Advance()
    {
        while (index < owners->size() && !Matches((*owners)[index]))
            index += 1;
    }

    template<>
    World::ViewRange<>::Iterator World::ViewRange<>::begin() const
    {
        Iterator iterator{world, owners, 0u};

        iterator.Advance();

        return iterator;
    }

    template<>
    World::ViewRange<>::Iterator World::ViewRange<>::end() const
    {
        return Iterator{world, owners, owners->size()};
    }

    template <typename Head>
    ComponentPool<Head>* World::SelectSmallestPool()
    {
        return GetPoolPointer<Head>();
    }

    template <typename Head, typename Next, typename... Rest>
    ComponentPool<Head>* World::SelectSmallestPool()
    {
        auto* head = GetPoolPointer<Head>();
        auto* smallestPool = SelectSmallestPool<Next, Rest...>();

        return head->Owners().size() <= smallestPool->Owners().size() ? head : reinterpret_cast<ComponentPool<Head>*>(smallestPool);
    }
}
