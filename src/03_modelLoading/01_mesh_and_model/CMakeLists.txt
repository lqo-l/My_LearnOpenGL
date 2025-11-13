add_executable(030101_mesh_and_model 030101_mesh_and_model.cpp)
target_link_libraries(030101_mesh_and_model OpenGLCommon)

file(GLOB shaders "*.vert" "*.frag")
file(COPY ${shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


file(COPY ${ASSETS_DIR}/03_modelLoading/GuitarBackpack/obj
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/03_modelLoading/GuitarBackpack)

# file(COPY ${ASSETS_DIR}/03_modelLoading/GuitarBackpack/glb
     # DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/03_modelLoading/GuitarBackpack)

# file(COPY ${ASSETS_DIR}/03_modelLoading/noload_haiseyin/obj
#      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/03_modelLoading/noload_haiseyin)

file(COPY ${ASSETS_DIR}/03_modelLoading/noload_funingna/obj
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/03_modelLoading/noload_funingna)

# dll
file(COPY ${CMAKE_SOURCE_DIR}/lib/libassimp-6.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})