#ifndef SHOOTFAST_REGISTRIES_HPP
#define SHOOTFAST_REGISTRIES_HPP

#include <vector>
#include "Client/Utility/GLFW.hpp"

namespace ShootFast::Client::Render
{
    struct MeshHandle;
    struct ShaderHandle;
    struct TextureHandle;
    struct MaterialHandle;

    struct MeshGPU final
    {
        GLuint vao{0u};
        GLuint vbo{0u};
        GLuint ebo{0u};

        GLsizei indexCount{0};
    };

    struct ShaderProgram final
    {
        GLuint programId{0u};
    };

    struct TextureGPU final
    {
        GLuint textureId{0u};
    };

    struct MaterialGPU final
    {
        ShaderHandle* shader;
        TextureHandle* albedo;
    };

    class MeshRegistry final
    {

    public:

        MeshRegistry();

        MeshHandle Create(const MeshGPU& gpu);
        MeshGPU& Get(MeshHandle handle);

        [[nodiscard]]
        const MeshGPU& Get(MeshHandle handle) const;

    private:

        std::vector<MeshGPU> meshList;
    };

    class ShaderRegistry final
    {

    public:

        ShaderRegistry();

        ShaderHandle Create(GLuint programId);
        ShaderProgram& Get(ShaderHandle handle);

        [[nodiscard]]
        const ShaderProgram& Get(ShaderHandle handle) const;

    private:

        std::vector<ShaderProgram> shaderProgramList;
    };

    class TextureRegistry final
    {

    public:

        TextureRegistry();

        TextureHandle Create(GLuint textureId);
        TextureGPU& Get(TextureHandle handle);

        [[nodiscard]]
        const TextureGPU& Get(TextureHandle handle) const;

    private:

        std::vector<TextureGPU> textureList;
    };

    class MaterialRegistry final
    {

    public:

        MaterialRegistry();

        MaterialHandle Create(const MaterialGPU& material);
        MaterialGPU& Get(MaterialHandle handle);

        [[nodiscard]]
        const MaterialGPU& Get(MaterialHandle handle) const;

    private:

        std::vector<MaterialGPU> materialList;
    };
}

#endif