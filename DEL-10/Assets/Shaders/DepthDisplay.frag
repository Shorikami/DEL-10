#version 450 core

uniform float nearP;
uniform float farP;

out vec4 fragColor;

float LinearizeDepth(float d)
{
	float z = d * 2.0f - 1.0f;
	return (2.0f * nearP * farP) / (farP + nearP - z * (farP - nearP));
}

void main()
{
	float depth = LinearizeDepth(gl_FragCoord.z) / farP;
	fragColor = vec4(vec3(depth), 1.0f);
}