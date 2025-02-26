#version 330 core

layout (location = 0) in vec3 aLocalPos;

out vec2 ComplexPlanePos;

uniform float xRange = 2.5;
uniform float yRange = 2;
uniform float xOffset = -0.75;
uniform float yOffset = 0;

void main()
{
    ComplexPlanePos = vec2(aLocalPos.x * xRange / 2 + xOffset, aLocalPos.y * yRange / 2 + yOffset);
    gl_Position = vec4(aLocalPos, 1.0);
}

