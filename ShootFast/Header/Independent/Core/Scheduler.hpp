#ifndef SHOOTFAST_SCHEDULER_HPP
#define SHOOTFAST_SCHEDULER_HPP

#include <functional>
#include <string>

namespace ShootFast::Client::Render
{
    struct ClientRenderContext;
}

namespace ShootFast::Independent::ECS
{
    class World;
}

namespace ShootFast::Independent::Core
{
    class FrameContext;

    enum class Phase : int
    {
        PRE_UPDATE = 0,
        UPDATE = 1,
        POST_UPDATE = 2,
        RENDER_PREP = 3,
        RENDER = 4
    };

    struct SystemNode final
    {
        std::string name;

        Phase phase;

        std::vector<std::string> after;
        std::function<void(ECS::World&, Client::Render::ClientRenderContext&, const FrameContext&)> run;
    };

    class Scheduler final
    {

    public:

        void Add(SystemNode node);
        void Run(ECS::World& world, Client::Render::ClientRenderContext& engine, const FrameContext& frame) const;

    private:

        [[nodiscard]]
        std::vector<const SystemNode*> OrderPhase(Phase phase) const;

        std::vector<SystemNode> nodeList;
    };
}

#endif