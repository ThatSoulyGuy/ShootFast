#ifndef SHOOTFAST_CLIENT_RENDER_CONTEXT_HPP
#define SHOOTFAST_CLIENT_RENDER_CONTEXT_HPP

#include <cstdint>

namespace ShootFast::Client::Render
{
    class ShaderRegistry;
    class TextureRegistry;
    class MeshRegistry;
    class MaterialRegistry;

    struct ClientRenderContext final
    {
        MeshRegistry* meshRegistry{nullptr};
        MaterialRegistry* materialRegistry{nullptr};
        ShaderRegistry* shaderRegistry{nullptr};
        TextureRegistry* textureRegistry{nullptr};
        std::uint32_t cameraHandle;
    };
}

#endif