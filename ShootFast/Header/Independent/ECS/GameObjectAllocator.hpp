#ifndef SHOOTFAST_GAME_OBJECT_ALLOCATOR_HPP
#define SHOOTFAST_GAME_OBJECT_ALLOCATOR_HPP

#include <cstdint>
#include <vector>

namespace ShootFast::Independent::ECS
{
    class GameObjectAllocator final
    {

    public:

        uint32_t Create();
        void Destroy(uint32_t gameObject);

    private:

        uint32_t nextId{1u};
        std::vector<uint32_t> freeList;

    };
}

#endif