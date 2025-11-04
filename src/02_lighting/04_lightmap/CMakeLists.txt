add_executable(0401_lightmap 0401_lightmap.cpp)
target_link_libraries(0401_lightmap OpenGLCommon)

file(GLOB shaders "*.vert" "*.frag")
file(COPY ${shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# 复制光照贴图文件到构建目录
file(GLOB textures ${ASSETS_DIR}/02_lighting/textures/*)
file(COPY ${textures} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/02_lighting/textures)