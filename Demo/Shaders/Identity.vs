#version 330 core

layout (location = 0) in vec3 aLocalPos;

out vec3 WorldPos;

void main()
{
    gl_Position = vec4(aLocalPos, 1.0);
    WorldPos    = aLocalPos;
}

