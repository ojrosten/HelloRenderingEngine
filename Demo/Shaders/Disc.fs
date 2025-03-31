#version 330 core

in vec2 ClipPos;

out vec4 FragColor;

uniform vec2 centre;
uniform float radius;

void main()
{
    if(length(ClipPos - centre) > radius)
      discard;

    FragColor = vec4(1 - length(vec2(0,0.5) - ClipPos), 1 - length(vec2(-0.5,-0.5) - ClipPos), 1 - length(vec2(0.5,-0.5) - ClipPos), 1.0f);
}