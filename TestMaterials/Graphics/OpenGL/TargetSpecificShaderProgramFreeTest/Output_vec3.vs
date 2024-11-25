#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 colour;

void main()
{
    gl_Position = vec4(aPos.xyz, 1.0);
}