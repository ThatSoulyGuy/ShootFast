#ifndef SHOOTFAST_TEAM_HPP
#define SHOOTFAST_TEAM_HPP

#include <cstdint>

namespace ShootFast::Client::Entity
{
    enum class Team : std::uint8_t
    {
        RED = 0,
        BLUE = 1,
        NONE = 2
    };
}

#endif