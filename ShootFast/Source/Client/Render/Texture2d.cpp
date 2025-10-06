#include "Client/Render/Texture2d.hpp"
#include <stdexcept>
#include <glad/gl.h>
#include "Client/Render/ClientRenderContext.hpp"
#include "Client/Render/Handles.hpp"
#include "Client/Render/Registries.hpp"

namespace ShootFast::Client::Render
{
    Texture2d::Handle Texture2d::Upload(const ClientRenderContext& context, const Texture2d& texture2d)
    {
        if (texture2d.width <= 0 || texture2d.height <= 0)
            throw std::runtime_error("Texture2D: invalid size");

        if (texture2d.pixels.empty())
            throw std::runtime_error("Texture2D: empty pixel data");

        GLenum internalFormat = GL_RGBA8;
        GLenum dataFormat = GL_RGBA;

        if (texture2d.format == TextureFormat::RGB8)
        {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }

        if (texture2d.sRGB)
        {
            if (texture2d.format == TextureFormat::RGBA8)
                internalFormat = GL_SRGB8_ALPHA8;
            else
                internalFormat = GL_SRGB8;
        }

        GLuint textureId = 0u;

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture2d.generateMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), texture2d.width, texture2d.height, 0, dataFormat, GL_UNSIGNED_BYTE, texture2d.pixels.data());

        if (texture2d.generateMips)
            glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        return context.textureRegistry->Create(textureId);
    }

    void Texture2d::Destroy(const ClientRenderContext& context, const Handle handle)
    {
        auto& [textureId] = context.textureRegistry->Get(handle);

        if (textureId != 0u)
        {
            glDeleteTextures(1, &textureId);
            textureId = 0u;
        }
    }
}
