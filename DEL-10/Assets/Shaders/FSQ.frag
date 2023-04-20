#version 450 core

in vec2 texCoords;

uniform sampler2D hdrBuffer;

uniform float exposure;

out vec4 fragColor;

void main() {
	vec3 color = texture(hdrBuffer, texCoords).rgb;
	vec3 result = vec3(1.0) - exp(-color * 1.0f);

	const float gamma = 2.2f;
	result = pow(result, vec3(1.0f / gamma));
	fragColor = vec4(result, 1.0f);
}