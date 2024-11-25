#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 colour;

void main()
{
	colour = vec3(0.0, 1.0, 0.0);
    gl_Position = vec4(aPos.xyz, 1.0);
}