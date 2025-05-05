#pragma once

#include <glad/glad.h>

#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/// <summary>
/// 根据模型的位置和lookat构建模型的model矩阵（从模型空间到世界空间） 
/// </summary>
/// <param name="pos">模型位置</param>
/// <param name="target">看向的目标</param>
/// <param name="up">上向量</param>
/// <returns>model矩阵</returns>
glm::mat4 ModelLookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up = glm::vec3(0.f,1.f,0.f)) {
	glm::mat4 model(1.f);

	glm::vec3 front(normalize(target - pos)); // z
	glm::vec3 right = normalize(glm::cross(up, front)); // x
	up = glm::cross(front, right); // y

	model[0] = glm::vec4(right,0);
	model[1] = glm::vec4(up, 0);
	model[2] = glm::vec4(front, 0);
	model[3] = glm::vec4(pos, 1);
	
	return model; // 模型->世界的变换矩阵model
}