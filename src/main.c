#include <stdio.h>
#include <GLFW/glfw3.h>
#include "time.h"
#include "file_utils.h"
#include "graphics.h"
#include "window_util.h"
#include "pid.h"

void init_pid() {
    double k_p, k_i, k_d, targetPosition, motorCenter, motorRadius;
    loadConstants("constants.txt", &k_p, &k_i, &k_d, &targetPosition, &motorCenter, &motorRadius);
    printf("K_p: %lf, K_i: %lf, K_d: %lf\n", k_p, k_i, k_d);
    pid_init(k_p, k_i, k_d, targetPosition, motorCenter, motorRadius);
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
    init_phidget();
    init_pid();

    window_init(key_callback);
    graphics_init(glfwGetProcAddress);

    double dt;
    struct timespec nowTime, lastTime;

    clock_gettime(CLOCK_MONOTONIC, &lastTime);
    while (window_open()) {
        clock_gettime(CLOCK_MONOTONIC, &nowTime);
        dt = nowTime.tv_sec - lastTime.tv_sec + (nowTime.tv_nsec - lastTime.tv_nsec) / 1E9;
        if (dt <= 0.01) continue;
        lastTime = nowTime;
        pid_update(dt);
        graphics_updateValues(pid_getBoatState().position, pid_getBoatState().velocity,
                              pid_getBoatState().acceleration);
        graphics_draw();
        window_update();
    }

    return 0;
}

