#version 330 core
out vec4 FragColour;

in vec2 TexCoords;

uniform sampler2D image0;

void main()
{
    FragColour = texture(image0, TexCoords);
}