# 02_basic_light

# 示例1: 基础光照
add_executable(01_basicLight 01_basicLight.cpp)
target_link_libraries(01_basicLight OpenGLCommon)

# 示例2: 基础光照(视图空间Phong)
add_executable(02_basicLight_view_phong 02_basicLight_view_phong.cpp)
target_link_libraries(02_basicLight_view_phong OpenGLCommon)

# 示例3: 基础光照(Gouraud着色)
add_executable(03_basicLight_Gouraud 03_basicLight_Gouraud.cpp)
target_link_libraries(03_basicLight_Gouraud OpenGLCommon)

# 示例4: 带ImGui的版本
add_executable(04_02_with_imgui 04_02_with_imgui.cpp)
target_link_libraries(04_02_with_imgui OpenGLCommon)

# 复制shader文件到构建目录
file(GLOB shaders "*.vert" "*.frag")
file(COPY ${shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# 复制纹理文件到构建目录
file(GLOB textures ${ASSETS_DIR}/02_lighting/textures/*)
file(COPY ${textures} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/02_lighting/textures)