cmake_minimum_required(VERSION 3.28.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(FIRST_ATTEMPT_OPENGL)

add_definitions(-DGL_SILENCE_DEPRECATION)

# GLFW
include(FetchContent)
FetchContent_Declare(
	GLFW
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG 3.4
)
FetchContent_MakeAvailable(GLFW)

# GLM
include(FetchContent)
FetchContent_Declare(
	GLM
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(GLM)

# GLAD
add_library(GLAD external/glad/src/gl.c)
target_include_directories(GLAD PUBLIC external/glad/include)

# OPEnGL 
find_library(OpenGL_LIBRARY OpenGL)

# Silence Warnings 
# find_package(Doxygen QUIET)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(FIRST_ATTEMPT_OPENGL ${SOURCES})
target_include_directories(FIRST_ATTEMPT_OPENGL PRIVATE src) 
target_link_libraries(FIRST_ATTEMPT_OPENGL GLAD glfw glm ${OpenGL_LIBRARY})

