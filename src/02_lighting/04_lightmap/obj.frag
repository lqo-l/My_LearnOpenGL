#version 430 core

in vec2 TexCoords;
in vec3 normal;
in vec3 fragWorldPos;

out vec4 FragColor;

// 教程这里漫反射贴图当纹理用了的感觉。

struct Material {
    sampler2D Kd;  // 漫反射贴图，等于ambient，不设单独ambient
    sampler2D Ks; // 镜面反射贴图
    sampler2D emission; // 自发光贴图
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

uniform float emissionOffset; // 移动效果
uniform float emissionStrength; // 闪烁效果

void main(){
    vec3 Kd = vec3(texture(material.Kd, TexCoords));
    vec3 Ka = Kd;
    vec3 Ks = vec3(texture(material.Ks, TexCoords));
    vec3 Ke = emissionStrength * texture(material.emission, TexCoords+vec2(0.f,emissionOffset)).rgb;
    // 仅在箱子内部显示自发光效果(贴图内部全黑,全0)
    if(Ks.r>0 || Ks.g>0 || Ks.b>0){
        Ke = vec3(0.f);
    }

    // 环境光
    vec3 ambient = light.Ia * Ka * light.lightColor;

    // 漫反射
    vec3 norm = normalize(normal); // world space下的法线
    vec3 lightDir = normalize(light.position - fragWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.Id * Kd * light.lightColor * diff;

    // 镜面反射
    vec3 viewDir = normalize(viewPos - fragWorldPos);
    vec3 h = normalize(lightDir + viewDir); // 半程向量
    float spec = pow(max(dot(norm, h),0.0), material.shininess);
    vec3 specular = light.Is * Ks * light.lightColor * spec;

    // 总颜色
    vec3 result = ambient + diffuse + specular + Ke;
    FragColor = vec4(result, 1.0);
}