#ifndef TMR4160_FILE_UTILS_H
#define TMR4160_FILE_UTILS_H

int getVectorCountFromFile(const char *filename);

void getFloatVectorFromFile(const char *filename, int count, float *array);

void getIntVectorFromFile(const char *filename, int count, int *array);

const GLchar *getShaderSource(const char *filename);

#endif //TMR4160_FILE_UTILS_H
