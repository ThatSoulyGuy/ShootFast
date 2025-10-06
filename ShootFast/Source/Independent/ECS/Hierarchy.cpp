#include "Independent/ECS/Hierarchy.hpp"
#include <algorithm>
#include "Independent/ECS/ComponentPool.hpp"
#include "Independent/ECS/GameObject.hpp"
#include "Independent/ECS/World.hpp"

namespace ShootFast::Independent::ECS
{
    static void RemoveChildFrom(World& world, const GameObject parent, const GameObject child)
    {
        if (!world.Has<Children>(parent))
            return;

        auto& list = world.Get<Children>(parent).value;

        if (const auto iterator = std::ranges::remove(list, child).begin(); iterator != list.end())
            list.erase(iterator, list.end());
    }

    void SetParent(World& world, const GameObject child, const GameObject parent)
    {
        if (world.Has<Parent>(child))
            RemoveChildFrom(world, world.Get<Parent>(child).value, child);

        world.Add<Parent>(child, Parent{.value = parent});

        if (!world.Has<Children>(parent))
            world.Add<Children>(parent, Children{});

        world.Get<Children>(parent).value.push_back(child);
    }

    void ClearParent(World& world, const GameObject child)
    {
        if (!world.Has<Parent>(child))
            return;

        const GameObject parent = world.Get<Parent>(child).value;

        RemoveChildFrom(world, parent, child);

        world.Remove<Parent>(child);
    }
}