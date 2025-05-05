#version 430 core

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;



void main()
{	
	// 纹理
	vec2 horizReverCoord = vec2(1.0 - texCoord.x,texCoord.y); // 水平翻转
	vec3 texColor = vec3(mix(texture(texture1,texCoord),texture(texture2,horizReverCoord),mixValue)); // 混合两张贴图，0.2表示第二张贴图的权重
	
	// 环境光
	float ambientStrenth = 0.2;
	vec3 ambient = ambientStrenth* lightColor;
	
	// 漫反射
	float diffuseStrenth = 1;
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 norm = normalize(normal);
	float diff = max(0.0,dot(lightDir,norm));
	vec3 diffuse = diffuseStrenth * diff * lightColor;

	// 镜面反射
	float specularStrength = 0.5;
	float specularExponent = 32;
	vec3 viewDir = normalize(viewPos - fragPos); 
	vec3 midDir = normalize(viewDir + lightDir);// 中程向量计算方式,blinn-phong，更柔和一些
	float spec = pow(max(0,dot(norm,midDir)),specularExponent);

	// vec3 reflectDir = reflect(-lightDir,norm); // 反射向量计算方式,注意取入射方向是从光源到顶点
	// reflectDir = 2*(dot(lightDir,norm))*norm - lightDir; // 手动计算反射向量公式
	// spec = pow(max(0,dot(viewDir,reflectDir)),specularExponent);

	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + spec) * objectColor;
	result *= vec3(texColor); // 贴图颜色叠加
	FragColor = vec4(result,1.0);

}