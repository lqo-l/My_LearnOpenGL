add_executable(01_material 01_material.cpp)
target_link_libraries(01_material OpenGLCommon)

file(GLOB shaders "*.vert" "*.frag")
file(COPY ${shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
