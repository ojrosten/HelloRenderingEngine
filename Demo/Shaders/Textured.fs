

#version 330 core
out vec4 FragColour;

in vec2 TexCoords;

uniform sampler2D image;
uniform float gamma = 2.2;

void main()
{
    FragColour     = texture(image, TexCoords);
    FragColour.rgb = pow(FragColour.rgb, vec3(1.0/gamma));
}