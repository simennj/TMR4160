#include <stdio.h>

#include <GLFW/glfw3.h>

#include "file_utils.h"
#include "graphics.h"
#include "window_util.h"
#include "pid.h"

void init_pid() {
    double k_p, k_i, k_d, targetPosition;
    loadConstants("constants.txt", &k_p, &k_i, &k_d, &targetPosition);
    printf("K_p: %lf, K_i: %lf, K_d: %lf\n", k_p, k_i, k_d);
    pid_init(k_p, k_i, k_d, targetPosition);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        graphics_reload();
        init_pid();
    }
}

int main(int argc, char **argv) {

    init_pid();

    window_init(key_callback);
    graphics_init(glfwGetProcAddress);


    while (window_open()) {
        pid_update();
        graphics_setBoatPosition(pid_getBoatState().position);
        graphics_update();
        window_update();
    }

//    int res;
//    res = initPhidget();
//    if (res != EXIT_SUCCESS) {
//        fprintf(stderr, "failed to init phidget\n");
//        return res;
//    }

    // unzødvendig å close? https://www.phidgets.com/docs/Phidget_Programming_Basics

    return 0;
}

