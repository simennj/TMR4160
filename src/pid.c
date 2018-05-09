#include <stdio.h>
#include <stdlib.h>
#include "pid.h"
#include "boat.h"
#include "fake_boat.h"

double k_p, k_i, k_d;
int phidget = 1;

void pid_init(double newK_p, double newK_i, double newK_d, double motorCenter, double motorRadius) {
    k_p = newK_p;
    k_i = newK_i;
    k_d = newK_d;

    if (phidget) {
        boat_toggleMotor();
        boat_setMotorValues(motorCenter, motorRadius);
    }
}

void init_phidget() {
    int res;
    res = boat_initPhidget();
    if (res != EXIT_SUCCESS) {
        fprintf(stderr, "failed to init phidget, using fake boat instead\n");
        phidget = 0;
    }
}

double getPosition() {
    if (phidget) return boat_getPosition();
    return fakeBoat_getPosition();
}

void update(double dt, double motorForce) {
    if (phidget) boat_update(motorForce);
    else fakeBoat_update(dt, motorForce);
}

struct pid_state pid_update(double dt, double targetPosition) {
    static double accumulatedDisplacement;
    static double displacement;
    static double displacementVelocity;
    static double motorForce;

    double last_displacement = displacement;
    displacement = targetPosition - getPosition();
    accumulatedDisplacement = accumulatedDisplacement + displacement * dt;
    displacementVelocity = (displacement - last_displacement) / dt;

    double pForce = k_p * displacement;
    double iForce = k_i * accumulatedDisplacement;
    double dForce = k_d * displacementVelocity;

    motorForce = pForce + iForce + dForce;
    update(dt, motorForce);
//    printFakeBoatState();
    struct pid_state state = {
            .boatPosition = (float) getPosition(),
            .pid_pForce = (float) pForce,
            .pid_iForce = (float) iForce,
            .pid_dForce = (float) dForce,
            .pidResultForce = (float) motorForce,
    };
    return state;
}
