#ifndef SHOOTFAST_HEALTH_HPP
#define SHOOTFAST_HEALTH_HPP

#include <cstdint>

namespace ShootFast::Client::Entity
{
    class Health final
    {

    public:

        std::uint8_t current{ 100 };
        std::uint8_t maximum{ 100 };
    };
}

#endif