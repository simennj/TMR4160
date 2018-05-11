cmake_minimum_required(VERSION 3.5)
project(TMR4160 C)

set(CMAKE_C_STANDARD 99)

find_package(OpenGL REQUIRED)

INCLUDE_DIRECTORIES(include)

add_library(glad include/glad/glad.c)

LINK_DIRECTORIES(external)

add_executable(TMR4160
        src/main.c
        src/pid.c
        src/boat.c
        src/logger.c
        src/fake_boat.c
        src/utils/clamp.c
        src/utils/file_utils.c
        src/utils/phidget_utils.c
        src/graphics/ui.c
        src/graphics/graph.c
        src/graphics/marker.c
        src/graphics/graphics.c
        src/graphics/window_util.c
        src/graphics/shader_util.c
        )

TARGET_LINK_LIBRARIES(TMR4160
        phidget22
        glfw3
        glad
        ${OPENGL_LIBRARY}
        )
