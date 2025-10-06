#ifndef SHOOTFAST_SHADER_HPP
#define SHOOTFAST_SHADER_HPP

#include <string>

namespace ShootFast::Client::Render
{
    struct ClientRenderContext;
    struct ShaderHandle;

    class Shader final
    {

    public:

        using Handle = ShaderHandle;

        std::string vertexSource;
        std::string fragmentSource;

        static Handle Create(const ClientRenderContext&, const Shader&);
        static void Destroy(const ClientRenderContext&, Handle);
    };
}

#endif