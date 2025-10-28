#version 430 core

in vec4 vertexColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
	FragColor = texture(ourTexture,texCoord);
	FragColor = FragColor * vertexColor;
}