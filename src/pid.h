#ifndef TMR4160_PID_H
#define TMR4160_PID_H

void pid_init(double k_p, double k_i, double k_d, double targetPosition, double motorCenter, double motorRadius);

void pid_update(double dt);

struct pid_state {
    float boatPosition;
    float estimatedBoatVelocity;
    float estimatedBoatAcceleration;
    float targetPosition;
};

struct pid_state pid_getState();

void init_phidget();
#endif //TMR4160_PID_H
