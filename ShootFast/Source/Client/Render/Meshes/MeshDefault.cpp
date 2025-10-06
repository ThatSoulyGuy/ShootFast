#include "Client/Render/Meshes/MeshDefault.hpp"

#include <stdexcept>

#include "Client/Render/Vertices/VertexDefault.hpp"

using namespace ShootFast::Client::Render::Vertices;

namespace ShootFast::Client::Render::Meshes
{
    MeshDefault::~MeshDefault()
    {
        if (elementBufferObject != 0)
            glDeleteBuffers(1, &elementBufferObject);

        if (vertexBufferObject != 0)
            glDeleteBuffers(1, &vertexBufferObject);

        if (vertexArrayObject != 0)
            glDeleteVertexArrays(1, &vertexArrayObject);
    }

    std::vector<VertexDefault> MeshDefault::GetVertices() const
    {
        return vertices;
    }

    void MeshDefault::SetVertices(const std::vector<VertexDefault>& newVertices)
    {
        vertices = newVertices;

        if (vertexBufferObject != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(VertexDefault)), vertices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    std::vector<uint32_t> MeshDefault::GetIndices() const
    {
        return indices;
    }

    void MeshDefault::SetIndices(const std::vector<uint32_t>& newIndices)
    {
        indices = newIndices;
        
        if (elementBufferObject != 0)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)), indices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }

    std::unordered_map<std::string, GLuint> MeshDefault::GetBufferMap()
    {
        return
        {
            {"VAO", vertexArrayObject},
            {"VBO", vertexBufferObject},
            {"EBO", elementBufferObject}
        };
    }

    void MeshDefault::Generate()
    {
        if (vertices.empty())
            throw std::runtime_error(std::string(typeid(*this).name()) + "::Generate: no vertices");

        if (indices.empty())
            throw std::runtime_error(std::string(typeid(*this).name()) + "::Generate: no indices");

        if (vertexArrayObject == 0)
            glGenVertexArrays(1, &vertexArrayObject);

        if (vertexBufferObject == 0)
            glGenBuffers(1, &vertexBufferObject);

        if (elementBufferObject == 0)
            glGenBuffers(1, &elementBufferObject);

        glBindVertexArray(vertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(VertexDefault)), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)), indices.data(), GL_STATIC_DRAW);

        for (const auto& [index, components, type, normalized, offset] : VertexDefault::GetAttributes())
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, components, type, normalized, VertexDefault::STRIDE, reinterpret_cast<const void*>(offset));
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void MeshDefault::Render()
    {
        if (vertexArrayObject == 0)
            return;

        glBindVertexArray(vertexArrayObject);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
    }
}