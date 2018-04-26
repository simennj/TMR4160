cmake_minimum_required(VERSION 3.9)
project(TMR4160 C)

set(CMAKE_C_STANDARD 99)

find_package(OpenGL REQUIRED)

INCLUDE_DIRECTORIES(include)

add_library(glad include/glad/glad.c)

#LINK_DIRECTORIES(external)

add_executable(TMR4160 src/main.c src/graphics.c src/fake_boat.c src/file_utils.c)

TARGET_LINK_LIBRARIES(TMR4160
        glfw
        glad
        ${OPENGL_LIBRARY}
)
