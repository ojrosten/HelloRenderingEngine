#version 410 core

layout (location = 0) in dvec3 aLocalPos;

void main()
{
    gl_Position = vec4(aLocalPos, 1.0);
}