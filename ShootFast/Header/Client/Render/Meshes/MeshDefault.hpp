#ifndef SHOOTFAST_MESH_DEFAULT_HPP
#define SHOOTFAST_MESH_DEFAULT_HPP

#include "Client/Render/MeshBase.hpp"

namespace ShootFast::Client::Render::Vertices
{
    class VertexDefault;
}

namespace ShootFast::Client::Render::Meshes
{
    class MeshDefault final : public MeshBase<Vertices::VertexDefault>
    {

    public:

        MeshDefault() = default;

        ~MeshDefault() override;

        [[nodiscard]]
        std::vector<Vertices::VertexDefault> GetVertices() const override;

        void SetVertices(const std::vector<Vertices::VertexDefault>&) override;

        [[nodiscard]]
        std::vector<uint32_t> GetIndices() const override;

        void SetIndices(const std::vector<uint32_t>&) override;

        std::unordered_map<std::string, GLuint> GetBufferMap() override;

        void Generate() override;

        void Render() override;

    private:

        std::vector<Vertices::VertexDefault> vertices;
        std::vector<uint32_t> indices;

        GLuint vertexArrayObject = 0;
        GLuint vertexBufferObject = 0;
        GLuint elementBufferObject = 0;

    };
}

#endif