#include <stdio.h>
#include <stdlib.h>
#include "pid.h"
#include "boat.h"
#include "fake_boat.h"

double k_p, k_i, k_d;
enum boatUsed {
    FAKE_BOAT = 0, BOAT = 1, NOT_SET = 2
};
enum boatUsed boat = NOT_SET;

void pid_init(double newK_p, double newK_i, double newK_d, double motorCenter, double motorRadius) {
    k_p = newK_p;
    k_i = newK_i;
    k_d = newK_d;

    if (boat == NOT_SET) {
        int res;
        res = boat_init();
        if (res == EXIT_SUCCESS) {
            boat = BOAT;
        } else {
            fprintf(stderr, "failed to init phidget, using fake boat instead\n");
            boat = FAKE_BOAT;
        }
    }
    if (boat) {
        boat_toggleMotor();
        boat_setMotorValues(motorCenter, motorRadius);
    }
}

double getPosition() {
    if (boat) return boat_getPosition();
    return fakeBoat_getPosition();
}

void update(double dt, double motorForce) {
    if (boat) boat_update(motorForce);
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
