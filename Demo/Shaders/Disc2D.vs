#version 330 core

layout (location = 0) in vec2 aWorldPos;

void main()
{
    gl_Position = vec4(aWorldPos, 0.0, 1.0);
}