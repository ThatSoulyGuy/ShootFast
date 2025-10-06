#ifndef SHOOTFAST_RENDER_MESH_HPP
#define SHOOTFAST_RENDER_MESH_HPP

#include "Client/Render/Handles.hpp"

namespace ShootFast::Client::Render
{
    class RenderMesh final
    {

    public:

        MeshHandle mesh;
        MaterialHandle material;
    };
}

#endif