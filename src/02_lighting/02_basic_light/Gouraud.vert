#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

out vec2 texCoord;
out vec3 lightResultColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 originLightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

// Gouraud着色，根据所有顶点的位置法线计算顶点颜色，之后在frag中进行颜色的插值； Phong是插值出所有片元（所属虚拟顶点）的位置和法线，然后计算每个片元的颜色。
// 插值颜色不如插值法线真实。
void main()
{
	gl_Position = projection*view*model * vec4(aPos,1.0) ; 
	texCoord = aTexCoord;

	vec3 normal = mat3(transpose(inverse(view*model))) * aNormal; //视图空间下的法线、顶点位置、光源位置
	vec3 fragPos = vec3(view*model * vec4(aPos, 1.0));
	vec3 lightPos = vec3(view * vec4(originLightPos,1.0));

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
	vec3 viewDir = normalize(-fragPos); 
	vec3 midDir = normalize(viewDir + lightDir);// 中程向量计算方式,blinn-phong，更柔和一些
	float spec = pow(max(0,dot(norm,midDir)),specularExponent);
	// vec3 reflectDir = reflect(-lightDir,norm); // 反射向量计算方式,注意取入射方向是从光源到顶点
	// reflectDir = 2*(dot(lightDir,norm))*norm - lightDir; // 手动计算反射向量公式
	// spec = pow(max(0,dot(viewDir,reflectDir)),specularExponent);
	vec3 specular = specularStrength * spec * lightColor;

	lightResultColor = (ambient + diffuse + spec) * objectColor;

}