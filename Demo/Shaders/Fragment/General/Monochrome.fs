#version 330 core

out vec4 FragColor;

uniform vec4 colour = vec4(1.0f, 0.5f, 0.2f, 1.0f); 

void main()
{
   FragColor = colour;
}