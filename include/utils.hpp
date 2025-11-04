#pragma once

#include <glad/glad.h>

#include <string>
#include <iostream>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

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
inline glm::mat4 ModelLookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up = glm::vec3(0.f, 1.f, 0.f))
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

/**
 * @brief 加载纹理
 * 
 * @param path 纹理绝对路径或相对于可执行文件的路径
 * @return unsigned int 纹理对象ID
 */
inline unsigned int loadTexture(const char *path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}