# ImGui 使用指南

ImGui（Immediate Mode GUI）是一个轻量级的即时模式图形用户界面库，专为3D图形应用程序和游戏开发设计。它允许开发者快速创建调试界面、编辑器工具和可视化控件。

## 初始化

在使用ImGui之前，需要进行初始化设置：

```cpp
// Setup Dear ImGui context
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO(); (void)io;
io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

// Setup Platform/Renderer backends
ImGui_ImplGlfw_InitForOpenGL(window, true);
ImGui_ImplOpenGL3_Init(glsl_version);

// 设置样式
ImGui::StyleColorsDark(); // 或 ImGui::StyleColorsLight() / ImGui::StyleColorsClassic()
```

## 主循环集成

在主渲染循环中，需要按以下顺序整合ImGui：

```cpp
while (!glfwWindowShouldClose(window)) {
    // 处理输入和更新逻辑
    
    // 清除屏幕
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 渲染3D场景
    // ... 你的OpenGL渲染代码 ...
    
    // 开始ImGui新帧
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // 创建UI界面
    // ... ImGui UI代码 ...
    
    // 渲染ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // 交换缓冲区
    glfwSwapBuffers(window);
}
```

## 基本控件使用

### 窗口创建

```cpp
ImGui::Begin("窗口标题");
// 在这里添加控件
ImGui::End();
```

### 文本显示

```cpp
ImGui::Text("普通文本");
ImGui::TextColored(ImVec4(1,0,0,1), "红色文本");
ImGui::TextDisabled("禁用状态文本");
ImGui::TextWrapped("自动换行文本");
```

### 按钮

```cpp
if (ImGui::Button("点击按钮")) {
    // 按钮被点击时执行的代码
}

// 带大小的按钮
if (ImGui::Button("大按钮", ImVec2(100, 30))) {
    // 按钮被点击时执行的代码
}
```

### 滑动条

```cpp
static float floatValue = 0.0f;
ImGui::SliderFloat("浮点滑动条", &floatValue, 0.0f, 1.0f);

static int intValue = 0;
ImGui::SliderInt("整数滑动条", &intValue, 0, 100);
```

### 复选框

```cpp
static bool checkBoxValue = false;
ImGui::Checkbox("启用功能", &checkBoxValue);
```

### 颜色选择器

```cpp
static ImVec4 color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
ImGui::ColorEdit3("清除颜色", (float*)&color);
```

## 实际使用示例

参考 [04_02_with_imgui.cpp](04_02_with_imgui.cpp) 文件中的示例：

### 创建控制面板

```cpp
{
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("控制面板");                          
    ImGui::Text("应用程序信息");               
    ImGui::Checkbox("显示演示窗口", &show_demo_window);      
    
    ImGui::SliderFloat("混合值", &mixValue, 0.0f, 1.0f);            
    ImGui::ColorEdit3("背景颜色", (float*)&clear_color); 
    
    if (ImGui::Button("计数器按钮"))                            
        counter++;
    
    ImGui::SameLine(); // 将下一个控件放在同一行
    ImGui::Text("计数器值 = %d", counter);
    
    ImGui::Text("平均 %.3f ms/帧 (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}
```

### 控制光照参数

你可以扩展ImGui界面来控制光照参数：

```cpp
// 在渲染循环中添加光照控制窗口
ImGui::Begin("光照控制");
ImGui::SliderFloat("环境光强度", &ambientStrength, 0.0f, 1.0f);
ImGui::SliderFloat("漫反射强度", &diffuseStrength, 0.0f, 1.0f);
ImGui::SliderFloat("镜面反射强度", &specularStrength, 0.0f, 1.0f);
ImGui::SliderFloat("镜面反射指数", &specularExponent, 1.0f, 128.0f);
ImGui::SliderFloat("常数衰减", &constant, 0.0f, 1.0f);
ImGui::SliderFloat("一次衰减", &linear, 0.0f, 1.0f);
ImGui::SliderFloat("二次衰减", &quadratic, 0.0f, 1.0f);
ImGui::End();
```

然后将这些值传递给着色器：

```cpp
// 在着色器使用前设置uniform变量
shader.setFloat("ambientStrength", ambientStrength);
shader.setFloat("diffuseStrength", diffuseStrength);
shader.setFloat("specularStrength", specularStrength);
shader.setFloat("specularExponent", specularExponent);
shader.setFloat("constant", constant);
shader.setFloat("linear", linear);
shader.setFloat("quadratic", quadratic);
```

## 布局控制

### 同行放置控件

```cpp
ImGui::Text("文本1");
ImGui::SameLine(); // 下一个控件在同一行
ImGui::Text("文本2");
ImGui::SameLine();
ImGui::Button("按钮");
```

### 分组

```cpp
ImGui::BeginGroup();
ImGui::Button("按钮1");
ImGui::Button("按钮2");
ImGui::Button("按钮3");
ImGui::EndGroup();
```

## 清理资源

程序结束前需要清理ImGui资源：

```cpp
ImGui_ImplOpenGL3_Shutdown();
ImGui_ImplGlfw_Shutdown();
ImGui::DestroyContext();
```

## 最佳实践

1. **性能**: ImGui每帧重建整个界面，避免在UI代码中进行重计算
2. **状态保持**: 使用static变量保存UI状态
3. **条件渲染**: 只在需要时才渲染复杂的UI元素
4. **命名空间**: 为不同功能的UI使用不同的窗口
5. **响应式设计**: 利用ImGui的自动布局特性适应不同分辨率

## 常见问题

### 与GLFW输入冲突

当使用ImGui时，自定义的回调会覆盖ImGUI的回调。可以通过以下方式解决：

```cpp
// 在创建ImGui上下文后，将GLFW的回调设置为你的处理函数
glfwSetCursorPosCallback(window, mouse_callback);
glfwSetScrollCallback(window, scroll_callback);

// 在回调函数中手动传递给ImGui
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // 
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos); // 给 ImGui
}
```
