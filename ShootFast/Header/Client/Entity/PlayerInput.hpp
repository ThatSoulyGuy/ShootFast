#ifndef SHOOTFAST_PLAYER_INPUT_HPP
#define SHOOTFAST_PLAYER_INPUT_HPP

#include <glm/vec2.hpp>

namespace ShootFast::Client::Entity
{
    class PlayerInput final
    {

    public:

        glm::vec2 moveAxis{ 0.0f, 0.0f };
        bool run{ false };
        glm::vec2 lookDelta{ 0.0f, 0.0f };
    };
}

#endif