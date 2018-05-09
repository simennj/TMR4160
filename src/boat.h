#ifndef TMR4160_BOAT_H
#define TMR4160_BOAT_H

int boat_initPhidget();

double boat_getPosition();

void boat_update(double motor_force);

void boat_setMotorValues(double center, double radius);

int boat_toggleMotor();

#endif //TMR4160_BOAT_H
