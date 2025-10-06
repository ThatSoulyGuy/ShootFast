#ifndef SHOOTFAST_HANDLES_HPP
#define SHOOTFAST_HANDLES_HPP

#include <cstdint>

namespace ShootFast::Client::Render
{
    struct MeshHandle final
    {
        uint32_t id{0u};

        explicit operator bool() const
        {
            return id != 0u;
        }

        bool operator==(const MeshHandle& other) const
        {
            return id == other.id;
        }

        bool operator!=(const MeshHandle& other) const
        {
            return id != other.id;
        }
    };

    struct ShaderHandle final
    {
        uint32_t id{0u};

        explicit operator bool() const
        {
            return id != 0u;
        }

        bool operator==(const ShaderHandle& other) const
        {
            return id == other.id;
        }

        bool operator!=(const ShaderHandle& other) const
        {
            return id != other.id;
        }
    };

    struct TextureHandle final
    {
        uint32_t id{0u};

        explicit operator bool() const
        {
            return id != 0u;
        }

        bool operator==(const TextureHandle& other) const
        {
            return id == other.id;
        }

        bool operator!=(const TextureHandle& other) const
        {
            return id != other.id;
        }
    };

    struct MaterialHandle final
    {
        uint32_t id{0u};

        explicit operator bool() const
        {
            return id != 0u;
        }

        bool operator==(const MaterialHandle& other) const
        {
            return id == other.id;
        }

        bool operator!=(const MaterialHandle& other) const
        {
            return id != other.id;
        }
    };
}

#endif