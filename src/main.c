#include <stdio.h>

#include <time.h>

#include "file_utils.h"
#include "pid.h"
#include "graphics.h"

int main(int argc, char **argv) {
    graphics_init();

    double k_p, k_i, k_d, target_position = 0;
    loadConstants("constants.txt", &k_p, &k_i, &k_d);
    printf("K_p: %lf, K_i: %lf, K_d: %lf\n", k_p, k_i, k_d);
    pid_init(k_p, k_i, k_d, target_position);

    while (graphics_open()) {
        pid_update();
        graphics_setBoatPosition(pid_getBoatState().position);
        graphics_update();
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

