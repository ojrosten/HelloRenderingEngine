#version 330 core

out vec4 FragColor;

in vec2 ClipPos;

uniform float radius;
uniform vec2 centre;

void main()
{
    if(length(ClipPos - centre) > radius)
        discard;

    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}