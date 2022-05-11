get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME})
#configure_file("${PROJECT_NAME}/configuration.txt" "${PROJECT_NAME}/../configuration.txt")
#file(GLOB SANDBOX ${PROJECT_NAME}/*.cpp ${PROJECT_NAME}/*.h)
#add_executable(${PROJECT_NAME} ${SANDBOX})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}) # set the binary directory to build folder
set (VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}) # set the binary directory to build folder
add_executable(${PROJECT_NAME}_bin Assignment2.cpp Assignment2.h main.cpp InputManager.h sceneParser.cpp sceneParser.h)
target_link_libraries(${PROJECT_NAME}_bin igl::core igl::opengl igl::opengl_glfw igl::opengl_glfw_imgui igl::png)
