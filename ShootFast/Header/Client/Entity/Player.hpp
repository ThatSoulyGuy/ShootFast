#ifndef SHOOTFAST_PLAYER_HPP
#define SHOOTFAST_PLAYER_HPP

#include "Client/Entity/Team.hpp"

namespace ShootFast::Client::Entity
{
    class Player final
    {

    public:

        Team team{ Team::NONE };

        std::uint32_t cameraHandle{0};

        float moveSpeed{ 5.0f };
        float runMultiplier{ 1.8f };
        float jumpHeight{ 0.0f };
    };
}

#endif