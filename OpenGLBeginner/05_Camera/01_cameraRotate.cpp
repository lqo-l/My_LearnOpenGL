#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

float mixValue = 0.5f; // 默认混合值

int main()
{
	//std::cout << __cplusplus << std::endl;
	/// glfw:初始化和配置
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//std::cout << "OpenGL版本: " << glGetString(GL_VERSION) << std::endl;
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // MacOS需要设置
#endif

	/// glfw窗口创建
	// ------------------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "窗口创建失败" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//glViewport(0, 0, 800, 600); // glad加载之前，无法使用OpenGL函数，指针尚未加载
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	/// glad:加载所有OpenGL函数指针
	// ------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "GLAD初始化失败" << std::endl;
		return -1;
	}

	/// 着色器
	Shader shader("1.vert", "1.frag"); // 同时加载两张纹理

	/// 纹理设置加载
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // 只有纹理缩小才能用MipMap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//加载生成纹理
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // 加载时反转图片，因为图片原点一般在左上角，opengl屏幕空间原点左下	
	unsigned char* data = stbi_load("../../Assets/Texture/container.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); // 根据level-0自动生成MipMap，共log2(max(width, height)) + 1
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// 纹理2
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float boarderColor[] = { 1.0f,0.0f,0.5f,1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, boarderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//stbi_set_flip_vertically_on_load(true); // 指定一次即可
	data = stbi_load("../../Assets/Texture/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // 注意PNG是4通道RGBA
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	/// 顶点
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
	  glm::vec3(0.0f,  0.0f,  0.0f),
	  glm::vec3(2.0f,  5.0f, -15.0f),
	  glm::vec3(-1.5f, -2.2f, -2.5f),
	  glm::vec3(-3.8f, -2.0f, -12.3f),
	  glm::vec3(2.4f, -0.4f, -3.5f),
	  glm::vec3(-1.7f,  3.0f, -7.5f),
	  glm::vec3(1.3f, -2.0f, -2.5f),
	  glm::vec3(1.5f,  2.0f, -2.5f),
	  glm::vec3(1.5f,  0.2f, -1.5f),
	  glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 线框模式(默认为GL_FILL）

	/// 设置着色器中纹理uniform变量对应的纹理单元
	// 只需要设置一次，所以放在循环外，需要指定着色器
	shader.use();
	glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0); // 手动设置
	shader.setInt("texture2", 1); // 或者使用着色器类设置



	/// 相机设置测试（给定相机位置和目标位置）
	glm::vec3 cameraPos(0.f, 0.f, 3.f);
	glm::vec3 cameraTarget(0.f);
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget); // z轴，相机朝向-z轴
	glm::vec3 up(0.f, 1.f, 0.f);

	// 手动构建view矩阵
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
	 // 分别算w2c的R(R_inverse变量)和T（T_inverse变量）
	glm::mat4 R_inverse( /// ，实际上，mat4按列向量接收参数，所以这样顺序输入的话恰好是想要的矩阵的转置,所以后续对这个矩阵进行纠正
		cameraRight.x, cameraRight.y, cameraRight.z, 0.0f,
		cameraUp.x, cameraUp.y, cameraUp.z, 0.0f,
		cameraDirection.x, cameraDirection.y, cameraDirection.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	glm::mat4 T_inverse( // 同上，得到的矩阵并非日常所见的矩阵。
		1.f, 0.f, 0.f, -cameraPos.x,
		0.f, 1.f, 0.f, -cameraPos.y,
		0.f, 0.f, 1.f, -cameraPos.z,
		0.f, 0.f, 0.f, 1.f
	);
	R_inverse = glm::transpose(R_inverse); // 纠正上述列优先问题导致的矩阵构造错误
	T_inverse = glm::transpose(T_inverse);
	glm::mat4 view_manual = R_inverse * T_inverse;
	// 注：访问某个元素时，mat[col][row]，即mat[col]是列向量，mat[col][row]是列向量的第row个元素

	// lookAt自动构建view矩阵
	glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, up);
	//for (int row = 0; row < 4; row++) {
	//	for (int col = 0; col < 4; col++) {
	//		std::cout << view[col][row] << " "; // 由于以列向量形式存储，所以访问row行col列要先拿到第col个列向量，再获取行，哈哈2行3列不是[2][3]是[3][2]
	//	}
	//	std::cout << std::endl;
	//}

	glEnable(GL_DEPTH_TEST);
	/// 窗口循环
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 状态设置
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 状态使用

		/// 应用纹理
		glActiveTexture(GL_TEXTURE0); // 激活一个纹理单元(默认激活GL_TEXTURE0)
		glBindTexture(GL_TEXTURE_2D, texture); // 绑定到该纹理单元
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		shader.use();
		shader.setFloat("mixValue", mixValue);

		glBindVertexArray(VAO);
		/// 变换十个立方体
		for (size_t i = 0; i < 10; i++)
		{
			glm::mat4 model(1.0f), view(1.0f), projection(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.f * i ;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			
			/// 绕圆旋转,看向原点
			float radius = 10.f;
			float cameraPosX = cos(float(glfwGetTime()) ) * radius;
			float cameraPosZ = sin(float(glfwGetTime()) ) * radius;
			cameraPos = glm::vec3{ cameraPosX, 0.f, cameraPosZ };

			view = glm::lookAt(cameraPos, cameraTarget,cameraUp);
			projection = glm::perspective(glm::radians(45.f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);
			shader.setMat4("model", model);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &texture2);
	glDeleteProgram(shader.ID);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		mixValue -= 0.01f;
		if (mixValue < 0.0f) {
			mixValue = 0.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		mixValue += 0.01f;
		if (mixValue > 1.0f) {
			mixValue = 1.0f;
		}
	}
}