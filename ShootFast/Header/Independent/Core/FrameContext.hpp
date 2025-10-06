#ifndef SHOOTFAST_FRAME_CONTEXT_HPP
#define SHOOTFAST_FRAME_CONTEXT_HPP

#include <cstdint>

namespace ShootFast::Independent::Core
{
    struct FrameContext final
    {
        float deltaSeconds{0.0f};

        std::uint64_t frameIndex{0u};
    };
}

#endif