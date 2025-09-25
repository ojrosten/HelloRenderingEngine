#version 330 core

layout (location = 0) in vec2 aWorldPos;
layout (location = 1) in vec2 aTexPos;

out vec2 ClipPos;
out vec2 TexPos;

void main()
{
    gl_Position = vec4(aWorldPos, 0.0, 1.0);
    ClipPos     = aWorldPos;
    TexPos      = aTexPos;
}