#include <GLFW/glfw3.h>
#include <stdio.h>
#include "window_util.h"

const GLuint WIDTH = 800;
const GLuint HEIGHT = 800;
GLFWwindow *window;

void error_callback(int code, const char *description) {
    printf("%i: ", code);
    printf(description);
    printf("\n");
}

void resize_callback(GLFWwindow *window, int width, int height) {
    if (height >= width) glViewport(0, (height - width) / 2, width, width);
    else glViewport((width - height) / 2, 0, height, height);
}

void window_init(void (*key_callback)(GLFWwindow *window, int key, int scancode, int action, int mode)) {
    glfwSetErrorCallback(error_callback);

    glfwInit();
    // Set required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a window object
    window = glfwCreateWindow(WIDTH, HEIGHT, "Dynamic Positioning", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, resize_callback);
    glfwSetKeyCallback(window, key_callback);

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
}

void window_update() {
    glfwPollEvents();
    glfwSwapBuffers(window);
}

int window_open() {
    return !glfwWindowShouldClose(window);
}
