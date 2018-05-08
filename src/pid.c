#include <stdio.h>
#include <pthread_time.h>
#include <stdlib.h>
#include "pid.h"
#include "boat.h"

double k_p, k_i, k_d;
double targetPosition;
double accumulatedDisplacement;
double dt;
struct timespec nowTime, lastTime;

double displacement;
double displacementVelocity;
double motorForce;

void pid_init(double newK_p, double newK_i, double newK_d, double newTargetPosition) {
    k_p = newK_p;
    k_i = newK_i;
    k_d = newK_d;
    targetPosition = newTargetPosition;

    clock_gettime(CLOCK_MONOTONIC, &lastTime);
}

void init_phidget() {
    int res;
    res = boat_initPhidget();
    if (res != EXIT_SUCCESS) {
        fprintf(stderr, "failed to init phidget\n");
    }
}

void pid_update() {
    clock_gettime(CLOCK_MONOTONIC, &nowTime);
    dt = nowTime.tv_sec - lastTime.tv_sec + (nowTime.tv_nsec - lastTime.tv_nsec) / 1E9;
    if (dt <= 0) return;
    lastTime = nowTime;
    double last_displacement = displacement;
    displacement = targetPosition - boat_getPosition();
    accumulatedDisplacement = accumulatedDisplacement + displacement * dt;
    displacementVelocity = (displacement - last_displacement) / dt;
    motorForce = k_p * displacement + k_i * accumulatedDisplacement + k_d * displacementVelocity;
    boat_update(dt, motorForce);
//    printFakeBoatState();
}

struct boatState pid_getBoatState() {
    struct boatState state = {
            (float) boat_getPosition(),
            (float) displacementVelocity,
            (float) motorForce
    };
    return state;
}
