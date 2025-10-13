#ifndef SHOOTFAST_CHARACTER_MOTOR_SYSTEM_HPP
#define SHOOTFAST_CHARACTER_MOTOR_SYSTEM_HPP

namespace ShootFast::Independent::ECS
{
    class World;
}

namespace ShootFast::Client::Entity::Systems
{
    class CharacterMotorSystem final
    {

    public:

        explicit CharacterMotorSystem(const float deltaSeconds) : deltaSeconds(deltaSeconds) { }
        void Run(Independent::ECS::World& world) const;

    private:

        float deltaSeconds{ 1.0f / 60.0f };

    };
}

#endif