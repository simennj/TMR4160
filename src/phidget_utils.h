#ifndef TMR4160_PHIDGET_UTILS_H
#define TMR4160_PHIDGET_UTILS_H

int phidget_init();

int phidget_initSensor();

int phidget_initMotor();

double phidget_getVoltage(double *voltage);

PhidgetReturnCode phidget_setMotorValue(double motor_input);

PhidgetReturnCode phidget_setMotorState(int enabled);

#endif //TMR4160_PHIDGET_UTILS_H
