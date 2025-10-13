#ifndef SHOOTFAST_CAMERA_FOLLOW_SYSTEM_HPP
#define SHOOTFAST_CAMERA_FOLLOW_SYSTEM_HPP

namespace ShootFast::Independent::ECS
{
    class World;
}

namespace ShootFast::Client::Entity::Systems
{
    class CameraFollowSystem final
    {

    public:

        static void Run(Independent::ECS::World& world);

    };
}

#endif