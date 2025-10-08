#ifndef SHOOTFAST_SHADER_HPP
#define SHOOTFAST_SHADER_HPP

#include "Independent/Utility/AssetPath.hpp"

namespace ShootFast::Client::Render
{
    struct ClientRenderContext;
    struct ShaderHandle;

    class Shader final
    {

    public:

        using Handle = ShaderHandle;

        Independent::Utility::AssetPath vertexPath;
        Independent::Utility::AssetPath fragmentPath;

        static Handle Create(const ClientRenderContext&, const Shader&);
        static void Destroy(const ClientRenderContext&, Handle);
    };
}

#endif