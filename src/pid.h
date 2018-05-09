#ifndef TMR4160_PID_H
#define TMR4160_PID_H

void pid_init(double k_p, double k_i, double k_d, double motorCenter, double motorRadius);

struct pid_state pid_update(double dt, double targetPosition);

struct pid_state {
    float boatPosition;
    float estimatedBoatVelocity;
    float pidResultForce;
};

void init_phidget();
#endif //TMR4160_PID_H
