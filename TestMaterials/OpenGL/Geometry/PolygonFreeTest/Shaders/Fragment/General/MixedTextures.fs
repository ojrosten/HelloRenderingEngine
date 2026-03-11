#version 410 core
out vec4 FragColour;

in vec2 TexCoords0;
in vec2 TexCoords1;
in float weight;

uniform sampler2D image0;
uniform sampler2D image1;


void main()
{
    FragColour = mix(texture(image0, TexCoords0), texture(image1, TexCoords1), weight);
}