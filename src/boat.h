#ifndef TMR4160_BOAT_H
#define TMR4160_BOAT_H

int boat_initPhidget();

double boat_getPosition();

void boat_update(double dt, double motor_force);

#endif //TMR4160_BOAT_H
