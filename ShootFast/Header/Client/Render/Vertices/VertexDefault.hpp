#ifndef SHOOTFAST_VERTEX_DEFAULT_HPP
#define SHOOTFAST_VERTEX_DEFAULT_HPP

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "Client/Render/VertexBase.hpp"

namespace ShootFast::Client::Render::Vertices
{
    class VertexDefault final : public VertexBase
    {

    public:

        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 normal;
        glm::vec2 uvs;

        static constexpr GLsizei STRIDE = 44;

        static constexpr std::array<VertexAttribute, 4> GetAttributes()
        {
            return
            {
                VertexAttribute{ 0u, 3u, GL_FLOAT, GL_FALSE, offsetof(VertexDefault, position) },
                VertexAttribute{ 1u, 3u, GL_FLOAT, GL_FALSE, offsetof(VertexDefault, color) },
                VertexAttribute{ 2u, 3u, GL_FLOAT, GL_FALSE, offsetof(VertexDefault, normal) },
                VertexAttribute{ 3u, 2u, GL_FLOAT, GL_FALSE, offsetof(VertexDefault, uvs) }
            };
        }
    };
}

#endif
