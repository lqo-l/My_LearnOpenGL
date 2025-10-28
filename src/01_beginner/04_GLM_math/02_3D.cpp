#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader.h>
#include <utils.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.5f; // 默认混合值

int main(int argc, char** argv)
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
	Shader shader(getAssetAbsPath(argv[0],"2_3D.vert"), getAssetAbsPath(argv[0],"1.frag")); // 同时加载两张纹理

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
	unsigned char* data = stbi_load("assets/01_beginner/textures/container.jpg", &width, &height, &nrChannels, 0);
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
	data = stbi_load("assets/01_beginner/textures/awesomeface.png", &width, &height, &nrChannels, 0);
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
		-0.5f,-0.5f,0.0f, 1.0f,0.0f,0.0f, 0.0f,0.0f,	//左下
		-0.5f,0.5f,0.0f, 0.0f,1.0f,0.0f, 0.0f,1.0f,	//左上
		0.5f,0.5f,0.0f, 1.0f,0.0f,1.0f, 1.0f,1.0f,	//右上
		0.5f,-0.5f,0.0f, 1.0f,1.0f,0.0f, 1.0f,0.0f	//右下
	}; // 正常纹理坐标

	//float vertices[] = {
	//	-0.5f,-0.5f,0.0f, 1.0f,0.0f,0.0f, 0.0f,0.0f,	//左下
	//	-0.5f,0.5f,0.0f, 0.0f,1.0f,0.0f, 0.0f,2.0f,	//左上
	//	0.5f,0.5f,0.0f, 1.0f,0.0f,1.0f, 2.0f,2.0f,	//右上
	//	0.5f,-0.5f,0.0f, 1.0f,1.0f,0.0f, 2.0f,0.0f	//右下
	//}; // 验证wrap效果，纹理坐标超出[0,1]范围

	//float vertices[] = {
	//	-0.5f,-0.5f,0.0f, 1.0f,0.0f,0.0f, 0.25f,0.25f,	//左下
	//	-0.5f,0.5f,0.0f, 0.0f,1.0f,0.0f, 0.25f,0.75f,	//左上
	//	0.5f,0.5f,0.0f, 1.0f,0.0f,1.0f, 0.75f,0.75f,	//右上
	//	0.5f,-0.5f,0.0f, 1.0f,1.0f,0.0f, 0.75f,0.25f	//右下
	//}; // 只显示纹理图像中间部分

	//float vertices[] = {
	//	-0.5f,-0.5f,0.0f, 1.0f,0.0f,0.0f, 0.45f,0.45f,	//左下
	//	-0.5f,0.5f,0.0f, 0.0f,1.0f,0.0f, 0.45f,0.55f,	//左上
	//	0.5f,0.5f,0.0f, 1.0f,0.0f,1.0f, 0.55f,0.55f,	//右上
	//	0.5f,-0.5f,0.0f, 1.0f,1.0f,0.0f, 0.55f,0.45f	//右下
	//}; // 采用纹理很小块，能看到单个像素点，配合放大过滤中的GL_NEAREST可以看到效果

	unsigned int indices[] = {
		0,1,3,
		1,2,3
	};

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 线框模式(默认为GL_FILL）

	/// 设置着色器中纹理uniform变量对应的纹理单元
	// 只需要设置一次，所以放在循环外，需要指定着色器
	shader.use();
	glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0); // 手动设置
	shader.setInt("texture2", 1); // 或者使用着色器类设置

	/// 变换
	glm::mat4 model(1.0f), view(1.0f),projection(1.0f);
	// M：绕x轴旋转45度
	model = glm::rotate(model, glm::radians(45.f), glm::vec3(1.f, 0.f, 0.f));
	// V：相机从原点向后移动3个单位
	view = glm::translate(view, glm::vec3(0.f, 0.f, -3.f)); // 相机看向-z，设置相机位置(0,0,3)，则view矩阵的T是相反数(0,0,-3),详见推导。
	// P：fovy=45度，宽高比800/600，近裁剪面0.1，远裁剪面100。 一般与画布宽高比相同
	projection = glm::perspective(glm::radians(45.f), 800.f / 600.f, 0.1f, 100.f);
	

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 状态设置
		glClear(GL_COLOR_BUFFER_BIT); // 状态使用

		/// 应用纹理
		glActiveTexture(GL_TEXTURE0); // 激活一个纹理单元(默认激活GL_TEXTURE0)
		glBindTexture(GL_TEXTURE_2D, texture); // 绑定到该纹理单元
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		shader.use();
		shader.setFloat("mixValue", mixValue);
		
		int modelLoc = glGetUniformLocation(shader.ID, "model");
		int viewLoc = glGetUniformLocation(shader.ID, "view");
		int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &texture2);
	glDeleteProgram(shader.ID);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
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