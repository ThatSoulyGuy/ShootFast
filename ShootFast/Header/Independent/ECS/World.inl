#pragma once

#include <typeindex>
#include "Independent/ECS/ComponentPool.hpp"

namespace ShootFast::Independent::ECS
{
    template <typename T, typename... Args>
    T& World::Add(uint32_t gameObject, Args&&... args)
    {
        auto& pool = GetPool<T>();

        return pool.Emplace(gameObject, std::forward<Args>(args)...);
    }

    template <typename T>
    bool World::Has(uint32_t gameObject) const
    {
        const auto iterator = poolMap.find(typeid(T));

        if (iterator == poolMap.end())
            return false;

        const auto* model = static_cast<PoolModel<T>*>(iterator->second.get());

        return model->pool.Has(gameObject);
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

        if (result == nullptr)
        {
            static constexpr std::vector<uint32_t> EmptyOwners{};

            result = &EmptyOwners;
        }

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
            auto model = std::make_unique<PoolModel<T>>();
            auto* created = &model->pool;

            poolMap.emplace(key, std::move(model));

            return *created;
        }

        return static_cast<PoolModel<T>*>(iterator->second.get())->pool;
    }

    template <typename T>
    ComponentPool<T>* World::GetPoolPointer()
    {
        return &GetPool<T>();
    }
}
