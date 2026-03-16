#version 410 core

layout (location = 0) in dvec3 aLocalPos;
layout (location = 1) in dvec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aLocalPos, 1.0);
    TexCoords   = vec2(aTexCoords);
}