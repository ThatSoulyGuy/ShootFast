#ifndef SHOOTFAST_MESH_HPP
#define SHOOTFAST_MESH_HPP

#include <vector>
#include "Client/Render/ClientRenderContext.hpp"
#include "Client/Render/Handles.hpp"
#include "Client/Render/Registries.hpp"
#include "Client/Render/VertexBase.hpp"

namespace ShootFast::Client::Render
{
    template <VertexType T>
    MeshGPU UploadStatic(const std::vector<T>& vertices, const std::vector<uint32_t>& indices)
    {
        MeshGPU result{};

        glGenVertexArrays(1, &result.vao);
        glGenBuffers(1, &result.vbo);
        glGenBuffers(1, &result.ebo);

        glBindVertexArray(result.vao);

        glBindBuffer(GL_ARRAY_BUFFER, result.vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(T)), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)), indices.data(), GL_STATIC_DRAW);

        for (const auto& attr : T::GetAttributes())
        {
            glEnableVertexAttribArray(attr.index);
            glVertexAttribPointer(attr.index, attr.components, attr.type, attr.normalized, T::STRIDE, reinterpret_cast<const void*>(attr.offset));
        }

        glBindVertexArray(0);

        result.indexCount = static_cast<GLsizei>(indices.size());

        return result;
    }

    template <VertexType T>
    class Mesh final
    {

    public:

        using Handle = MeshHandle;

        std::vector<T> vertices;
        std::vector<uint32_t> indices;

        static Handle Upload(const ClientRenderContext& context, const Mesh& mesh)
        {
            return context.meshRegistry->Create(UploadStatic(mesh.vertices, mesh.indices));
        }

        static void Destroy(const ClientRenderContext& context, const Handle handle)
        {
            auto& [vao, vbo, ebo, indexCount] = context.meshRegistry->Get(handle);

            if (ebo != 0u)
            {
                glDeleteBuffers(1, &ebo);
                ebo = 0u;
            }

            if (vbo != 0u)
            {
                glDeleteBuffers(1, &vbo);
                vbo = 0u;
            }

            if (vao != 0u)
            {
                glDeleteVertexArrays(1, &vao);
                vao = 0u;
            }

            indexCount = 0;
        }
    };
}

#endif