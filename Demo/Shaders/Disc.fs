#version 330 core

in vec2 ClipPos;

out vec4 FragColor;

uniform vec2 centre;
uniform float radius;

void main()
{
    if(length(ClipPos - centre) > radius)
      discard;

    FragColor = vec4(1 - length(vec2(0,1) - ClipPos), 1 - length(vec2(-1,0) - ClipPos), 1 - length(vec2(1,0) - ClipPos), 1.0f);
}