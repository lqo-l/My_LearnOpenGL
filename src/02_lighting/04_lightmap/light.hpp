#pragma once

#include <glm/glm.hpp>

class Light{
public:
    Light(glm::vec3 position, 
          glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f), // 白光
          glm::vec3 Ia = glm::vec3(1.0f, 1.0f, 1.0f),
          glm::vec3 Id = glm::vec3(1.0f, 1.0f, 1.0f),
          glm::vec3 Is = glm::vec3(1.0f, 1.0f, 1.0f))
        : position(position), lightColor(lightColor), Ia(Ia), Id(Id), Is(Is) {}
    
    glm::vec3 position; // 光源位置（点光源）
    glm::vec3 lightColor; // 光源颜色

    // 注:以下强度在使用预设材质时默认应为(1.0, 1.0, 1.0)
    glm::vec3 Ia;  // 环境光强度
    glm::vec3 Id;  // 漫反射强度
    glm::vec3 Is; // 镜面反射强度
};