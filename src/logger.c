#include "pid.h"
#include <stdio.h>
#include "logger.h"

FILE *logFile;

void logger_init(time_t startTimeSeconds) {
    char filename[50];
    sprintf(filename, "%lli.dat", startTimeSeconds);
    logFile = fopen(filename, "w");
    fprintf(logFile, "#%14s %15s %15s %15s %15s %15s %15s\n", "Timestamp", "Position", "Target Position", "P Force",
            "I Force", "D Force", "Total Force");
}

void logger_update(struct pid_state *structboatState, double targetPosition, double dt) {
    static double timeSinceStart = 0, nextWrite = 0;

    timeSinceStart += dt;
    if (timeSinceStart > nextWrite) {
        fprintf(logFile, "%15f %15f %15f %15f %15f %15f %15f\n", timeSinceStart,
                (*structboatState).boatPosition, targetPosition,
                (*structboatState).pid_pForce, (*structboatState).pid_iForce, (*structboatState).pid_dForce,
                (*structboatState).pidResultForce);
        nextWrite += .5;
    }
}

void logger_end() { fclose(logFile); }
