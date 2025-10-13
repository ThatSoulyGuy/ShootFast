#ifndef SHOOTFAST_COMPONENT_POOL_HPP
#define SHOOTFAST_COMPONENT_POOL_HPP

#include <cstdint>
#include <vector>
#include <limits>
#include <stdexcept>
#include <utility>

namespace ShootFast::Independent::ECS
{
    template <typename T>
    class ComponentPool final
    {

    public:

        using ValueType = T;

        template <typename... Args>
        T& Emplace(const uint32_t gameObject, Args&&... args)
        {
            if (gameObject >= sparseList.size())
                sparseList.resize(static_cast<std::size_t>(gameObject) + 1u, NoPosition);

            if (sparseList[gameObject] != NoPosition)
            {
                denseList[sparseList[gameObject]] = T(std::forward<Args>(args)...);
                return denseList[sparseList[gameObject]];
            }

            const std::size_t index = denseList.size();

            denseList.emplace_back(std::forward<Args>(args)...);
            ownerList.push_back(gameObject);
            sparseList[gameObject] = index;

            return denseList[index];
        }

        [[nodiscard]]
        bool Has(const uint32_t gameObject) const
        {
            return gameObject < sparseList.size() && sparseList[gameObject] != NoPosition;
        }

        T& Get(const uint32_t gameObject)
        {
            if (!Has(gameObject))
                throw std::out_of_range("ComponentPool::Get: missing component for gameObject");

            return denseList[sparseList[gameObject]];
        }

        const T& Get(const uint32_t gameObject) const
        {
            if (!Has(gameObject))
                throw std::out_of_range("ComponentPool::Get: missing component for gameObject");

            return denseList[sparseList[gameObject]];
        }

        void Remove(const uint32_t gameObject)
        {
            if (!Has(gameObject))
                return;

            const std::size_t index = sparseList[gameObject];
            const std::size_t last = denseList.size() - 1u;

            std::swap(denseList[index], denseList[last]);
            std::swap(ownerList[index], ownerList[last]);

            sparseList[ownerList[index]] = index;

            denseList.pop_back();
            ownerList.pop_back();
            sparseList[gameObject] = NoPosition;
        }

        [[nodiscard]]
        const std::vector<uint32_t>& GetOwnerList() const
        {
            return ownerList;
        }

        std::vector<T>& GetDenseList()
        {
            return denseList;
        }

        const std::vector<T>& GetDenseList() const
        {
            return denseList;
        }

    private:

        static constexpr std::size_t NoPosition = std::numeric_limits<std::size_t>::max();

        std::vector<T> denseList;
        std::vector<uint32_t> ownerList;
        std::vector<std::size_t> sparseList;
    };
}

#endif