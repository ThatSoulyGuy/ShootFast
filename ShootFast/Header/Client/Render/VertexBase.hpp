#ifndef SHOOTFAST_VERTEX_HPP
#define SHOOTFAST_VERTEX_HPP

#include <cstddef>
#include "Client/Utility/GLFW.hpp"

namespace ShootFast::Client::Render
{
    class VertexBase
    {

    public:

        ~VertexBase() = default;

    };

    struct VertexAttribute
    {
        GLuint index;
        GLint components;
        GLenum type;
        GLboolean normalized;

        std::size_t offset;
    };

    enum class VertexSemantic : uint8_t
    {
        POSITION = 0,
        NORMAL = 1,
        TANGENT = 2,
        BITANGENT = 3,
        TEXCOORD0 = 4,
        COLOR0 = 5
    };

    template <typename T>
    concept VertexType = requires
    {
        { T::STRIDE } -> std::convertible_to<GLsizei>;
        { T::GetAttributes() } -> std::ranges::forward_range;
    } && std::same_as<std::remove_cvref_t<std::ranges::range_value_t<decltype(T::GetAttributes())>>, VertexAttribute>;
}

#endif