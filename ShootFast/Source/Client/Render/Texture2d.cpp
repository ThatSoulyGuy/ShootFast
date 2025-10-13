#include "Client/Render/Texture2d.hpp"
#include <stdexcept>
#include <glad/gl.h>
#ifdef SHOOTFAST_HAVE_FREEIMAGE
#include <FreeImage.h>
#endif
#include "Client/Render/ClientRenderContext.hpp"
#include "Client/Render/Handles.hpp"
#include "Client/Render/Registries.hpp"
#include "Independent/Utility/AssetPath.hpp"

#ifdef SHOOTFAST_HAVE_FREEIMAGE
namespace
{
    class FreeImageGuard
    {

    public:

        FreeImageGuard()
        {
            FreeImage_Initialise(FALSE);
        }

        ~FreeImageGuard()
        {
            FreeImage_DeInitialise();
        }
    };

    void EnsureFreeImageInitialized()
    {
        static FreeImageGuard guard{};
    }

    FIBITMAP* LoadFIBitmapFromFile(const std::string& filePath)
    {
        FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filePath.c_str(), 0);

        if (format == FIF_UNKNOWN)
            format = FreeImage_GetFIFFromFilename(filePath.c_str());

        if (format == FIF_UNKNOWN)
            throw std::runtime_error("FreeImage: unknown file format for " + filePath);

        if (!FreeImage_FIFSupportsReading(format))
            throw std::runtime_error("FreeImage: format does not support reading for " + filePath);

        FIBITMAP* result = FreeImage_Load(format, filePath.c_str());

        if (result == nullptr)
            throw std::runtime_error("FreeImage: failed to load " + filePath);

        return result;
    }

    ShootFast::Client::Render::Texture2d ConvertFIBitmapToTexture(FIBITMAP* bitmap, const bool forceRgba, const bool flipVertical, const bool generateMips, const bool sRgb)
    {
        if (flipVertical)
            FreeImage_FlipVertical(bitmap);

        ShootFast::Client::Render::Texture2d texture{};

        texture.generateMips = generateMips;
        texture.sRGB = sRgb;

        const unsigned int sourceBitsPerPixel = FreeImage_GetBPP(bitmap);

        FIBITMAP* converted = nullptr;

        if (forceRgba)
        {
            if (sourceBitsPerPixel != 32)
            {
                converted = FreeImage_ConvertTo32Bits(bitmap);

                if (converted == nullptr)
                    throw std::runtime_error("FreeImage: ConvertTo32Bits failed.");

                FreeImage_Unload(bitmap);

                bitmap = converted;
            }

            texture.format = ShootFast::Client::Render::TextureFormat::RGBA8;
        }
        else
        {
            if (sourceBitsPerPixel == 32)
                texture.format = ShootFast::Client::Render::TextureFormat::RGBA8;
            else
            {
                if (sourceBitsPerPixel != 24)
                {
                    converted = FreeImage_ConvertTo24Bits(bitmap);

                    if (converted == nullptr)
                        throw std::runtime_error("FreeImage: ConvertTo24Bits failed.");

                    FreeImage_Unload(bitmap);

                    bitmap = converted;
                }

                texture.format = ShootFast::Client::Render::TextureFormat::RGB8;
            }
        }

        const int width = static_cast<int>(FreeImage_GetWidth(bitmap));
        const int height = static_cast<int>(FreeImage_GetHeight(bitmap));

        const int bytesPerPixel = texture.format == ShootFast::Client::Render::TextureFormat::RGBA8 ? 4 : 3;

        const unsigned pitch = FreeImage_GetPitch(bitmap);
        const BYTE* bits = FreeImage_GetBits(bitmap);

        if (bits == nullptr || width <= 0 || height <= 0)
        {
            FreeImage_Unload(bitmap);
            throw std::runtime_error("FreeImage: invalid bitmap data.");
        }

        texture.width = width;
        texture.height = height;
        texture.pixels.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * static_cast<std::size_t>(bytesPerPixel));

        for (int y = 0; y < height; ++y)
        {
            const BYTE* srcRow = bits + static_cast<std::size_t>(y) * static_cast<std::size_t>(pitch);

            for (int x = 0; x < width; ++x)
            {
                const std::size_t srcIndex = static_cast<std::size_t>(x) * static_cast<std::size_t>(sourceBitsPerPixel / 8);

                const BYTE b = srcRow[srcIndex + 0];
                const BYTE g = srcRow[srcIndex + 1];
                const BYTE r = srcRow[srcIndex + 2];

                const std::size_t dstIndex = (static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)) * static_cast<std::size_t>(bytesPerPixel);

                texture.pixels[dstIndex + 0] = r;
                texture.pixels[dstIndex + 1] = g;
                texture.pixels[dstIndex + 2] = b;

                if (bytesPerPixel == 4)
                {
                    const BYTE a = (sourceBitsPerPixel == 32) ? srcRow[srcIndex + 3] : static_cast<BYTE>(255);
                    texture.pixels[dstIndex + 3] = a;
                }
            }
        }

        FreeImage_Unload(bitmap);

        return texture;
    }
}
#endif

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

    Texture2d Texture2d::LoadFromFileUsingFreeImage(const Independent::Utility::AssetPath& filePath, const bool forceRgba, const bool flipVertical, const bool generateMips, bool sRgb)
    {
#ifndef SHOOTFAST_HAVE_FREEIMAGE
        throw std::runtime_error("Texture2d::LoadFromFileUsingFreeImage called but SHOOTFAST_HAVE_FREEIMAGE is not defined.");
#else
        EnsureFreeImageInitialized();

        FIBITMAP* bitmap = LoadFIBitmapFromFile(filePath.GetFullPath());

        return ConvertFIBitmapToTexture(bitmap, forceRgba, flipVertical, generateMips, bitmap);
#endif
    }

    Texture2d::Handle Texture2d::LoadAndUploadUsingFreeImage(const ClientRenderContext& context, const Independent::Utility::AssetPath& filePath, const bool forceRgba, const bool flipVertical, const bool generateMips, const bool sRgb)
    {
#ifndef SHOOTFAST_HAVE_FREEIMAGE
        throw std::runtime_error("Texture2d::LoadAndUploadUsingFreeImage called but SHOOTFAST_HAVE_FREEIMAGE is not defined.");
#else
        return Upload(context, LoadFromFileUsingFreeImage(filePath, forceRgba, flipVertical, generateMips, sRgb));
#endif
    }
}
