#include <stdio.h>
#include "fake_boat.h"

double position = .5;
double velocity = 0;
double acceleration = 0;


double getBoatPosition() {
    return position;
}

double clamp(double value, double min, double max) {
    if (value > max) return max;
    if (value < min) return min;
    return value;
}

void updateBoat(double dt, double motor_force) {
    acceleration = clamp(motor_force, -1, 1);
    velocity = clamp(velocity + acceleration * dt, -1, 1);
    position = position + velocity * dt;
}

void printFakeBoatState() {
    printf("Position: %f; Velocity: %f; Acceleration: %f\n", position, velocity, acceleration);
}
