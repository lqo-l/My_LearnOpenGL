# 02_shader

# 示例1: shader基础
add_executable(01_shaderBeginner 01_shaderBeginner.cpp)
target_link_libraries(01_shaderBeginner OpenGLCommon)

# 示例2: 使用Shader类
add_executable(02_useShaderClass 02_useShaderClass.cpp)
target_link_libraries(02_useShaderClass OpenGLCommon)

# 复制shader文件到构建目录
file(COPY 1.vert 1.frag DESTINATION ${CMAKE_CURRENT_BINARY_DIR})