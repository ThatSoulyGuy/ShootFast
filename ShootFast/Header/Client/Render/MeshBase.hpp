#ifndef SHOOTFAST_MESH_HPP
#define SHOOTFAST_MESH_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "Client/Utility/GLFW.hpp"

namespace ShootFast::Client::Render
{
    class VertexBase;

    template <typename T>
    class MeshBase
    {

    public:

        virtual ~MeshBase() = default;

        [[nodiscard]]
        virtual std::vector<T> GetVertices() const = 0;

        virtual void SetVertices(const std::vector<T>&) = 0;

        [[nodiscard]]
        virtual std::vector<uint32_t> GetIndices() const = 0;

        virtual void SetIndices(const std::vector<uint32_t>&) = 0;

        virtual std::unordered_map<std::string, GLuint> GetBufferMap() = 0;

        virtual void Generate() = 0;
        virtual void Render() = 0;
        
    };
}

#endif