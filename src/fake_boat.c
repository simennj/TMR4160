#include <stdio.h>
#include "clamp.h"
#include "fake_boat.h"

double position = .5;
double velocity = 0;
double acceleration = 0;


double fakeBoat_getPosition() {
    return position;
}

/*
 * Calculates position and velocity change based on acceleration and velocity from last call and time elapsed since.
 * Then updates the acceleration based on the input.
 * All values are clamped to match the control of the real boat better
 */
void fakeBoat_update(double dt, double motor_force) {
    position = clamp(position + velocity * dt, -1, 1);
    velocity = clamp(velocity + acceleration * dt, -1, 1);
    acceleration = clamp(motor_force, -1, 1);
}

// Helper function used when debugging PID controller
void fakeBoat_printState() {
    printf("Position: %f; Velocity: %f; Acceleration: %f\n", position, velocity, acceleration);
}
