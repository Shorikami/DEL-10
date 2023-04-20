#version 450 core

in VERTEX
{
	vec3 fragPos;
	vec3 fragNorm;
	vec2 texCoords;
	
	vec3 viewPos;
} fs_in;

layout(std140, binding = 0) readonly uniform Lights
{
  vec4 position[10000];
  vec4 colorRadius[10000]; // xyz = color, w = radius
} lightBuffer;

layout(std430, binding = 1) readonly buffer VisibilityBuffer
{
	int idx[];
} visibility;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform int numLights;
uniform int xAmount;

uniform bool debugDisplay;

out vec4 fragColor;

float Attenuation(vec3 L, float r)
{
	float cutoff = 0.25f;
	
	float att = dot(L, L) / (20.0f * r);
	att = 1.0f / (att * 15.0f + 1.0f);
	att = (att - cutoff) / (1.0f - cutoff);
	
	return clamp(att, 0.0f, 1.0f);
}

void main()
{
	// Current pixel of screen
	ivec2 px = ivec2(gl_FragCoord.xy);
	
	// Current tile based on pixel
	ivec2 tile = px / ivec2(32, 32);
	
	// Current index of tile
	uint idx = tile.y * xAmount + tile.x;
	
	vec3 diff = texture(texture_diffuse1, fs_in.texCoords).rgb;
	vec3 spec = texture(texture_specular1, fs_in.texCoords).rgb;
	
	vec3 N = normalize(fs_in.fragNorm);
	vec3 V = normalize(fs_in.viewPos - fs_in.fragPos);
	
	uint o = idx * numLights;
	
	int lights = 0;
	vec3 color = vec3(0.0f);
	
	for (int i = 0; i < numLights && visibility.idx[o + i] != -1; ++i)
	{
		if (!debugDisplay)
		{
			int lightIdx = visibility.idx[o + i];
		
			vec4 lightColor = vec4(lightBuffer.colorRadius[lightIdx].xyz, 1.0f);
			float lightRadius = lightBuffer.colorRadius[lightIdx].w;
			
			vec3 L = lightBuffer.position[lightIdx].xyz - fs_in.fragPos;
			float att = Attenuation(L, lightRadius);
			
			L = normalize(L);
			vec3 H = normalize(L + V);
			
			vec3 finalDiff = diff * max(dot(N, L), 0.0f);
			vec3 finalSpec = spec * pow(max(dot(N, H), 0.0f), 16.0f);
			
			color += (lightColor.rgb * (finalDiff + finalSpec) * att);
		}
		else
		{
			++lights;
		}
	}
	
	if (!debugDisplay)
	{
		// "Ambient" light
		color += (diff * 0.02f);
	}
	else
	{
		if (lights <= 0)
		{
			color = vec3(1.0f, 0.0f, 0.0f);
		}
		else if (lights > 0 && lights <= 50)
		{
			color = vec3(1.0f - (lights / 50.0f), 0.0f + (lights / 50.0f), 0.0f);
		}
		else if (lights > 50 && lights <= 100)
		{
			color = vec3(0.0f, 1.0f - ((lights - 50) / 50.0f), 0.0f + ((lights - 50) / 50.0f));
		}
		else if (lights > 100 && lights <= 1000)
		{
			color = vec3(0.0f + ((lights - 100) / 900.0f), 0.0f, 1.0f);
		}
		else
		{
			color = vec3(1.0f, 1.0f, 1.0f);
		}
	}
	
	fragColor = vec4(color, 1.0f);
}