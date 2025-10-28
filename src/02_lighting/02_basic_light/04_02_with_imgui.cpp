#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader.h>
#include <camera.h>
#include <utils.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// settings
unsigned int SCR_WIDTH = 1080;
unsigned int SCR_HEIGHT = 800;
bool cursorLocked = true;

float mixValue = 0.5f; // 默认混合值
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.f, 1.f, 0.f));
// timer
float deltaTime = 0.f, lastTime = 0.f;
// mouse
double cursorLastX = double(SCR_WIDTH / 2);
double cursorLastY = double(SCR_HEIGHT / 2);
bool firstMouse = true;

// light
glm::vec3 lightPos{ 1.2f, 1.0f, 2.0f };


int main(int argc, char** argv)
{
	//std::cout << __cplusplus << std::endl;
	/// glfw:初始化和配置
	// ------------------------------
	const char* glsl_version = "#version 430";
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

	/// imgui
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr;   // 禁止保存配置到文件（ImGui 自动保存的配置文件，记录了窗口大小、位置、是否打开等信息。）
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true); //注册了所有必要的GLFW回调函数，注意会被自定义回调覆盖，需要手动在自定义回调转发到imgui对应事件
	ImGui_ImplOpenGL3_Init(glsl_version);
	// imgui风格
	ImGui::StyleColorsDark();	//ImGui::StyleColorsLight();


	/// 回调注册，设置
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback); /// 鼠标指针回调
	glfwSetScrollCallback(window, scroll_callback); /// 滚轮回调
	glfwSetMouseButtonCallback(window, mouse_button_callback); /// 鼠标按键回调
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); /// 锁定光标
	glEnable(GL_DEPTH_TEST); /// 深度测试

	/// 着色器
	Shader objectShader(getAssetAbsPath(argv[0],"phong_view.vert"), getAssetAbsPath(argv[0],"object_phong_view.frag")); // 视图空间中计算phong物体着色器
	Shader lightShader(getAssetAbsPath(argv[0],"phong_world.vert"), getAssetAbsPath(argv[0],"light.frag")); // 光源着色器

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
	std::string texturePath1 = getAssetAbsPath(argv[0],"assets/02_lighting/textures/container.jpg");
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
	std::string texturePath2 = getAssetAbsPath(argv[0],"assets/02_lighting/textures/awesomeface.png");
	data = stbi_load(texturePath2.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // 注意PNG是4通道RGBA
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	/// 立方体渲染顶点
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f
	};

	glm::vec3 cubePositions[] = {
	  glm::vec3(0.0f,  0.0f,  0.0f),
	};


	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	/// 光源立方体顶点VAO，想独立于物体
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);

	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(0 * sizeof(float)));
	glEnableVertexAttribArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 线框模式(默认为GL_FILL）

	/// 设置着色器中纹理uniform变量对应的纹理单元
	objectShader.use();
	glUniform1i(glGetUniformLocation(objectShader.ID, "texture1"), 0); // 手动设置, 指定着色器中的 texture1 采样器从纹理单元 GL_TEXTURE0获取数据, 
	objectShader.setInt("texture2", 1); // 或者使用着色器类设置( uniform变量 -- GL_TEXTURE0 -- glGenTextures缓冲区对象ID)
	/// 应用纹理
	glActiveTexture(GL_TEXTURE0); // 激活一个纹理单元(默认激活GL_TEXTURE0)
	glBindTexture(GL_TEXTURE_2D, texture); // 绑定到该纹理单元
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);


	bool show_demo_window = true;
	bool use_same_line = true;
	ImVec4 clear_color{ 0.45f, 0.55f, 0.60f, 1.00f };
	float ambientStrength = 0.2f;


	/// 窗口循环
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		float currentFrame = float(glfwGetTime());
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;
		processInput(window);

		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 状态设置
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 状态使用

		/// 光源旋转
		float lightPosRadius = 3.f;
		lightPos = std::move(glm::vec3{ lightPosRadius * cos(glfwGetTime() * 2),1.f,lightPosRadius * sin(glfwGetTime() * 2) }); // 这里不使用一开始的位置了，现在绕y轴旋转
		//std::cout << std::format("光源位置: ({},{},{})\n", lightPos.x, lightPos.y, lightPos.z);

		/// 物体Shader
		objectShader.use();
		objectShader.setFloat("mixValue", mixValue);
		objectShader.setVec3("lightColor", 1.0f, 0.5f, 0.31f);
		objectShader.setVec3("objectColor", glm::vec3(1.0f));
		objectShader.setVec3("viewPos", camera.Position);
		objectShader.setVec3("originLightPos", lightPos);
		objectShader.setFloat("ambientStrenth", ambientStrength); 

		glBindVertexArray(VAO);

		glm::mat4 model(1.0f), view(1.0f), projection(1.0f);
		model = glm::translate(model, cubePositions[0]);
		//float angle = 20.f * 0;
		//model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);
		objectShader.setMat4("model", model);
		objectShader.setMat4("view", view);
		objectShader.setMat4("projection", projection);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		/// 光源物体shader
		lightShader.use();
		glBindVertexArray(lightVAO);
		// 这里额外计算了让光源朝向target的model矩阵，可以不计算（我闲的）
		glm::vec3 target{ 0.f,0.f,0.f };
		model = ModelLookAt(lightPos, target);
		//model = glm::mat4(1);
		//model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); //缩小
		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		/// imgui
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
			ImGui::SliderFloat("Ambient Strength ", &ambientStrength, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("use sameline", &use_same_line);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			if (use_same_line) {
				ImGui::SameLine();
			}

			ImGui::Text("counter = %d", counter);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();

		}
		//渲染
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		//glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		glfwSwapBuffers(window);
		
	}
	/// 清理
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &texture2);
	glDeleteProgram(objectShader.ID);
	glDeleteProgram(lightShader.ID);
	//imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
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
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera.ProcessKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera.ProcessKeyboard(DOWN, deltaTime);
	}

	// 光标锁定/解锁切换--> 相机控制 / imgui输入
	static bool tPressedLastFrame = false;
	bool tPressed = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
	// 只在按键从释放变为按下时触发（边缘检测）
	if (tPressed && !tPressedLastFrame) {
        if (cursorLocked) { // 解锁光标，用于imgui输入
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 锁定光标，用于控制fps摄像机
            firstMouse = true; // 防止恢复锁定时的位置偏移导致的视角跳动
        }
        cursorLocked = !cursorLocked;
    }
    tPressedLastFrame = tPressed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (!cursorLocked) { // 光标解锁时不处理相机旋转
		ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos); // 给 ImGui
		return;
	}
	//相机旋转
	if (firstMouse) {
		firstMouse = false;
		cursorLastX = xpos;
		cursorLastY = ypos;
	}
	double xOffset = xpos - cursorLastX;
	double yOffset = cursorLastY - ypos;
	cursorLastX = xpos;
	cursorLastY = ypos;
	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (!cursorLocked) { // 光标解锁时不处理相机，控制imgui
		ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset); 
		return;
	}
	camera.ProcessMouseScroll(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) { 
	if(!cursorLocked) { // 仅光标解锁时允许点击imgui
		ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods); 
		return;
	}
}