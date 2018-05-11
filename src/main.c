#include <stdio.h>
#include <GLFW/glfw3.h>
#include "time.h"
#include "utils/file_utils.h"
#include "graphics/graphics.h"
#include "graphics/window_util.h"
#include "pid.h"
#include "logger.h"

/*
 * The position the PID controller will try to move the boat to.
 * -1 represents the start of the tank, 1 the end of the tank.
 * Is read from a file at start or when the R key is pressed, but can also be changed with the arrow keys.
 */
double targetPosition;

/*
 * Initializes the pid controller, can be called again to reinitialize with new values from the constants file
 */
void init_pid() {
    // Retrieve constants from file
    double k_p, k_i, k_d, motorCenter, motorRadius;
    loadConstants("constants.txt", &k_p, &k_i, &k_d, &targetPosition, &motorCenter, &motorRadius);
    // Initialize the PID controller with the new values
    pid_init(k_p, k_i, k_d, motorCenter, motorRadius);
}

/*
 * Handles all user input, is called from window_utils.c
 */
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                // Closes the window which quits the program
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_R:
                // Reloads shaders and ui points from shader, vertices and indices files
                graphics_reload();
                // Reinitializes PID controller with values from constants file
                init_pid();
                break;
            case GLFW_KEY_UP:
                // Move target position in tank forward
                targetPosition += .1;
                break;
            case GLFW_KEY_DOWN:
                // Move target position in tank backward
                targetPosition -= .1;
                break;
            default:
                break;
        }
    }
}

/*
 * The main function
 */
int main(int argc, char **argv) {
    // Initializes main modules
    init_pid();
    window_init(key_callback);
    graphics_init((void *(*)(const char)) glfwGetProcAddress);

    // Initializes local variables
    double dt;
    struct timespec nowTime, lastTime;
    clock_gettime(CLOCK_MONOTONIC, &lastTime);
    // Initializes logging module
    logger_init(lastTime.tv_sec);

    // Run while window is open
    while (window_open()) {
        // Calculate time since last frame (delta time)
        clock_gettime(CLOCK_MONOTONIC, &nowTime);
        dt = nowTime.tv_sec - lastTime.tv_sec + (nowTime.tv_nsec - lastTime.tv_nsec) / 1E9;
        if (dt <= 0.01) continue; // limit the program to 100fps (fixes problem with being too battery intensive)
        lastTime = nowTime;

        // Update pid
        struct pid_state structboatState = pid_update(dt, targetPosition);
        float graphValues[4] = {structboatState.pid_pForce, structboatState.pid_iForce, structboatState.pid_dForce,
                                structboatState.pidResultForce};
        // Draw updated values and handle input
        graphics_draw(structboatState.boatPosition, (GLfloat) targetPosition, graphValues);
        window_update();
        // Give updated values to logger for logging
        logger_update(&structboatState, targetPosition, dt);
    }

    // Safaly end logging to file
    logger_end();

    // Success
    return 0;
}
