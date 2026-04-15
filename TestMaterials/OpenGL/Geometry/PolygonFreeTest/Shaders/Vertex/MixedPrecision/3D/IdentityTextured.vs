#version 410 core

layout (location = 0) in dvec3 aLocalPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aLocalPos, 1.0);
    TexCoords   = aTexCoords;
}