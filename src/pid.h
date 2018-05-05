#ifndef TMR4160_PID_H
#define TMR4160_PID_H
void pid_init(double k_p, double k_i, double k_d, double targetPosition);
void pid_update();

struct boatState {
    float position;
    float velocity;
    float acceleration;
};

struct boatState pid_getBoatState();
#endif //TMR4160_PID_H
