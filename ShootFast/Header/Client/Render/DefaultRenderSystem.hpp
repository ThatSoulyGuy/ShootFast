#ifndef SHOOTFAST_RENDER_SYSTEM_HPP
#define SHOOTFAST_RENDER_SYSTEM_HPP

namespace ShootFast::Independent::ECS
{
    class World;
}

namespace ShootFast::Client::Render
{
    struct ClientRenderContext;

    class DefaultRenderSystem final
    {

    public:

        static constexpr auto Name = "RenderSystem";

        explicit DefaultRenderSystem(ClientRenderContext& context) : context(context) { }

        void Run(Independent::ECS::World& world) const;

    private:

        ClientRenderContext& context;

    };
}

#endif