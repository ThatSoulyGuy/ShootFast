#ifndef SHOOTFAST_HIERARCHY_HPP
#define SHOOTFAST_HIERARCHY_HPP

#include <cstdint>
#include <vector>

namespace ShootFast::Independent::ECS
{
    class World;

    struct Parent final
    {
        uint32_t value{};
    };

    struct Children final
    {
        std::vector<uint32_t> value;
    };

    void SetParent(World& world, uint32_t child, uint32_t parent);
    void ClearParent(World& world, uint32_t child);
}

#endif //SHOOTFAST_HIERARCHY_HPP