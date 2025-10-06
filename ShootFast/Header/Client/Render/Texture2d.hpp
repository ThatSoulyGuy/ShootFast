#ifndef SHOOTFAST_TEXTURE2D_HPP
#define SHOOTFAST_TEXTURE2D_HPP

#include <cstdint>
#include <vector>

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
    };
}

#endif