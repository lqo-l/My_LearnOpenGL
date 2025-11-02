#pragma once

#include <glad/glad.h>

#include <string>
#include <iostream>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fs = std::filesystem;

/**
 * @brief Get the Absolute Dir string of filePath
 * 
 * @param filePath File path(relative or absolute)
 * @return std::string. Directory path string
 */
inline std::string getDir(const char *filePath)
{
	return fs::absolute(fs::path(filePath)).parent_path().string();
}


/**
 * @brief 获取相对于可执行文件的素材文件绝对路径
 * 
 * @param argv0 可执行文件路径
 * @param relativePath 素材文件相对于可执行文件所在目录的相对路径
 * @return std::string 素材绝对路径
 */
inline std::string getAssetAbsPath(const char *argv0, const std::string &relativePath)
{
	// 把相对路径和可执行文件目录合并后再做归一化。
	// 之前的实现是在右侧的相对路径上调用 lexically_normal(),
	// 导致合并后仍可能包含 ".." 等成分而没有被消除。
	fs::path base = fs::path(getDir(argv0));
	fs::path combined = base / fs::path(relativePath);
	// 先转为绝对路径再做 lexically_normal()，这样能正确清理多级相对路径成分。
	return fs::absolute(combined).lexically_normal().string();
}

/// <summary>
/// 根据模型的位置和lookat构建模型的model矩阵（从模型空间到世界空间），
///注：view构建用glm::lookAt,这里用于构造model的。区别在于：相机的front是-z；此外view矩阵是world->camera(model)，model矩阵是model->world。
/// </summary>
/// <param name="pos">模型位置</param>
/// <param name="target">看向的目标</param>
/// <param name="up">上向量</param>
/// <returns>model矩阵</returns>
glm::mat4 ModelLookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up = glm::vec3(0.f, 1.f, 0.f))
{
	glm::mat4 model(1.f);

	glm::vec3 front(normalize(target - pos));			// z
	glm::vec3 right = normalize(glm::cross(up, front)); // x
	up = glm::cross(front, right);						// y

	model[0] = glm::vec4(right, 0);
	model[1] = glm::vec4(up, 0);
	model[2] = glm::vec4(front, 0);
	model[3] = glm::vec4(pos, 1);

	return model; // 模型->世界的变换矩阵model
}