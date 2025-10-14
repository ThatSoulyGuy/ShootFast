#ifndef SHOOTFAST_TRANSFORM_SYNC_SYSTEM_HPP
#define SHOOTFAST_TRANSFORM_SYNC_SYSTEM_HPP

#include <algorithm>
#include <cstdint>
#include <vector>

namespace ShootFast::Independent::ECS
{
    class World;
}

namespace ShootFast::Independent::Network
{
    class TransformSyncSystem final
    {

    public:

        explicit TransformSyncSystem(const float sendHz = 20.f) : intervalSeconds(1.f / sendHz) { }
        void Run(ECS::World& world, float deltaSeconds);

        void AddObject(const uint32_t object)
        {
            syncObjectList.push_back(object);
        }

        bool HasObject(const uint32_t object)
        {
            return std::ranges::find(syncObjectList, object) != syncObjectList.end();
        }

        void RemoveObject(const uint32_t object)
        {
            syncObjectList.erase(std::ranges::remove(syncObjectList, object).begin(), syncObjectList.end());
        }

    private:

        std::vector<std::uint32_t> syncObjectList;

        float intervalSeconds{0.05f};
        float accumulator{0.f};
    };
}

#endif