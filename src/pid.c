#include <stdio.h>
#include <stdlib.h>
#include "pid.h"
#include "boat.h"
#include "fake_boat.h"

double k_p, k_i, k_d;
double targetPosition;
int phidget = 1;

void pid_init(double newK_p, double newK_i, double newK_d, double newTargetPosition, double motorCenter,
              double motorRadius) {
    k_p = newK_p;
    k_i = newK_i;
    k_d = newK_d;
    targetPosition = newTargetPosition;

    if (phidget) {
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

struct pid_state pid_update(double dt) {
    static double accumulatedDisplacement;
    static double displacement;
    static double displacementVelocity;
    static double motorForce;

    double last_displacement = displacement;
    displacement = targetPosition - getPosition();
    accumulatedDisplacement = accumulatedDisplacement + displacement * dt;
    displacementVelocity = (displacement - last_displacement) / dt;
    motorForce = k_p * displacement + k_i * accumulatedDisplacement + k_d * displacementVelocity;
    update(dt, motorForce);
//    printFakeBoatState();
    struct pid_state state = {
            (float) getPosition(),
            (float) displacementVelocity,
            (float) motorForce,
            (float) targetPosition
    };
    return state;
}
