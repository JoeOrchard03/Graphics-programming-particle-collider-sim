#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

out vec3 vertNorm;
out vec2 vertUV;

uniform mat4 transform;
uniform mat4 transform2;

void main()
{
	gl_Position = transform * vec4(vertexPosition, 1.0f);

	//vec3 newVertexPosition = vertexPosition;
	//gl_Position = vec4(newVertexPosition,1.0f);

	vertNorm = vertexNormal;
	vertUV = vertexUV;
}