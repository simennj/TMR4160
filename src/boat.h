#ifndef TMR4160_BOAT_H
#define TMR4160_BOAT_H

double boat_getPosition();

void boat_update(double motor_force);

void boat_setMotorValues(double center, double radius);

void boat_toggleMotor();

int boat_init();

#endif //TMR4160_BOAT_H
