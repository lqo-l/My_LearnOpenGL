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

struct DirLight{ // 定向光（平行光），无衰减
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

uniform DirLight parallel;
uniform PointLight point;
uniform SpotLight spot;

uniform bool emissionEnable; // 是否启用自发光
uniform float emissionOffset; // 移动效果
uniform float emissionStrength; // 闪烁效果

uniform bool openParallelLight;
uniform bool openPointLight;
uniform bool openSpotLight;

uniform sampler2D flashTexture; // 聚光处的纹理图

vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir);

void main(){
    vec3 norm = normalize(normal); // world space下的法线
    vec3 viewDir = normalize(viewPos - fragWorldPos); 

    // 多光源累加
    vec3 result = vec3(0);
    if(openParallelLight){
        result += calcDirLight(parallel, norm, viewDir);
    }
    if(openPointLight){
        result += calcPointLight(point, norm, fragWorldPos, viewDir);
    }

    vec3 spotColor = calcSpotLight(spot, norm, fragWorldPos, viewDir);
    if(openSpotLight){
        result += spotColor;
    }

    // 仅屏幕中心显示的纹理效果
    vec2 screenCoords = gl_FragCoord.xy; // 片元的窗口坐标
    float screenAspect = 1440.0 / 800.0; // 屏幕宽高比，写死
    vec2 normalizedCoords = screenCoords / vec2(1440.0, 800.0); // 窗口大小这里写死为1440*800
    float zoom = 0.4; // 缩放系数，值越小，图片越小

    vec2 texPos = (normalizedCoords - 0.5) / zoom;
    texPos.x *= screenAspect; // 根据屏幕宽高比调整采样的y坐标，否则会被拉伸
    texPos = texPos + 0.5;
    vec3 flashColor = texture(flashTexture, texPos).rgb; // 仅在屏幕中心（聚光灯）显示。注：opengl窗口坐标系左下角为原点。

    result += flashColor * spotColor;

    // 自发光效果
    if(emissionEnable){ 
        vec3 texEmission = texture(material.emission, TexCoords + vec2(0.f, emissionOffset)).rgb;
        if(any(notEqual(texKs,vec3(0.0)))){ // 仅在箱子内部显示自发光效果(贴图内部全黑->全0, 不全0就设置自发光为0)
            texEmission = vec3(0.f);
        }
        vec3 Ke = texEmission * emissionStrength;
        result += Ke;
    }

    // 总颜色
    FragColor = vec4(result, 1.0);
}

// 坐标和向量均为世界坐标下
vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir){
    vec3 result = vec3(0);
    norm = normalize(norm);
    viewDir = normalize(viewDir);
    vec3 texKd = texture(material.Kd, TexCoords).rgb;
    vec3 texKs = texture(material.Ks, TexCoords).rgb;

    result += light.Ia * texKd * light.lightColor;
    vec3 lightDir = normalize(-light.direction);
    result += light.Id * texKd * light.lightColor * max(0,cos(dot(lightDir,norm)));
    vec3 h = normalize(lightDir + viewDir); // 半程向量
    result += light.Is * texKs * light.lightColor * pow(max(dot(norm, h ),0.0),material.shininess) ;

    return result;
}

// 坐标和向量均为世界坐标下
vec3 calcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir){
    vec3 result = vec3(0);
    norm = normalize(norm);
    viewDir = normalize(viewDir);
    vec3 texKd = texture(material.Kd, TexCoords).rgb;
    vec3 texKs = texture(material.Ks, TexCoords).rgb;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    result += light.Ia * texKd * light.lightColor * attenuation;
    vec3 lightDir = normalize(light.position - fragPos);
    result += light.Id * texKd * light.lightColor * max(0,dot(norm, lightDir)) * attenuation;
    vec3 h = normalize(lightDir + viewDir); // 半程向量
    result += light.Is * texKs * light.lightColor * pow(max(dot(norm, h),0.0), material.shininess) * attenuation;

    return result;
}

// 坐标和向量均为世界坐标下
vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir){
    vec3 result = vec3(0);
    norm = normalize(norm);
    viewDir = normalize(viewDir);
    vec3 texKd = texture(material.Kd, TexCoords).rgb;
    vec3 texKs = texture(material.Ks, TexCoords).rgb;
    float distance = length(spot.position - fragPos);
    vec3 lightDir = normalize(spot.position - fragPos); // frag2lightPos
    float attenuation = 1.0 / (spot.constant + spot.linear * distance + spot.quadratic * (distance*distance));
    float epsilon = spot.cutOff - spot.outerCutOff; // 分母
    float lightRadian = dot(lightDir, -spot.spotDir); // 计算光线与聚光灯中心方向的余弦值(注意：spot.spotDir是光源指向物体，需要取反与lightDir保持一致)
    float intensity = (lightRadian - spot.outerCutOff)/ epsilon; // 线性插值计算强度，角度越大强度越小。角度大于等于外光锥时，强度为0，角度小于内光锥时，强度为1
    intensity = clamp(intensity, 0.f, 1.f); // 限制在0-1之间

    result += spot.Ia * texKd * spot.lightColor * attenuation; // 环境光不受聚光强度影响
    result += spot.Id * texKd * spot.lightColor * max(0, dot(norm, lightDir)) * attenuation * intensity;
    vec3 h = normalize(lightDir + viewDir);
    result += spot.Is * spot.lightColor * texKs * pow(max(0, dot(h, norm)), material.shininess) * attenuation * intensity;

    return result;
}

