/* shader变量传递 uniform类型 */
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const char* vertexShaderSource = "#version 430 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aColor;\n"
"out vec4 vertexColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);\n"   // gl_Position是内建变量，vec4类型，裁剪空间坐标
"	vertexColor = vec4(aColor,1.0);\n"
"}\0";

/// uniform变量
const char* fragmentShaderSource = "#version 430 core\n"
"in vec4 vertexColor;\n"
"out vec4 FragColor;\n"
"uniform vec4 specialColor;\n"
"void main()\n"
"{\n"
"	FragColor = specialColor;\n"
"}\0";

/// 插值颜色
const char* fragmentShaderSource_2 = "#version 430 core\n"
"in vec4 vertexColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vertexColor;\n"
"}\0";


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
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

	/// 顶点着色器 
	// ------------------------------
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// 检查编译错误
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/// 片元着色器 
	// ------------------------------	
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// 
	unsigned int fragmentShader_2;
	fragmentShader_2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader_2, 1, &fragmentShaderSource_2, NULL);
	glCompileShader(fragmentShader_2);
	glGetShaderiv(fragmentShader_2, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader_2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/// 着色器程序 
	// ------------------------------
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// 检查链接错误
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader); // 链接后不再需要着色器对象
	// 2
	unsigned int shaderProgram_2;
	shaderProgram_2 = glCreateProgram();
	glAttachShader(shaderProgram_2, vertexShader);
	glAttachShader(shaderProgram_2, fragmentShader_2);
	glLinkProgram(shaderProgram_2);
	// 检查链接错误
	glGetProgramiv(shaderProgram_2, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramiv(shaderProgram_2, GL_LINK_STATUS, &success);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader_2); // 链接后不再需要着色器对象
	/// 1.三角形绘制
	// ------------------------------
	float vertices[] = {
	-0.8f,-0.5f,0.0f,
	0.2f,-0.5f,0.0f,
	-0.3f,0.5f,0.0f
	};

	/// VAO顶点数组对象
	// ------------------------------
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	/// VBO顶点缓冲对象
	// ------------------------------
	unsigned int VBO; // 顶点缓冲对象ID
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	/*std::cout << "size:" << sizeof(vertices) << std::endl;*/
	// 顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // 会从当前绑定的VBO中获取数据
	glEnableVertexAttribArray(0); // 0是顶点属性的位置值

	glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑VBO，在glVertexAttribPointer之后解绑是可以的
	glBindVertexArray(0); // 解绑VAO,非必要，可以防止修改该VAO

	/// 2. 三角形绘制2
	float vertices_2[] = {
	-0.2f,-0.5f,0.0f, 1.0f,0.0f,0.0f,
	0.8f,-0.5f,0.0f, 0.0f,1.0f,0.0f,
	0.3f,0.5f,0.0f, 0.0f,0.0f,1.0f
	};
	unsigned int indices[] = {
		0,1,2
	};
	unsigned int VAO_2, VBO_2, EBO;
	glGenVertexArrays(1, &VAO_2);
	glGenBuffers(1, &VBO_2);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO_2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), vertices_2, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float))); 
	glEnableVertexAttribArray(1);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 线框模式(默认为GL_FILL）

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 状态设置
		glClear(GL_COLOR_BUFFER_BIT); // 状态使用
		/// 1
		glUseProgram(shaderProgram);

		float timeValue = static_cast<float>(glfwGetTime());
		float greenValue = (sin(timeValue) + 1.f) / 2.f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "specialColor");
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);	// uniform更新前需要先使用程序，因为是在当前激活的着色器程序中设置uniform

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/// 2
		glUseProgram(shaderProgram_2);
		glBindVertexArray(VAO_2);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

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
}