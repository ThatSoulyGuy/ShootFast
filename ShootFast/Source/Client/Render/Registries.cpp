#include "Client/Render/Registries.hpp"
#include <cstdint>
#include <stdexcept>
#include "Client/Render/Handles.hpp"

namespace ShootFast::Client::Render
{
    MeshRegistry::MeshRegistry() : meshList{MeshGPU{}} { }

    MeshHandle MeshRegistry::Create(const MeshGPU& gpu)
    {
        const MeshHandle handle{static_cast<std::uint32_t>(meshList.size())};

        meshList.push_back(gpu);

        return handle;
    }

    MeshGPU& MeshRegistry::Get(const MeshHandle handle)
    {
        if (handle.id >= meshList.size())
            throw std::out_of_range("MeshRegistry::Get invalid handle");

        return meshList[handle.id];
    }

    const MeshGPU& MeshRegistry::Get(const MeshHandle handle) const
    {
        if (handle.id >= meshList.size())
            throw std::out_of_range("MeshRegistry::Get invalid handle");

        return meshList[handle.id];
    }

    ShaderRegistry::ShaderRegistry() : shaderProgramList{ShaderProgram{}} { }

    ShaderHandle ShaderRegistry::Create(const GLuint programId)
    {
        const ShaderHandle handle{static_cast<std::uint32_t>(shaderProgramList.size())};

        shaderProgramList.push_back(ShaderProgram{.programId = programId});

        return handle;
    }

    ShaderProgram& ShaderRegistry::Get(const ShaderHandle handle)
    {
        if (handle.id >= shaderProgramList.size())
            throw std::out_of_range("ShaderRegistry::Get invalid handle");

        return shaderProgramList[handle.id];
    }

    const ShaderProgram& ShaderRegistry::Get(const ShaderHandle handle) const
    {
        if (handle.id >= shaderProgramList.size())
            throw std::out_of_range("ShaderRegistry::Get invalid handle");

        return shaderProgramList[handle.id];
    }

    TextureRegistry::TextureRegistry() : textureList{TextureGPU{}} { }

    TextureHandle TextureRegistry::Create(const GLuint textureId)
    {
        const TextureHandle handle{static_cast<std::uint32_t>(textureList.size())};

        textureList.push_back(TextureGPU{textureId});

        return handle;
    }

    TextureGPU& TextureRegistry::Get(const TextureHandle handle)
    {
        if (handle.id >= textureList.size())
            throw std::out_of_range("TextureRegistry::Get invalid handle");

        return textureList[handle.id];
    }

    const TextureGPU& TextureRegistry::Get(const TextureHandle handle) const
    {
        if (handle.id >= textureList.size())
            throw std::out_of_range("TextureRegistry::Get invalid handle");

        return textureList[handle.id];
    }

    MaterialRegistry::MaterialRegistry() : materialList{MaterialGPU{}} { }

    MaterialHandle MaterialRegistry::Create(const MaterialGPU& material)
    {
        const MaterialHandle handle{static_cast<std::uint32_t>(materialList.size())};

        materialList.push_back(material);

        return handle;
    }

    MaterialGPU& MaterialRegistry::Get(const MaterialHandle handle)
    {
        if (handle.id >= materialList.size())
            throw std::out_of_range("MaterialRegistry::Get invalid handle");

        return materialList[handle.id];
    }

    const MaterialGPU& MaterialRegistry::Get(const MaterialHandle handle) const
    {
        if (handle.id >= materialList.size())
            throw std::out_of_range("MaterialRegistry::Get invalid handle");

        return materialList[handle.id];
    }
}
