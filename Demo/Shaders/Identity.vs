#version 330 core

layout (location = 0) in vec3 aLocalPos;

out vec3 WorldPos;

void main()
{
	WorldPos    = aLocalPos;
    gl_Position = vec4(aLocalPos.xyz, 1.0);
}