#version 410 core

layout (location = 0) in dvec3 aLocalPos;
layout (location = 1) in dvec2 aTexCoords0;
layout (location = 2) in dvec2 aTexCoords1;

out vec2 TexCoords0;
out vec2 TexCoords1;

void main()
{
    gl_Position = vec4(aLocalPos, 1.0);
    TexCoords0  = vec2(aTexCoords0);
    TexCoords1  = vec2(aTexCoords1);
}