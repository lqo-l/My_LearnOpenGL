#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec4 vertexColor;

uniform float xOffset;

void main()
{
	gl_Position = vec4(aPos.x + xOffset,aPos.y,aPos.z,1.0) ; 
	gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0) ; 

	//VertexColor = vec4(aColor,1.0);
	vertexColor = gl_Position;
}