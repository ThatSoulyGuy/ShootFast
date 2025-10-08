#ifndef SHOOTFAST_RENDER_MESH_HPP
#define SHOOTFAST_RENDER_MESH_HPP

#include "Client/Render/Handles.hpp"
#include "Client/Render/VertexBase.hpp"

namespace ShootFast::Client::Render
{
    template <VertexType>
    class RenderMesh final
    {

    public:

        MeshHandle mesh;
        MaterialHandle material;
    };
}

#endif