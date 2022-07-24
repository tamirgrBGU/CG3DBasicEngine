### cmake_minimum_required(VERSION 3.8)

message(STATUS "Creating target: engine")

project(engine)

set(ENGINE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../engine")

file(GLOB ENGINE_SOURCES
	"${ENGINE_ROOT}/*.cpp"
	"${ENGINE_ROOT}/*.h*"
    )

add_library(engine STATIC ${ENGINE_SOURCES})

set_property(TARGET engine PROPERTY EXPORT_NAME engine)

set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(engine PROPERTIES FOLDER "lib")

target_include_directories(engine PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/igl"
	"${CMAKE_CURRENT_LIST_DIR}/igl/opengl"
	"${CMAKE_CURRENT_LIST_DIR}/igl/opengl/glfw"
)
