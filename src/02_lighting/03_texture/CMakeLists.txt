add_executable(01_texture 01_texture.cpp)
target_link_libraries(01_texture OpenGLCommon)

file(GLOB shaders "*.vert" "*.frag")
file(COPY ${shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

