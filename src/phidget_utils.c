#include <phidget22.h>
#include <stdio.h>
#include <stdlib.h>
#include "phidget_utils.h"

PhidgetRCServoHandle servoHandle;
PhidgetVoltageInputHandle voltageInputHandle;

int openChannel(PhidgetHandle handle) {
    PhidgetReturnCode res;
    const char *errs;
    /*
    * Open the channel synchronously: waiting a maximum of 5 seconds.
    */
    res = Phidget_openWaitForAttachment(handle, 5000);
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
    return EXIT_SUCCESS;
}

int phidget_initSensor() {
    if (PhidgetVoltageInput_create(&voltageInputHandle) != EPHIDGET_OK) {
        fprintf(stderr, "failed to create voltage input channel\n");
        return EXIT_FAILURE;
    }
    return openChannel((PhidgetHandle) voltageInputHandle);
}

int phidget_initMotor() {
    if (PhidgetRCServo_create(&servoHandle) != EPHIDGET_OK) {
        fprintf(stderr, "failed to create rc servo channel\n");
        return EXIT_FAILURE;
    }
    return openChannel((PhidgetHandle) servoHandle);
}

int phidget_init() {
    PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);

    int res;
    res = phidget_initSensor();
    if (res != EXIT_SUCCESS) {
        fprintf(stderr, "failed to init sensor\n");
        return res;
    }
    res = phidget_initMotor();
    if (res != EXIT_SUCCESS) {
        fprintf(stderr, "failed to init motor\n");
        return res;
    }

    return 0;
}

double * phidget_getVoltage(double *voltage) {
    PhidgetVoltageInput_getVoltage(voltageInputHandle, voltage);
    return voltage;
}

int phidget_setMotorValue(double motor_input) {
    return PhidgetRCServo_setTargetPosition(servoHandle, motor_input);
}

int phidget_setMotorState(int enabled) { return PhidgetRCServo_setEngaged(servoHandle, enabled); }