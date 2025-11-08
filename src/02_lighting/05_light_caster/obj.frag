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

struct ParallelLight{
    vec3 direction; // 平行光方向, 从光源到物体，使用时需取反
    vec3 lightColor; // 光源颜色

    vec3 Ia; // ambient强度(vec3形式，可以对rgb分量分别调整)
    vec3 Id; // diffusion强度
    vec3 Is; // specular强度
};

struct PointLight{
    vec3 position;
    vec3 lightColor; // 光源颜色

    // 衰减系数
    float constant; // 一般为1，确保分母不小于1
    float linear;   // 主导小距离衰减
    float quadratic; // 主导大距离衰减

    vec3 Ia; // ambient强度(vec3形式，可以对rgb分量分别调整)
    vec3 Id; // diffusion强度
    vec3 Is; // specular强度
};

struct SpotLight{
    vec3 position;
    vec3 lightColor; // 光源颜色

    vec3 spotDir; // 聚光灯的方向
    float cutOff; // 内切光束余弦值
    float outerCutOff; // 外切光束余弦值
    
    // 衰减系数
    float constant; // 一般为1，确保分母不小于1
    float linear;   // 主导小距离衰减
    float quadratic; // 主导大距离衰减

    vec3 Ia; // ambient强度(vec3形式，可以对rgb分量分别调整)
    vec3 Id; // diffusion强度
    vec3 Is; // specular强度
};

uniform Material material;
uniform vec3 viewPos;

uniform ParallelLight parallel;
uniform PointLight point;
uniform SpotLight spot;

uniform bool emissionEnable; // 是否启用自发光
uniform float emissionOffset; // 移动效果
uniform float emissionStrength; // 闪烁效果

uniform bool openParallelLight;
uniform bool openPointLight;
uniform bool openSpotLight;

void main(){
    vec3 norm = normalize(normal); // world space下的法线
    vec3 viewDir = normalize(viewPos - fragWorldPos);

    vec3 texKd = texture(material.Kd, TexCoords).rgb;
    vec3 texKs = texture(material.Ks, TexCoords).rgb;

    vec3 ambient = vec3(0),diffuse = vec3(0) , specular = vec3(0);
    
    // 平行光
    if(openParallelLight){
        ambient += parallel.Ia * texKd * parallel.lightColor;
        vec3 lightDir = normalize(-parallel.direction);
        diffuse += parallel.Id * texKd * parallel.lightColor * max(0,cos(dot(lightDir,norm)));
        vec3 h = normalize(lightDir + viewDir); // 半程向量
        specular += parallel.Is * texKs * parallel.lightColor * pow(max(dot(norm, h ),0.0),material.shininess) ;
    }

    // 点光源
    if(openPointLight){
        float distance = length(point.position - fragWorldPos);
        float attenuation = 1.0 / (point.constant + point.linear * distance + point.quadratic * (distance * distance));
        ambient += point.Ia * texKd * point.lightColor * attenuation;
        vec3 lightDir = normalize(point.position - fragWorldPos);
        diffuse += point.Id * texKd * point.lightColor * max(0,dot(norm, lightDir)) * attenuation;
        vec3 h = normalize(lightDir + viewDir); // 半程向量
        specular += point.Is * texKs * point.lightColor * pow(max(dot(norm, h),0.0), material.shininess) * attenuation;
    }

    // 聚光灯
    if(openSpotLight){
        float distance = length(spot.position - fragWorldPos);
        vec3 lightDir = normalize(spot.position - fragWorldPos); // frag2lightPos
        float attenuation = 1.0 / (spot.constant + spot.linear * distance + spot.quadratic * (distance*distance));
        float epsilon = spot.cutOff - spot.outerCutOff; // 分母
        float lightRadian = dot(lightDir, -spot.spotDir); // 计算光线与聚光灯中心方向的余弦值(注意：spot.spotDir是光源指向物体，需要取反与lightDir保持一致)
        
        float intensity = (lightRadian - spot.outerCutOff)/ epsilon; // 线性插值计算强度，角度越大强度越小。角度大于等于外光锥时，强度为0，角度小于内光锥时，强度为1
        intensity = clamp(intensity, 0.f, 1.f); // 限制在0-1之间
        ambient += spot.Ia * texKd * spot.lightColor * attenuation; // 环境光不受聚光强度影响
        diffuse += spot.Id * texKd * spot.lightColor * max(0, dot(norm, lightDir)) * attenuation * intensity;
        vec3 h = normalize(lightDir + viewDir);
        specular += spot.Is * spot.lightColor * texKs * pow(max(0, dot(h, norm)), material.shininess) * attenuation * intensity;
    }

    vec3 result = ambient + diffuse + specular ;
    // 自发光
    if(emissionEnable){ 
        vec3 texEmission = texture(material.emission, TexCoords + vec2(0.f, emissionOffset)).rgb;
        if(texKs.r > 0 || texKs.g > 0 || texKs.b > 0){ // 仅在箱子内部显示自发光效果(贴图内部全黑->全0, 不全0就设置自发光为0)
            texEmission = vec3(0.f);
        }
        vec3 Ke = texEmission * emissionStrength;
        result += Ke;
    }
    
    // 总颜色
    FragColor = vec4(result, 1.0);
}