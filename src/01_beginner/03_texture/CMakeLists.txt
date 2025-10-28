# 03_texture

add_executable(01_texture 01_texture.cpp)
target_link_libraries(01_texture OpenGLCommon)

# 复制shader文件到构建目录
file(COPY 1.vert 1.frag 2.frag DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# 纹理资源文件
file(GLOB textures ${ASSETS_DIR}/01_beginner/textures/*)
file(COPY ${textures} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/01_beginner/textures)