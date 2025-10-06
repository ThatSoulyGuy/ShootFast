#include "Independent/Core/Scheduler.hpp"

#include <queue>

namespace ShootFast::Independent::Core
{
    void Scheduler::Add(SystemNode node)
    {
        nodeList.push_back(std::move(node));
    }

    std::vector<const SystemNode*> Scheduler::OrderPhase(const Phase phase) const
    {
        std::vector<const SystemNode*> phaseNodeList;

        for (const auto& node : nodeList)
        {
            if (node.phase == phase)
                phaseNodeList.push_back(&node);
        }

        std::unordered_map<std::string, const SystemNode*> byName;

        for (const auto* node : phaseNodeList)
            byName.emplace(node->name, node);

        std::unordered_map<const SystemNode*, int> indegree;
        std::unordered_map<const SystemNode*, std::vector<const SystemNode*>> edgeMap;

        for (const auto* node : phaseNodeList)
            indegree[node] = 0;

        for (const auto* node : phaseNodeList)
        {
            for (const auto& dep : node->after)
            {
                if (auto iterator = byName.find(dep); iterator != byName.end())
                {
                    edgeMap[iterator->second].push_back(node);
                    indegree[node] += 1;
                }
            }
        }

        std::queue<const SystemNode*> nodeQueue;

        for (const auto& [nodePointer, deg] : indegree)
        {
            if (deg == 0)
                nodeQueue.push(nodePointer);
        }

        std::vector<const SystemNode*> ordered;

        while (!nodeQueue.empty())
        {
            const SystemNode* node = nodeQueue.front();

            nodeQueue.pop();
            ordered.push_back(node);

            for (const auto* edgeNode : edgeMap[node])
            {
                indegree[edgeNode] -= 1;

                if (indegree[edgeNode] == 0)
                    nodeQueue.push(edgeNode);
            }
        }

        if (ordered.size() != phaseNodeList.size())
            return phaseNodeList;

        return ordered;
    }

    void Scheduler::Run(ECS::World& world, Client::Render::ClientRenderContext& engine, const FrameContext& frame) const
    {
        for (int phase = static_cast<int>(Phase::PRE_UPDATE); phase <= static_cast<int>(Phase::RENDER); ++phase)
        {
            for (auto ordered = OrderPhase(static_cast<Phase>(phase)); const auto* node : ordered)
                node->run(world, engine, frame);
        }
    }
}
