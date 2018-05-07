#include "boat.h"

#include <phidget22.h>
#include <stdio.h>
#include <stdlib.h>


PhidgetRCServoHandle servoHandle;
PhidgetVoltageInputHandle voltageInputHandle;

int initPhidget();

int initPhidgetSensor();

int initPhidgetMotor();

double min_voltage, max_voltage, middle_voltage;
double max_motor = 170, min_motor = 20;

static void CCONV
onVoltageChangeHandler(PhidgetVoltageInputHandle ch, void *ctx, double voltage) {
    printf("Voltage Changed: %.4f\n", voltage);
    double position = (voltage - middle_voltage) / (max_voltage - min_voltage);
    printf("calculated position %f\n", position);
    double motor_input = position * 150 + 90;
    printf("Setting motor input to %f\n", motor_input);
//    fflush(stdout);
    if (PhidgetRCServo_setTargetPosition(servoHandle, motor_input) != EPHIDGET_OK) {
        printf("Motor input set to %f\n", motor_input);
    }
}

int initPhidget() {
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

    min_voltage = 3.5;
    max_voltage = 5;
    middle_voltage = (min_voltage + max_voltage) / 2;
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
