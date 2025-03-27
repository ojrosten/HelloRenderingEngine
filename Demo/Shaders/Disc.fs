#version 330 core

in vec2 ClipPos;

out vec4 FragColor;

uniform vec2 centre;
uniform float radius;

void main()
{
    if(length(ClipPos - centre) > radius)
      discard;

    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}