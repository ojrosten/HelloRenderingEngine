#version 410 core

layout (location = 0) in dvec3 aLocalPos;
layout (location = 2) in vec2 aTexCoords0;
layout (location = 3) in vec2 aTexCoords1;

out vec2 TexCoords0;
out vec2 TexCoords1;

void main()
{
    gl_Position = vec4(aLocalPos, 1.0);
    TexCoords0  = aTexCoords0;
    TexCoords1  = aTexCoords1;
}