#version 430 core

in vec4 vertexColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	FragColor = mix(texture(texture1,texCoord),texture(texture2,texCoord),0.2); // 混合两张贴图，0.2表示第二张贴图的权重
}