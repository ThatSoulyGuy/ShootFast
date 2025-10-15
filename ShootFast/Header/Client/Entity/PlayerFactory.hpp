#ifndef SHOOTFAST_PLAYER_HANDLES_HPP
#define SHOOTFAST_PLAYER_HANDLES_HPP

#include <cstdint>
#include <glm/vec3.hpp>

namespace ShootFast::Client::Render
{
    struct ClientRenderContext;
}

namespace ShootFast::Independent::ECS
{
    class World;
}

namespace ShootFast::Client::Entity
{
    struct PlayerFactory final
    {
        std::uint32_t player{};
        std::uint32_t camera{};

        static PlayerFactory CreateLocalPlayer(Render::ClientRenderContext& context, Independent::ECS::World& world, bool isRemote, const glm::vec3& startPosition);
    };
}

#endif