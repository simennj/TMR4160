#include <stdio.h>
#include <GLFW/glfw3.h>
#include "time.h"
#include "file_utils.h"
#include "graphics.h"
#include "window_util.h"
#include "pid.h"

double targetPosition;

void init_pid() {
    double k_p, k_i, k_d, motorCenter, motorRadius;
    loadConstants("constants.txt", &k_p, &k_i, &k_d, &targetPosition, &motorCenter, &motorRadius);
    printf("K_p: %lf, K_i: %lf, K_d: %lf\n", k_p, k_i, k_d);
    pid_init(k_p, k_i, k_d, motorCenter, motorRadius);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_R:
                graphics_reload();
                init_pid();
                break;
            case GLFW_KEY_RIGHT:
                targetPosition += .1;
                break;
            case GLFW_KEY_LEFT:
                targetPosition -= .1;
                break;
            default:
                break;
        }
    }
}

int main(int argc, char **argv) {
    init_phidget();
    init_pid();

    window_init(key_callback);
    graphics_init((void *(*)(const char)) glfwGetProcAddress);

    double dt;
    struct timespec nowTime, lastTime;

    clock_gettime(CLOCK_MONOTONIC, &lastTime);
    while (window_open()) {
        clock_gettime(CLOCK_MONOTONIC, &nowTime);
        dt = nowTime.tv_sec - lastTime.tv_sec + (nowTime.tv_nsec - lastTime.tv_nsec) / 1E9;
        if (dt <= 0.01) continue;
        lastTime = nowTime;
        struct pid_state structboatState = pid_update(dt, targetPosition);
        graphics_updateGraph(structboatState.boatPosition, structboatState.estimatedBoatVelocity,
                             structboatState.pidResultForce);
        graphics_draw(structboatState.boatPosition, (GLfloat) targetPosition);
        window_update();
    }

    return 0;
}

