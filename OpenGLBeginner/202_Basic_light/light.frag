#version 430 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;

uniform vec3 objectColor;
uniform vec3 lightColor;


void main()
{	
	FragColor = vec4(1.0); // 光源始终白色
}