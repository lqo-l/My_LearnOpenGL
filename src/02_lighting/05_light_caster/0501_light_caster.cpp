#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <shader.hpp>
#include <camera.hpp>
#include <utils.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

unsigned int SCR_WIDTH = 1440;
unsigned int SCR_HEIGHT = 800;
bool cursorLocked = true;

Camera camera(glm::vec3(0.f, 0.f, 6.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

float deltaTime = 0.f, lastTime = 0.f;

double cursorLastX = double(SCR_WIDTH / 2);
double cursorLastY = double(SCR_HEIGHT / 2);
bool firstMouse = true;

int main(int argc, char **argv)
{
    // 初始化
    const char *glsl_version = "#version 430";
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Texture", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Fail to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 加载OpenGL函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Imgui初始化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // 中文字体
    ImFontConfig config;
    config.MergeMode = false;
    config.OversampleH = 2;
    config.OversampleV = 2;
    const char *fontPath = getAssetAbsPath(argv[0], "C:\\Windows\\Fonts\\msyh.ttc").c_str();                                  // 微软雅黑
    ImFont *font = io.Fonts->AddFontFromFileTTF(fontPath, 18.0f, &config, io.Fonts->GetGlyphRangesChineseSimplifiedCommon()); // GetGlyphRangesChineseFull()
    if (font == nullptr)
    {
        std::cout << "Failed to load font: " << fontPath << std::endl;
        io.Fonts->AddFontDefault(); // 回退到默认字体
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
    Shader objShader(getAssetAbsPath(argv[0], "obj.vert"), getAssetAbsPath(argv[0], "obj.frag"));
    Shader lightShader(getAssetAbsPath(argv[0], "light.vert"), getAssetAbsPath(argv[0], "light.frag"));

    /// 光照贴图数据
    // 漫反射贴图
    std::string diffusePath = getAssetAbsPath(argv[0], "assets/02_lighting/textures/container2.png");
    std::string specularPath = getAssetAbsPath(argv[0], "assets/02_lighting/textures/container2_specular.png");
    std::string emissionPath = getAssetAbsPath(argv[0], "assets/02_lighting/textures/emission_matrix.jpg");

    unsigned int diffuseMap, specularMap, emissionMap;
    diffuseMap = loadTexture(diffusePath.c_str());
    specularMap = loadTexture(specularPath.c_str());
    emissionMap = loadTexture(emissionPath.c_str());

    // 立方体顶点、uv坐标、法线。(x,  y,  z,  u, v,  nx, ny, nz, )
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

    std::vector<glm::vec3> cubePositions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // 光源立方体顶点
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 设置shader中纹理uniform变量对应的纹理单元
    objShader.use();
    objShader.setInt("material.Kd", 0);
    objShader.setInt("material.Ks", 1);
    objShader.setInt("material.emission", 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissionMap);

    // 变量
    ImVec4 clear_color{0.2f, 0.3f, 0.3f, 1.0f};
    float emissionOffset = 0.f;
    float emissionStrength = 1.f;
    bool emissionMove = false;
    bool emissionFlick = false;
    bool emissionEnable = false;

    // 光源开启设置
    bool openParallelLight = false;
    bool openPointLight = false;
    bool openSpotLight = true;

    // 平行光
    glm::vec3 parallelLightColor = glm::vec3(1.f);
    glm::vec3 parallelLightDirection = glm::vec3(-0.2f, -1.0f, -0.3f); 
    glm::vec3 parallelIa = glm::vec3(0.2f);
    glm::vec3 parallelId = glm::vec3(0.5f);
    glm::vec3 parallelIs = glm::vec3(0.5f);

    // 点光源
    glm::vec3 pointLightColor = glm::vec3(1.f);
    glm::vec3 pointLightPos = glm::vec3(-0.2f, -1.0f, -0.3f); 
    float pointLightConstant = 1.0f;
    float pointLightLinear = 0.09f;
    float pointLightQuadratic = 0.032f;
    glm::vec3 pointIa = glm::vec3(0.2f);
    glm::vec3 pointId = glm::vec3(0.5f);
    glm::vec3 pointIs = glm::vec3(0.5f);

    // 聚光灯
    glm::vec3 spotLightColor = glm::vec3(1.f);
    float spotLightConstant = 1.0f;
    float spotLightLinear = 0.02f;
    float spotLightQuadratic = 0.006f;
    float cutOffAngle = 4.f;
    float outerCutOffAngle = 8.f; 
    glm::vec3 spotIa = glm::vec3(0.4f);
    glm::vec3 spotId = glm::vec3(0.6f);
    glm::vec3 spotIs = glm::vec3(0.6f);


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;
        processInput(window);

        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /// 物体
        objShader.use();
        objShader.setVec3("viewPos", camera.Position);
        objShader.setBool("openParallelLight", openParallelLight);
        objShader.setBool("openPointLight", openPointLight);
        objShader.setBool("openSpotLight", openSpotLight);

        objShader.setVec3("parallel.direction", parallelLightDirection);
        objShader.setVec3("parallel.lightColor", parallelLightColor);
        objShader.setVec3("parallel.Ia", parallelIa);
        objShader.setVec3("parallel.Id", parallelId);
        objShader.setVec3("parallel.Is", parallelIs);

        objShader.setVec3("point.lightColor", pointLightColor);
        objShader.setVec3("point.position", pointLightPos);
        objShader.setVec3("point.Ia", pointIa);
        objShader.setVec3("point.Id", pointId);
        objShader.setVec3("point.Is", pointIs);
        objShader.setFloat("point.constant", pointLightConstant);
        objShader.setFloat("point.linear", pointLightLinear);
        objShader.setFloat("point.quadratic", pointLightQuadratic);

        objShader.setVec3("spot.lightColor", spotLightColor);
        objShader.setVec3("spot.position", camera.Position);
        objShader.setVec3("spot.spotDir", camera.Front); // spotDIr是聚光灯照射方向
        objShader.setFloat("spot.cutOff", glm::cos(glm::radians(cutOffAngle))); // 转为cos，减少片段着色器计算量
        objShader.setFloat("spot.outerCutOff", glm::cos(glm::radians(outerCutOffAngle)));
        objShader.setVec3("spot.Ia", spotIa);
        objShader.setVec3("spot.Id", spotId);
        objShader.setVec3("spot.Is", spotIs);
        objShader.setFloat("spot.constant", spotLightConstant);
        objShader.setFloat("spot.linear", spotLightLinear);
        objShader.setFloat("spot.quadratic", spotLightQuadratic);

        objShader.setFloat("material.shininess", 32);

        if (emissionMove)
        {
            emissionOffset += deltaTime * 0.5f;
        }
        if (emissionFlick)
        {
            emissionStrength = 1.f + sinf(currentFrame);
        }
        objShader.setBool("emissionEnable", emissionEnable);
        objShader.setFloat("emissionOffset", emissionOffset);
        objShader.setFloat("emissionStrength", emissionStrength);

        glm::mat4 model(1.f), view(1.f), projection(1.f);
        for (int i = 0; i < cubePositions.size(); i++)
        {
            model = glm::mat4(1.f);
            model = glm::translate(model, cubePositions[i]);
            float angle = i % 3 == 0 ? static_cast<float>(glfwGetTime()) * i : 0.f;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            model = glm::scale(model, glm::vec3(1.5));
            view = camera.GetViewMatrix();
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

            objShader.setMat4("model", model);
            objShader.setMat4("view", view);
            objShader.setMat4("projection", projection);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        /// 光源
        model = ModelLookAt(pointLightPos, {0.f, 0.f, 0.f}); // 相机看向原点
        model = glm::scale(model, glm::vec3(0.2f));           // 缩小一些

        lightShader.use();
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("lightColor", pointLightColor);

        /// imgui(最后绘制，避免被覆盖)
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
            ImGui::Begin("Config");
            
            /// 光源
            if (ImGui::CollapsingHeader("平行光设置", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("开启平行光", &openParallelLight);
                if (openParallelLight)
                {
                    ImGui::Indent();
                    ImGui::ColorEdit3("平行光颜色", &parallelLightColor[0]);
                    ImGui::DragFloat3("平行光方向", &parallelLightDirection[0], 0.1f);
                    ImGui::ColorEdit3("平行光Ia", &parallelIa[0]);
                    ImGui::ColorEdit3("平行光Id", &parallelId[0]);
                    ImGui::ColorEdit3("平行光Is", &parallelIs[0]);
                    ImGui::Unindent();
                }
            }

            if (ImGui::CollapsingHeader("点光源设置", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("开启点光源", &openPointLight);
                if (openPointLight)
                {
                    ImGui::Indent();
                    ImGui::ColorEdit3("点光源颜色", &pointLightColor[0]);
                    ImGui::DragFloat3("点光源位置", &pointLightPos[0], 0.1f);
                    ImGui::ColorEdit3("点光源Ia", &pointIa[0]);
                    ImGui::ColorEdit3("点光源Id", &pointId[0]);
                    ImGui::ColorEdit3("点光源Is", &pointIs[0]);
                    ImGui::DragFloat("点光源衰减常数项", &pointLightConstant, 0.1f, 0.f, 5.f);
                    ImGui::DragFloat("点光源衰减线性项", &pointLightLinear, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("点光源衰减二次项", &pointLightQuadratic, 0.001f, 0.f, 2.f);
                    ImGui::Unindent();
                }
            }

            if (ImGui::CollapsingHeader("聚光灯设置", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("开启聚光灯", &openSpotLight);
                if (openSpotLight)
                {
                    ImGui::Indent();
                    ImGui::ColorEdit3("聚光灯颜色", &spotLightColor[0]);
                    ImGui::DragFloat("内切角", &cutOffAngle, 0.1f, 0.f, 90.f);
                    ImGui::DragFloat("外切角", &outerCutOffAngle, 0.1f, 0.f, 90.f);
                    ImGui::ColorEdit3("聚光Ia", &spotIa[0]);
                    ImGui::ColorEdit3("聚光Id", &spotId[0]);
                    ImGui::ColorEdit3("聚光Is", &spotIs[0]);
                    ImGui::DragFloat("聚光衰减常数项", &spotLightConstant, 0.1f, 0.f, 5.f);
                    ImGui::DragFloat("聚光衰减线性项", &spotLightLinear, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("聚光衰减二次项", &spotLightQuadratic, 0.001f, 0.f, 2.f);
                    ImGui::Unindent();
                }
            }

            // 自发光设置
            if (ImGui::CollapsingHeader("自发光设置", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("开启自发光", &emissionEnable);
                if (emissionEnable)
                {
                    ImGui::Indent();
                    ImGui::Checkbox("Move Emission", &emissionMove);
                    ImGui::Checkbox("Flicker Emission", &emissionFlick);
                    ImGui::SliderFloat("Emission Strength", &emissionStrength, 0.f, 5.f);
                    ImGui::Unindent();
                }
            }

            // 其他
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("其他设置");
            ImGui::ColorEdit4("Background Color", (float *)&clear_color);
            ImGui::Text("FPS: %d", int(io.Framerate));

            ImGui::End();
        }
        glClear(GL_DEPTH_BUFFER_BIT);   // 把深度重置到 1.0
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


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

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void processInput(GLFWwindow *window)
{
    // 退出
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // 相机移动
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

    // 光标锁定/解锁切换--> 相机控制 / imgui输入
    static bool tPressedLastFrame = false;
    bool tPressed = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
    // 只在按键从释放变为按下时触发（边缘检测）
    if (tPressed && !tPressedLastFrame)
    {
        if (cursorLocked)
        { // 解锁光标，用于imgui输入
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 锁定光标，用于控制fps摄像机
            firstMouse = true;                                           // 防止恢复锁定时的位置偏移导致的视角跳动
        }
        cursorLocked = !cursorLocked;
    }
    tPressedLastFrame = tPressed;
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (!cursorLocked)
    {                                                         // 光标解锁时不处理相机旋转
        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos); // 给 ImGui
        return;
    }
    // 相机旋转
    if (firstMouse)
    {
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

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (!cursorLocked)
    { // 光标解锁时不处理相机，控制imgui
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
        return;
    }
    camera.ProcessMouseScroll(yoffset);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (!cursorLocked)
    { // 仅光标解锁时允许点击imgui
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        return;
    }
}