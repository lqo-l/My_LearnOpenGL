#version 430 core

in vec2 texCoord;
// in vec3 normal;
// in vec3 fragPos;
// in vec3 lightPos;
in vec3 lightResultColor;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;

// uniform vec3 objectColor;
// uniform vec3 lightColor;

void main()
{	
	// 纹理
	vec2 horizReverCoord = vec2(1.0 - texCoord.x,texCoord.y); // 水平翻转
	vec3 texColor = vec3(mix(texture(texture1,texCoord),texture(texture2,horizReverCoord),mixValue)); // 混合两张贴图，0.2表示第二张贴图的权重
	
	vec3 result = lightResultColor;
	result *= vec3(texColor); // 贴图颜色叠加
	FragColor = vec4(result,1.0);

}