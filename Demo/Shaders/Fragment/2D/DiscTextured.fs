#version 330 core

out vec4 FragColor;

in vec2 ClipPos;
in vec2 TexPos;

uniform float radius;
uniform vec2 centre;
uniform sampler2D image;

void main()
{
    if(length(ClipPos - centre) > radius)
        discard;

    FragColor = texture(image, TexPos);
}