# 01_color

add_executable(01_color 01_color.cpp)
target_link_libraries(01_color OpenGLCommon)


# 复制shader文件到构建目录
file(GLOB shaders *.vert *.frag)
file(COPY ${shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# 复制纹理文件到构建目录
file(GLOB textures ${ASSETS_DIR}/02_lighting/textures/*)
file(COPY ${textures} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/02_lighting/textures)
