#include "boat.h"
#include "clamp.h"

#include <phidget22.h>
#include <stdio.h>
#include <stdlib.h>


PhidgetRCServoHandle servoHandle;
PhidgetVoltageInputHandle voltageInputHandle;

static void CCONV
onVoltageChangeHandler(PhidgetVoltageInputHandle ch, void *ctx, double voltage) {
    printf("Voltage Changed: %.4f\n", voltage);
}

int initPhidgetSensor() {
    PhidgetReturnCode res;
    const char *errs;

    res = PhidgetVoltageInput_create(&voltageInputHandle);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to create voltage input channel\n");
        return EXIT_FAILURE;
    }

    res = Phidget_openWaitForAttachment((PhidgetHandle) voltageInputHandle, 5000);
    if (res != EPHIDGET_OK) {
        if (res == EPHIDGET_TIMEOUT) {
            printf("Channel did not attach after 5 seconds: please check that the device is attached\n");
            return EXIT_FAILURE;
        } else {
            Phidget_getErrorDescription(res, &errs);
            fprintf(stderr, "failed to open channel:%s\n", errs);
            return EXIT_FAILURE;
        }
    }

    res = PhidgetVoltageInput_setOnVoltageChangeHandler(voltageInputHandle, onVoltageChangeHandler, NULL);
    if (res != EPHIDGET_OK) {
        Phidget_getErrorDescription(res, &errs);
        fprintf(stderr, "failed to set voltage change handler: %s\n", errs);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int initPhidgetMotor() {
    PhidgetReturnCode res;
    const char *errs;

    res = PhidgetRCServo_create(&servoHandle);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to create rc servo channel\n");
        return EXIT_FAILURE;
    }

    /*
    * Open the channel synchronously: waiting a maximum of 5 seconds.
    */
    res = Phidget_openWaitForAttachment((PhidgetHandle) servoHandle, 5000);
    if (res != EPHIDGET_OK) {
        if (res == EPHIDGET_TIMEOUT) {
            printf("Channel did not attach after 5 seconds: please check that the device is attached\n");
            return EXIT_FAILURE;
        } else {
            Phidget_getErrorDescription(res, &errs);
            fprintf(stderr, "failed to open channel:%s\n", errs);
            return EXIT_FAILURE;
        }
    }

    PhidgetRCServo_setTargetPosition(servoHandle, 90);
    printf("Setting engaged\n");
    res = PhidgetRCServo_setEngaged(servoHandle, 1);
    if (res != EPHIDGET_OK) {
        printf("failed to set engaged\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
int boat_initPhidget() {
    int res;

    PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);

    res = initPhidgetSensor();
    if (res != EXIT_SUCCESS) {
        fprintf(stderr, "failed to init sensor\n");
        return res;
    }

    res = initPhidgetMotor();
    if (res != EXIT_SUCCESS) {
        fprintf(stderr, "failed to init motor\n");
        return res;
    }

    return 0;
}

#define V_MAX 5
#define V_MIN 3.5
#define V_RADIUS ((V_MAX/V_MIN)/2)

double boat_getPosition() {
    double voltage;
    PhidgetVoltageInput_getVoltage(voltageInputHandle, &voltage);
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
    double motor_input = motor_force * 50 + 119;
    printf("Setting motor input to %f\n", motor_input);
    fflush(stdout);
    if (PhidgetRCServo_setTargetPosition(servoHandle, motor_input) != EPHIDGET_OK) {
        printf("Motor input set to %f\n", motor_input);
    }
}
