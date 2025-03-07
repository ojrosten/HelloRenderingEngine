#version 330 core

layout (location = 0) in vec2 aLocalPos;

void main()
{
    gl_Position = vec4(aLocalPos.xy, 0.0, 1.0);
}