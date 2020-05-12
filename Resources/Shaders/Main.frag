#version 330

in vec3 Normal;
in vec2 TexCoord;
flat in vec3 Color;

out vec4 outputColor;

uniform sampler2D gSampler;
uniform vec4 vColor;

uniform bool bSelection;
uniform bool bTextured;
uniform bool bLighted;
uniform bool bProjection;

struct DirectionalLight
{
	vec3 vColor;
	vec3 vDirection;
	float fAmbient;
};

uniform DirectionalLight Light;

void main()
{

	vec3 ColorOut = vec3(0, 0, 0);
	
	if (bSelection) {
		outputColor = vec4(Color, 1.0);
		return;
	}

	if (bProjection) {
		ColorOut = vec3(1, 1, 1);
	} else {
		ColorOut = vec3(0, 0, 0);
	}

	if (bTextured) {
		ColorOut = texture2D(gSampler, TexCoord).rgb;
	} else {
		outputColor = vec4(ColorOut, 1.0) * vColor;
	}

	
	if (bLighted) {
		float DiffuseIntensity = max(0.0, dot(normalize(Normal), -Light.vDirection));
		outputColor = vec4(ColorOut, 1.0) * vColor * vec4(Light.vColor* (Light.fAmbient + DiffuseIntensity), 1.0);
	} else {
		outputColor = vec4(ColorOut, 1.0) * vColor;
	}



}
