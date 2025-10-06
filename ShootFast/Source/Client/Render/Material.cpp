#include "Client/Render/Material.hpp"
#include <stdexcept>
#include "Client/Render/ClientRenderContext.hpp"
#include "Client/Render/Handles.hpp"
#include "Client/Render/Registries.hpp"

namespace ShootFast::Client::Render
{
    Material::Handle Material::Create(const ClientRenderContext& context, const Material& material)
    {
        if (!material.shader)
            throw std::runtime_error("MaterialSpec: shader handle is invalid");

        return context.materialRegistry->Create({.shader = material.shader, .albedo = material.albedo});
    }

    void Material::Destroy(ClientRenderContext&, Handle) { }
}