#ifndef SHOOTFAST_CLIENT_CORE_GAMESTATE_HPP
#define SHOOTFAST_CLIENT_CORE_GAMESTATE_HPP

namespace ShootFast::Client
{
    class ClientApplication;
}

namespace ShootFast::Client::Core
{
    class GameState
    {
    public:
        virtual ~GameState() = default;

        virtual void Initialize(ClientApplication& application) = 0;
        virtual void Update(ClientApplication& application, float deltaSeconds) = 0;
        virtual void Render(ClientApplication& application) = 0;
    };
}

#endif
