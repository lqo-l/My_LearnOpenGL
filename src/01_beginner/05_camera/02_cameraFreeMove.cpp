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
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

float mixValue = 0.5f; // 默认混合值

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.f, pitch = 0.f; // 让初始通过角度计算的front方向与上面相同
float moveSpeed = 4.f;
float fov = 45.f;
// timer
float deltaTime=0.f, lastTime=0.f;
// mouse
double cursorLastX = double(SCR_WIDTH/2);
double cursorLastY = double(SCR_HEIGHT/2);
double sensitivityX = 0.05f;
double sensitivityY = 0.06f;
bool firstMouse = true;

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

	/// glad:加载所有OpenGL函数指针
	// ------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "GLAD初始化失败" << std::endl;
		return -1;
	}
	/// 回调注册，设置
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback); /// 鼠标指针回调
	glfwSetScrollCallback(window, scroll_callback); /// 滚轮回调
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); /// 锁定光标
	glEnable(GL_DEPTH_TEST); /// 深度测试

	/// 着色器
	Shader shader(getAssetAbsPath(argv[0],"1.vert"), getAssetAbsPath(argv[0],"1.frag")); // 同时加载两张纹理

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
	
	std::string texturePath1 = getAssetAbsPath(argv[0], "assets/01_beginner/textures/container.jpg");
	unsigned char* data = stbi_load(texturePath1.c_str(), &width, &height, &nrChannels, 0);
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
	std::string texturePath2 = getAssetAbsPath(argv[0], "assets/01_beginner/textures/awesomeface.png");
	data = stbi_load(texturePath2.c_str(), &width, &height, &nrChannels, 0);
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


	/// 窗口循环
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = float(glfwGetTime());
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;
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
			float angle = 20.f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			/// 看向相机前方（-z）
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);
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
	// 退出
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	// 纹理混合比例
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
	//相机移动
	float cameraSpeed = deltaTime* moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront,cameraUp));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraUp;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraUp;
	}
	
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	//相机旋转
	if (firstMouse) {
		firstMouse = false;
		cursorLastX = xpos;
		cursorLastY = ypos;
	}
	double xOffset = xpos - cursorLastX;
	//double yOffset = ypos - cursorLastY; // GLFW提供的鼠标位置 (xpos, ypos):原点在窗口的左上角,Y 轴从上到下递增。
	//注：opengl仍然是左下角为原点，Y轴从下到上递增
	double yOffset =  cursorLastY - ypos;
	cursorLastX = xpos;
	cursorLastY = ypos;
	//std::cout << xOffset << " " << yOffset << std::endl;
	xOffset = xOffset * sensitivityX;
	yOffset = yOffset * sensitivityY;

	yaw += xOffset; // 角度制
	pitch += yOffset;
	pitch = glm::clamp<double>(pitch, -89.0, 89.0); // 限制不超过89度，fps模式
	cameraFront.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	cameraFront.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront.y = sin(glm::radians(pitch));
	cameraFront = glm::normalize(cameraFront); // 这些角度是front向量的角度（相机看向），不是相机z轴的角度，因此通过角度计算front无需额外取反
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	fov -= yoffset; // 向后滑滚轮时，yoffset为负，fov增大
	fov = glm::clamp<float>(fov, 1.0f, 70.0f);
}