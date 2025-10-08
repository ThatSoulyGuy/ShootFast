#ifndef SHOOTFAST_TEXTURE2D_HPP
#define SHOOTFAST_TEXTURE2D_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace ShootFast::Independent::Utility
{
    class AssetPath;
}

namespace ShootFast::Client::Render
{
    struct TextureHandle;
    struct ClientRenderContext;

    enum class TextureFormat : uint8_t
    {
        RGBA8 = 0,
        RGB8 = 1
    };

    class Texture2d final
    {

    public:

        using Handle = TextureHandle;

        std::vector<std::uint8_t> pixels;
        TextureFormat format{TextureFormat::RGBA8};

        int width{0};
        int height{0};
        bool generateMips{true};
        bool sRGB{false};

        static Handle Upload(const ClientRenderContext&, const Texture2d&);
        static void Destroy(const ClientRenderContext&, Handle);

        static Texture2d LoadFromFileUsingFreeImage(const Independent::Utility::AssetPath& filePath, bool forceRgba, bool flipVertical, bool generateMips, bool sRgb);
        static Handle LoadAndUploadUsingFreeImage(const ClientRenderContext& context, const Independent::Utility::AssetPath& filePath, bool forceRgba, bool flipVertical, bool generateMips, bool sRgb);
    };
}

#endif