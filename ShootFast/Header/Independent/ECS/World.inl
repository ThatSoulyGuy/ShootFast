#pragma once

#include <typeindex>
#include "Independent/ECS/ComponentPool.hpp"

namespace ShootFast::Independent::ECS
{
    template <typename T>
    T& World::Add(uint32_t gameObject, const T& value)
    {
        auto& pool = GetPool<T>();

        return pool.Emplace(gameObject, value);
    }

    template <typename T>
    bool World::Has(uint32_t gameObject) const
    {
        const auto iterator = poolMap.find(typeid(T));

        if (iterator == poolMap.end())
            return false;

        return static_cast<ComponentPool<T>*>(iterator->second)->Has(gameObject);
    }

    template <typename T>
    T& World::Get(uint32_t gameObject)
    {
        return GetPool<T>().Get(gameObject);
    }

    template <typename T>
    void World::Remove(uint32_t gameObject)
    {
        GetPool<T>().Remove(gameObject);
    }

    template <typename... T>
    World::ViewRange<T...> World::View()
    {
        const std::vector<uint32_t>* result = nullptr;

        auto pick = [&](auto* pointer)
        {
            const auto* owner = &pointer->GetOwnerList();

            if (result == nullptr || owner->size() < result->size())
                result = owner;
        };

        (pick(GetPoolPointer<T>()), ...);

        return ViewRange<T...>(this, result);
    }

    template <typename... T>
    void World::ViewRange<T...>::Iterator::Advance()
    {
        while (index < owners->size() && !Matches((*owners)[index])) { index += 1; }
    }

    template <typename... T>
    auto World::ViewRange<T...>::begin() const -> Iterator
    {
        Iterator iterator{ world, owners, 0u };

        iterator.Advance();

        return iterator;
    }

    template <typename... T>
    auto World::ViewRange<T...>::end() const -> Iterator
    {
        return Iterator{ world, owners, owners->size() };
    }

    template <typename T>
    ComponentPool<T>& World::GetPool()
    {
        const std::type_index key = typeid(T);
        const auto iterator = poolMap.find(key);

        if (iterator == poolMap.end())
        {
            auto* created = new ComponentPool<T>();

            poolMap.emplace(key, created);

            destructorMap.emplace(key, [](void* pointer)
            {
                delete static_cast<ComponentPool<T>*>(pointer);
            });

            eraserMap.emplace(key, [created](uint32_t gameObject)
            {
                created->Remove(gameObject);
            });

            return *created;
        }

        return *static_cast<ComponentPool<T>*>(iterator->second);
    }

    template <typename T>
    ComponentPool<T>* World::GetPoolPointer()
    {
        return &GetPool<T>();
    }
}
