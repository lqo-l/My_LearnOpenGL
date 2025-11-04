#version 430 core

in vec3 normal;
in vec3 fragWorldPos;

out vec4 FragColor;

struct Material {
    vec3 Ka; // ambient反射系数     // 一般认为是物体颜色
    vec3 Kd; // diffusion反射系数   // 一般认为是物体颜色，等于Ka
    vec3 Ks; // specular反射系数
    float shininess; // 高光指数
};

struct Light{
    vec3 position;
    vec3 lightColor; // 光源颜色

    vec3 Ia; // ambient强度(vec3形式，可以对rgb分量分别调整)
    vec3 Id; // diffusion强度
    vec3 Is; // specular强度
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main(){
    // 环境光
    vec3 ambient = light.Ia * material.Ka * light.lightColor;

    // 漫反射
    vec3 norm = normalize(normal); // world space下的法线
    vec3 lightDir = normalize(light.position - fragWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.Id * material.Kd * light.lightColor * diff;

    // 镜面反射
    vec3 viewDir = normalize(viewPos - fragWorldPos);
    vec3 h = normalize(lightDir + viewDir); // 半程向量
    float spec = pow(max(dot(norm, h),0.0), material.shininess);
    vec3 specular = light.Is * material.Ks * light.lightColor * spec;

    // 总颜色
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}