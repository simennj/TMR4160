#include <stdio.h>
#include <math.h>
#include "utils/clamp.h"
#include "fake_boat.h"

#define DRAG .5
double position = .0;
double velocity = .5;
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
    velocity = clamp(velocity * pow(DRAG, dt) + acceleration * dt, -1, 1);
    acceleration = clamp(motor_force, -1, 1);
}

// Helper function used when debugging PID controller
void fakeBoat_printState() {
    printf("Position: %f; Velocity: %f; Acceleration: %f\n", position, velocity, acceleration);
}
