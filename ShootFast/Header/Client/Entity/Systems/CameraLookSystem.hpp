#ifndef SHOOTFAST_CAMERA_LOOK_SYSTEM_HPP
#define SHOOTFAST_CAMERA_LOOK_SYSTEM_HPP

#include "Independent/ECS/World.hpp"

namespace ShootFast::Client::Entity::Systems
{
    class CameraLookSystem
    {

    public:

        explicit CameraLookSystem(const float sensitivity = 0.12f, const bool invertY = false) : sensitivity(sensitivity), invertY(invertY) { }

        void Run(Independent::ECS::World& world) const;

    private:

        float sensitivity;
        bool invertY;
    };
}

#endif