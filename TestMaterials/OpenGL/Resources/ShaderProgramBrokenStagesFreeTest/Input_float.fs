#version 330 core

in float colour;

out vec4 FragColor;

void main()
{
   FragColor = vec4(colour, 0.0, 0.0, 1.0);
}