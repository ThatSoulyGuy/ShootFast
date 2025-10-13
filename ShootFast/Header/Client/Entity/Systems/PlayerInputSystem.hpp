#ifndef SHOOTFAST_PLAYER_INPUT_SYSTEM_HPP
#define SHOOTFAST_PLAYER_INPUT_SYSTEM_HPP

namespace ShootFast::Independent::ECS
{
    class World;
}

namespace ShootFast::Client::Entity::Systems
{
    class PlayerInputSystem final
    {

    public:

        explicit PlayerInputSystem() { }

        static void Run(Independent::ECS::World& world);

    };
}

#endif