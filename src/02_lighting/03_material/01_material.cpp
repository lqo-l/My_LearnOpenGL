#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader.hpp>
#include <camera.hpp>
#include <utils.hpp>
#include "material.hpp"
#include "light.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

unsigned int SCR_WIDTH = 1440;
unsigned int SCR_HEIGHT = 800;
bool cursorLocked = true;

Camera camera(glm::vec3(0.f, 0.f, 6.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
Light light(glm::vec3{1.2f, 1.0f, 2.0f});
Material material(MatID::bronze);

float deltaTime =0.f, lastTime = 0.f;

double cursorLastX = double(SCR_WIDTH / 2);
double cursorLastY = double(SCR_HEIGHT / 2);
bool firstMouse = true;


int main(int argc, char** argv){
    // 初始化
    const char* glsl_version = "#version 430";
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Texture", NULL, NULL);
    if (window == NULL) {
        std::cout << "Fail to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 加载OpenGL函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Imgui初始化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // 中文字体
    ImFontConfig config;
    config.MergeMode = false;
    config.OversampleH = 2;
    config.OversampleV = 2;
    const char* fontPath = getAssetAbsPath(argv[0], "C:\\Windows\\Fonts\\msyh.ttc").c_str(); // 微软雅黑
    ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath, 18.0f, &config, io.Fonts->GetGlyphRangesChineseSimplifiedCommon()); // GetGlyphRangesChineseFull()
    if(font== nullptr){
        std::cout << "Failed to load font: " << fontPath << std::endl;
        io.Fonts->AddFontDefault();  // 回退到默认字体
    } 

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    // 回调注册，设置
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    // 着色器
    Shader objShader(getAssetAbsPath(argv[0],"obj.vert"), getAssetAbsPath(argv[0],"obj.frag"));
    Shader lightShader(getAssetAbsPath(argv[0],"light.vert"), getAssetAbsPath(argv[0],"light.frag"));

    // 纹理略

    // 立方体顶点、uv坐标、法线。(x,  y,  z,  u, v,  nx, ny, nz, )
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
        glm::vec3(0.f, 0.f, 0.f)
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5* sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // 光源立方体顶点
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 变量
    ImVec4 clear_color{0.2f, 0.3f, 0.3f, 1.0f};
    int currentMatID = material.getID();

    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        float currentFrame = float(glfwGetTime());
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;
		processInput(window);

        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /// imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
            ImGui::Begin("Config");

            /// 光源
            ImGui::Separator(); // 分割线
            ImGui::Text("Light");
            ImGui::ColorEdit3("Light Color", &light.lightColor[0]);
            ImGui::DragFloat3("Light Position", &light.position[0], 0.1f,-100.f, 100.f);
            ImGui::ColorEdit3("Ia", &light.Ia[0]);
            ImGui::ColorEdit3("Id", &light.Id[0]);
            ImGui::ColorEdit3("Is", &light.Is[0]);

            /// 物体材质
            ImGui::Separator(); // 分割线
            ImGui::Text("Material: %s  Desc: %s", material.name.c_str(), material.desc.c_str());
            if(material.isModified()){
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),"(Modified)");
            }
            if(ImGui::Combo("Presets", &currentMatID, Material::getPresetNames().data(), Material::getPresetCount())){
                material.setMaterial(static_cast<MatID>(currentMatID));
            }

            ImGui::Spacing();
            ImGui::Text("Adjust Material Parameters:");
            ImGui::ColorEdit3("Ambient (Ka)", &material.Ka[0]);
            ImGui::ColorEdit3("Diffuse (Kd)", &material.Kd[0]);
            ImGui::ColorEdit3("Specular (Ks)", &material.Ks[0]);
            ImGui::SliderFloat("Shininess", &material.shininess, 1.0f, 256.0f);
            if(material.isModified()){ // 重置按钮（只在有预设ID且被修改时显示）
                if(ImGui::Button("Reset to Preset")){
                    material.setMaterial(currentMatID);
                }
            }

            // 其他
            ImGui::ColorEdit4("Background Color", (float*)&clear_color);
            ImGui::Text("FPS: %d", int(io.Framerate));

            ImGui::End();
        }
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /// 物体
        glm::mat4 model(1.f), view(1.f), projection(1.f);
        model = glm::translate(model, cubePositions[0]);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        objShader.use();
        objShader.setMat4("model", model);
        objShader.setMat4("view", view);
        objShader.setMat4("projection", projection);
        objShader.setVec3("viewPos", camera.Position);
        // TODO: 封装这些结构体配置
        objShader.setVec3("light.Ia", light.Ia);
        objShader.setVec3("light.Id", light.Id);
        objShader.setVec3("light.Is", light.Is);
        objShader.setVec3("light.position", light.position);
        objShader.setVec3("light.lightColor", light.lightColor);

        objShader.setVec3("material.Ka", material.Ka);
        objShader.setVec3("material.Kd", material.Kd);
        objShader.setVec3("material.Ks", material.Ks);
        objShader.setFloat("material.shininess", material.shininess);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        /// 光源
        model = ModelLookAt(light.position, {0.f, 0.f, 0.f}); // 相机看向原点
        model = glm::scale(model, glm::vec3(0.2f)); // 缩小一些

        lightShader.use();
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("lightColor", light.lightColor);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


		glfwSwapBuffers(window);
    }
    // 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteProgram(objShader.ID);
    glDeleteProgram(lightShader.ID);
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