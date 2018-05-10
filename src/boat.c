#include "boat.h"
#include "clamp.h"

#include <stdio.h>
#include "phidget_utils.h"

double motorCenter = 111, motorRadius = 50;

void boat_setMotorValues(double center, double radius) {
    motorCenter = center;
    motorRadius = radius;
}

#define V_MAX 5
#define V_MIN 3.5
#define V_RADIUS ((V_MAX/V_MIN)/2)

double boat_getPosition() {
    double voltage;
    voltage = phidget_getVoltage(&voltage);
    // center around 0 with a radius of 1 (range -1 to 1)
    double position = (V_MAX - voltage) / V_RADIUS - 1;
    printf("calculated position %f\n", position);
    fflush(stdout);
    return position;
}

void boat_update(double motor_force) {
    printf("received: %f\t", motor_force);
    motor_force = clamp(motor_force, -1, 1);
    printf("used: %f\n", motor_force);
    double motor_input = motor_force * motorRadius + motorCenter;
    printf("Setting motor input to %f\n", motor_input);
    fflush(stdout);
}

void boat_toggleMotor() {
    static int enabled = 1;
    phidget_setMotorValue(90);
    phidget_setMotorState(enabled);
    enabled = !enabled;
}

int boat_init() {
    return phidget_init();
}

