#ifndef SHOOTFAST_CAMERA_HPP
#define SHOOTFAST_CAMERA_HPP

namespace ShootFast::Client::Render
{
    class Camera final
    {

    public:

        float fovDegrees{ 70.0f };
        float nearPlane{ 0.01f }, farPlane{ 1000.0f };
        bool active{ true };
        float offsetY{ 1.7f };
    };
}

#endif