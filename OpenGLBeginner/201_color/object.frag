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
	// vec2 horizReverCoord = vec2(1.0 - texCoord.x,texCoord.y); // 水平翻转
	// FragColor = mix(texture(texture1,texCoord),texture(texture2,horizReverCoord),mixValue); // 混合两张贴图，0.2表示第二张贴图的权重
	FragColor = vec4(objectColor*lightColor,1.0);
}