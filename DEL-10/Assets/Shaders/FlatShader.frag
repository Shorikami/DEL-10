#version 430 core

in vec3 fragPos;
in vec3 fragNorm;

uniform vec3 color;

out vec4 fragColor;

void main()
{
	fragColor = vec4(vec3(color), 1.0f);
}
