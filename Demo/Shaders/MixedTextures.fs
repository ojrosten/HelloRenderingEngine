#version 330 core
out vec4 FragColour;

in vec2 TexCoords0;
in vec2 TexCoords1;

uniform sampler2D image0;
uniform sampler2D image1;
uniform float gamma = 2.2;

void main()
{
    FragColour     = mix(texture(image0, TexCoords0), texture(image1, TexCoords1), 0.4);
    FragColour.rgb = pow(FragColour.rgb, vec3(1.0/gamma));
}