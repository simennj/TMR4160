#include <stdio.h>

#include <time.h>

#include "fake_boat.h"

int loadConstants(const char *filename, double *k_p, double *k_i, double *k_d) {
    FILE *file = fopen(filename, "r");
    int res = fscanf(file, "%lf %lf %lf", k_p, k_i, k_d);
    fclose(file);
    return res;
}

int main(int argc, char **argv) {

    double k_p, k_i, k_d;
    loadConstants("constants.txt", &k_p, &k_i, &k_d);
    printf("K_p: %lf, K_i: %lf, K_d: %lf\n", k_p, k_i, k_d);

    double target_position = 0;
    double accumulated_displacement = 0;
    double last_displacement = target_position - getBoatPosition();

    double dt;
    struct timespec now_time, last_time;
    clock_gettime(CLOCK_MONOTONIC, &last_time);
    for (int i = 0; i < 6000;) {
        clock_gettime(CLOCK_MONOTONIC, &now_time);
        dt = now_time.tv_sec - last_time.tv_sec + (now_time.tv_nsec - last_time.tv_nsec) / 1E9;
        if (dt > .01) {
            i++;
            last_time = now_time;
            double displacement = target_position - getBoatPosition();
            accumulated_displacement = accumulated_displacement + displacement * dt;
            double displacement_velocity = (displacement - last_displacement) / dt;
            double motor_force = k_p * displacement + k_i * accumulated_displacement + k_d * displacement_velocity;
            updateBoat(dt, motor_force);
            if (i % 10 == 0) {
                printFakeBoatState();
            }
        }
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

