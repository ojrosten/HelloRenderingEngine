#version 330 core

out vec4 FragColor;

uniform float foo_f = 1;
uniform float bar_f = 2;

uniform vec2 foo_f2 = vec2( 1, -1);
uniform vec2 bar_f2 = vec2(-2,  2);

uniform vec3 foo_f3 = vec3( 1, -1,  1);
uniform vec3 bar_f3 = vec3(-2,  2, -2);

uniform vec4 foo_f4 = vec4( 1, -1,  1, -1);
uniform vec4 bar_f4 = vec4(-2,  2, -2,  2);

uniform int bar_i = 2;
uniform int foo_i = 1;

uniform ivec2 bar_i2 = ivec2(-2,  2);
uniform ivec2 foo_i2 = ivec2( 1, -1);

uniform ivec3 bar_i3 = ivec3(-2,  2, -2);
uniform ivec3 foo_i3 = ivec3( 1, -1,  1);

uniform ivec4 bar_i4 = ivec4(-2,  2, -2,  2);
uniform ivec4 foo_i4 = ivec4( 1, -1,  1, -1);

void main()
{
   FragColor = vec4(foo_f, bar_f, 0, 0);
   FragColor += vec4(foo_f2, 0, 0);
   FragColor += vec4(bar_f2, 0, 0);
   FragColor += vec4(foo_f3, 0);
   FragColor += vec4(bar_f3, 0);
   FragColor += foo_f4;
   FragColor += bar_f4;
   FragColor += vec4(foo_i, bar_i, 0, 0);
   FragColor += vec4(foo_i2, 0, 0);
   FragColor += vec4(bar_i2, 0, 0);
   FragColor += vec4(foo_i3, 0);
   FragColor += vec4(bar_i3, 0);
   FragColor += foo_i4;
   FragColor += bar_i4;
}