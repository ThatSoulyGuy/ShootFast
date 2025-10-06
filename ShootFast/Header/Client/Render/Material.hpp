#ifndef SHOOTFAST_MATERIAL_HPP
#define SHOOTFAST_MATERIAL_HPP

namespace ShootFast::Client::Render
{
    struct ClientRenderContext;
    struct MaterialHandle;
    struct ShaderHandle;
    struct TextureHandle;

    class Material final
    {

    public:

        using Handle = MaterialHandle;

        ShaderHandle* shader = nullptr;
        TextureHandle* albedo = nullptr;

        static Handle Create(const ClientRenderContext&, const Material&);
        static void Destroy(ClientRenderContext&, Handle);
    };
}

#endif