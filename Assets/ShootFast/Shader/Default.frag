#version 410 core

out vec4 FragColor;

uniform sampler2D diffuse;

in vec3 colorPass;
in vec3 normalPass;
in vec2 uvsPass;

void main()
{
    FragColor = texture(diffuse, uvsPass) * vec4(colorPass, 1.0f);
}