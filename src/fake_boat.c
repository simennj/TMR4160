#include <stdio.h>
#include "fake_boat.h"

double drag = 1;

double position = 1;
double velocity = 0;
double acceleration = 0;


double getBoatPosition() {
    return position;
}

void updateBoat(double dt, double motor_force) {
    acceleration = motor_force - drag * velocity;
    velocity = velocity + acceleration * dt;
    position = position + velocity * dt;
}

void printFakeBoatState() {
    printf("Position: %f; Velocity: %f; Acceleration: %f\n", position, velocity, acceleration);
}
