#version 410 core

layout (location = 0) in vec3 positionIn;
layout (location = 1) in vec3 colorIn;
layout (location = 2) in vec3 normalIn;
layout (location = 3) in vec2 uvsIn;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 colorPass;
out vec3 normalPass;
out vec2 uvsPass;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(positionIn, 1.0f);

    colorPass = colorIn;
    normalPass = normalIn;
    uvsPass = uvsIn;
}