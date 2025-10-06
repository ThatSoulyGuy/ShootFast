#ifndef SHOOTFAST_WORLD_HPP
#define SHOOTFAST_WORLD_HPP

#include <cstdint>
#include <functional>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include "Independent/ECS/ComponentPool.hpp"

namespace ShootFast::Independent::ECS
{
    template <typename>
    class ComponentPool;

    class GameObjectAllocator;

    class World final
    {

    public:

        World();
        ~World();

        uint32_t CreateGameObject() const;

        void DestroyGameObject(uint32_t gameObject);

        template <typename T>
        T& Add(uint32_t gameObject, const T& value);

        template <typename T>
        bool Has(uint32_t gameObject) const;

        template <typename T>
        T& Get(uint32_t gameObject);

        template <typename T>
        void Remove(uint32_t gameObject);

        template <typename... T>
        class ViewRange
        {

        public:

            struct Iterator
            {
                World* world{nullptr};
                const std::vector<uint32_t>* owners{nullptr};
                std::size_t index{0};

                void Advance();

                [[nodiscard]]
                bool Matches(const uint32_t gameObject) const
                {
                    return (world->Has<T>(gameObject) && ...);
                }

                bool operator!=(const Iterator& other) const
                {
                    return index != other.index;
                }

                void operator++()
                {
                    index += 1;

                    Advance();
                }

                auto operator*() const
                {
                    uint32_t gameObject = (*owners)[index];

                    return std::tuple<uint32_t, T&...>(gameObject, world->Get<T>(gameObject)...);
                }
            };

            ViewRange(World* world, const std::vector<uint32_t>* object) : world(world), owners(object) { }

            Iterator begin() const;

            Iterator end() const;

        private:

            World* world;
            const std::vector<uint32_t>* owners;

        };

        template <typename... T>
        ViewRange<T...> View();

    private:

        template <typename T>
        ComponentPool<T>& GetPool();

        template <typename T>
        ComponentPool<T>* GetPoolPointer();

        template <typename Head>
        ComponentPool<Head>* SelectSmallestPool();

        template <typename Head, typename Next, typename... Rest>
        ComponentPool<Head>* SelectSmallestPool();

        GameObjectAllocator* allocator = nullptr;

        std::unordered_map<std::type_index, void*> poolMap;
        std::unordered_map<std::type_index, std::function<void(void*)>> destructorMap;
        std::unordered_map<std::type_index, std::function<void(uint32_t)>> eraserMap;
    };
}

#include "World.inl"

#endif