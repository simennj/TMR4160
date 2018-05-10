#include "boat.h"
#include "clamp.h"

#include <stdio.h>
#include "phidget_utils.h"

// Defines constants found from testing the potentiometer
#define V_MAX 5
#define V_MIN 3.5
#define V_RADIUS ((V_MAX/V_MIN)/2)

// Constants found from testing the motor, can be changed during runtime because they seem to change from day to day
double motorCenter = 111, motorRadius = 50;

/*
 * Set new motor constants
 * center: the motor input between going forward and backward
 * radius: how much larger/smaller a value can be without the motor stopping
 */
void boat_setMotorValues(double center, double radius) {
    motorCenter = center;
    motorRadius = radius;
}

/*
 * Retrieve a position between -1 and 1 by reading the potentiometer
 */
double boat_getPosition() {
    double voltage;
    phidget_getVoltage(&voltage);
    // center around 0 with a radius of 1 (range -1 to 1)
    return (V_MAX - voltage) / V_RADIUS - 1;
}

/*
 * Change the force applied by the motor
 * motor_force: the force that should be applied, -1 is max force backwards and 1 is max force forward,
 */
void boat_update(double motor_force) {
    printf("received: %f\t", motor_force);
    // Clamp the value to be between -1 and 1 to avoid going outside the motorRadius, which would stop the motor
    motor_force = clamp(motor_force, -1, 1);
    printf("used: %f\n", motor_force);
    // Calculate the motor input based on the force wanted and the motor constants
    double motor_input = motor_force * motorRadius + motorCenter;
    printf("Setting motor input to %f\n", motor_input);
    fflush(stdout);
}

/*
 * Toggle motor on/off
 */
void boat_toggleMotor() {
    static int enabled = 1;
    phidget_setMotorValue(90); // Have to set a motor value before enabling
    phidget_setMotorState(enabled);
    enabled = !enabled;
}

/*
 * Initializes the phidget module used to control the boat
 */
int boat_init() {
    return phidget_init();
}
