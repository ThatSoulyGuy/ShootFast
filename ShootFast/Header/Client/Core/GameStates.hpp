#ifndef SHOOTFAST_CLIENT_CORE_GAMESTATES_HPP
#define SHOOTFAST_CLIENT_CORE_GAMESTATES_HPP

#include "Client/Core/GameState.hpp"

namespace ShootFast::Client::Core::States
{
    class ConnectingState final : public GameState
    {
    public:
        void Initialize(ClientApplication& application) override;
        void Update(ClientApplication& application, float deltaSeconds) override;
        void Render(ClientApplication& application) override;
    };

    class GameplayState final : public GameState
    {
    public:
        void Initialize(ClientApplication& application) override;
        void Update(ClientApplication& application, float deltaSeconds) override;
        void Render(ClientApplication& application) override;
    };

    class DisconnectedState final : public GameState
    {
    public:
        void Initialize(ClientApplication& application) override;
        void Update(ClientApplication& application, float deltaSeconds) override;
        void Render(ClientApplication& application) override;
    };
}

#endif
