#include <stdio.h>
#include <stdlib.h>
#include "pid.h"
#include "boat.h"
#include "fake_boat.h"

static double k_p, k_i, k_d;

static double accumulatedDisplacement;

enum boatUsed {
    FAKE_BOAT = 0, BOAT = 1, NOT_SET = 2
};
static enum boatUsed boat = NOT_SET;

/*
 * Initializes the PID controller with the given values
 */
void pid_init(double newK_p, double newK_i, double newK_d, double motorCenter, double motorRadius) {
    k_p = newK_p;
    k_i = newK_i;
    k_d = newK_d;

    // If the boat is not set, try to use the real boat through phidget
    if (boat == NOT_SET) {
        int res;
        res = boat_init();
        if (res == EXIT_SUCCESS) {
            boat = BOAT;
        } else {
            // Use the fake boat if the phidget connection fails
            fprintf(stderr, "failed to init phidget, using fake boat instead\n");
            boat = FAKE_BOAT;
        }
    }
    if (boat) {
        // Toggle the motor if the real boat is used
        boat_toggleMotor();
        boat_setMotorValues(motorCenter, motorRadius);
    }
}

static double getPosition() {
    if (boat) return boat_getPosition();
    return fakeBoat_getPosition();
}

static void update(double dt, double motorForce) {
    if (boat) boat_update(motorForce);
    else fakeBoat_update(dt, motorForce);
}

struct pid_state pid_update(double dt, double targetPosition) {
    static double displacement;
    double motorForce;
    double displacementVelocity;

    // Save error from last call
    double last_displacement = displacement;
    // Calculate new error
    displacement = targetPosition - getPosition();
    // Update the error sum
    accumulatedDisplacement = accumulatedDisplacement + displacement * dt;
    // Calculate the new error velocity
    displacementVelocity = (displacement - last_displacement) / dt;

    // Multiply with constants to get the contribution from each of the P, I and D parts
    double pForce = k_p * displacement;
    double iForce = k_i * accumulatedDisplacement;
    double dForce = k_d * displacementVelocity;

    // Calculate the resulting regulation force
    motorForce = pForce + iForce + dForce;
    // Apply the calculated force
    update(dt, motorForce);
    // Return the current state for drawing and logging
    struct pid_state state = {
            .boatPosition = (float) getPosition(),
            .pid_pForce = (float) pForce,
            .pid_iForce = (float) iForce,
            .pid_dForce = (float) dForce,
            .pidResultForce = (float) motorForce,
    };
    return state;
}
