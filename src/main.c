#include <stdio.h>

#include <time.h>

#include "fake_boat.h"

int main(int argc, char **argv) {

    double k_p = 1;
    double k_i = 1;
    double k_d = 1;

    double target_position = 0;

    double dt;
    struct timespec now_time, last_time;
    clock_gettime(CLOCK_MONOTONIC, &last_time);
    for (int i = 0; i < 1000;) {
        clock_gettime(CLOCK_MONOTONIC, &now_time);
        dt = now_time.tv_sec - last_time.tv_sec + (now_time.tv_nsec - last_time.tv_nsec) / 1E9;
        if (dt > .01) {
            i++;
            last_time = now_time;
            double displacement = target_position - getFakeBoatPosition();
            double motor_force = k_p * displacement;
            updateFakeBoat(dt, motor_force);
            if (i % 5 == 0) {
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

