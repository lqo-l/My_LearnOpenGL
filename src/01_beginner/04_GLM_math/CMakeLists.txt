# 04_GLM_math

# 示例1: 变换
add_executable(01_transform 01_transform.cpp)
target_link_libraries(01_transform OpenGLCommon)

# 示例2: 3D
add_executable(02_3D 02_3D.cpp)
target_link_libraries(02_3D OpenGLCommon)

# 示例3: 立方体
add_executable(03_cubic 03_cubic.cpp)
target_link_libraries(03_cubic OpenGLCommon)

# 复制shader文件到构建目录
file(GLOB shaders *.vert *.frag)
file(COPY ${shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# 复制纹理文件到构建目录
file(GLOB textures ${ASSETS_DIR}/01_beginner/textures/*)
file(COPY ${textures} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/01_beginner/textures)