# 05_camera

# 示例1: 摄像机旋转
add_executable(01_cameraRotate 01_cameraRotate.cpp)
target_link_libraries(01_cameraRotate OpenGLCommon)

# 示例2: 摄像机自由移动
add_executable(02_cameraFreeMove 02_cameraFreeMove.cpp)
target_link_libraries(02_cameraFreeMove OpenGLCommon)

# 示例3: 摄像机类版本
add_executable(03_camClassVer 03_camClassVer.cpp)
target_link_libraries(03_camClassVer OpenGLCommon)

# 复制shader文件到构建目录
file(GLOB shaders *.vert *.frag)
file(COPY ${shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# 复制纹理文件到构建目录
file(GLOB textures ${ASSETS_DIR}/01_beginner/textures/*)
file(COPY ${textures} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/01_beginner/textures)
