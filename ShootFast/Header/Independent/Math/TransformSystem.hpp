#ifndef SHOOTFAST_TRANSFORM_SYSTEM_HPP
#define SHOOTFAST_TRANSFORM_SYSTEM_HPP

namespace ShootFast::Independent::ECS
{
    class World;
}

namespace ShootFast::Independent::Math
{
    class TransformSystem final
    {

    public:

        static constexpr auto Name = "TransformSystem";

        void Run(ECS::World& world) const;
    };
}

#endif