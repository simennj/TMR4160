#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_utils.h"

const int VECTOR_BUFFER_SIZE = 100;


int getVectorCountFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    int count = 0;
    int bufferChar;
    char lastCharNewline = 1;
    for (bufferChar = getc(file); bufferChar != EOF; bufferChar = getc(file)) {
        if (bufferChar == '\n') {
            count += !lastCharNewline;
            lastCharNewline = 1;
        } else {
            lastCharNewline = 0;
        }
    }
    return count;
}

void getFloatVectorFromFile(const char *filename, int count, float *array) {
    FILE *file = fopen(filename, "r");
    char *rest;
    char line[VECTOR_BUFFER_SIZE];

    for (int i = 0; i < count * 3; i += 3) {
        fgets(line, VECTOR_BUFFER_SIZE, file);
        rest = strtok(line, "#"); // everything after the first # is a comment
        for (int j = 0; j < 3; ++j) { // the third coordinate becomes 0 if not defined
            array[i + j] = strtof(rest, &rest);
        }
    }
    fclose(file);
}

void getIntVectorFromFile(const char *filename, int count, int *array) {
    FILE *file = fopen(filename, "r");
    char *rest;
    char line[VECTOR_BUFFER_SIZE];

    for (int i = 0; i < count * 3; i += 3) {
        fgets(line, VECTOR_BUFFER_SIZE, file);
        rest = strtok(line, "#"); // everything after the first # is a comment
        for (int j = 0; j < 3; ++j) { // the third coordinate becomes 0 if not defined
            array[i + j] = (int) strtol(rest, &rest, 10);
        }
    }
    fclose(file);
}

const char *getShaderSource(const char *filename) {
    FILE *f = fopen(filename, "rb");
    char *fragmentShaderSource = 0;
    size_t length;
    if (f) {
        fseek(f, 0, SEEK_END);
        length = (size_t) ftell(f);
        fseek(f, 0, SEEK_SET);
        fragmentShaderSource = malloc(length + 1);
        if (fragmentShaderSource) {
            fread(fragmentShaderSource, 1, length, f);
            fragmentShaderSource[length] = '\0';
        }
        fclose(f);
    }
    return fragmentShaderSource;
}

int loadConstants(const char *filename, double *k_p, double *k_i, double *k_d, double *targetPosition) {
    FILE *file = fopen(filename, "r");
    int res = fscanf(file, "%lf %lf %lf %lf", k_p, k_i, k_d, targetPosition);
    fclose(file);
    return res;
}