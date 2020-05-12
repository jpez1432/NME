#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

flat out vec3 Color;

void main()
{
	
	gl_Position = projectionMatrix*modelViewMatrix*vec4(inPosition, 1.0);
	Color = inColor;

}
