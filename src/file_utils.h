#ifndef TMR4160_FILE_UTILS_H
#define TMR4160_FILE_UTILS_H

#include <glad/glad.h>

int getVectorCountFromFile(const char *filename);

void getFloatVectorFromFile(const char *filename, int count, float *array);

void getIntVectorFromFile(const char *filename, int count, int *array);

const GLchar *getShaderSource(const char *filename);

int loadConstants(const char *filename, double *k_p, double *k_i, double *k_d);

#endif //TMR4160_FILE_UTILS_H
