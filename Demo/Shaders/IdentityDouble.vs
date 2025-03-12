#version 410 core

layout (location = 0) in dvec3 aLocalPos;

void main()
{
    gl_Position = vec4(aLocalPos.xyz, 1.0);
}