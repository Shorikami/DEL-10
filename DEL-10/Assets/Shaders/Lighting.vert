#version 450 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoords;

out VERTEX
{
	vec3 fragPos;
	vec3 fragNorm;
	vec2 texCoords;
	
	vec3 viewPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 viewPos;

void main()
{
	vec4 pos = vec4(aPosition, 1.0f);

	gl_Position = projection * view * model * pos;
	vs_out.fragPos = vec3(model * pos);
	vs_out.texCoords = aTexCoords;
	vs_out.viewPos = viewPos;
	
	mat3 normalMat = transpose(inverse(mat3(model)));
	vec3 norm = normalize(normalMat * aNormals);
	vs_out.fragNorm = norm;
}