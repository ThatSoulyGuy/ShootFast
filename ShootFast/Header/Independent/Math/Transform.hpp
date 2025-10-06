#ifndef SHOOTFAST_TRANSFORM_HPP
#define SHOOTFAST_TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ShootFast::Independent::Math
{
    struct Transform final
    {
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::quat rotation{ };
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
    };

    struct LocalToWorld final
    {
        glm::mat4 matrix{1.0f};
    };
}


#endif