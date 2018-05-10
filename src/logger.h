#ifndef TMR4160_LOGGER_H
#define TMR4160_LOGGER_H

void logger_init(time_t startTimeSeconds);

void logger_update(struct pid_state *structboatState, double targetPosition, double dt);

void logger_end();

#endif //TMR4160_LOGGER_H
