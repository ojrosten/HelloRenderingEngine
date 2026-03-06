#version 330 core

out vec4 FragColor;

const float col = 128.0/255.0;
uniform vec4 colour = vec4(col, col, col, col);

void main()
{
   FragColor = colour;
}