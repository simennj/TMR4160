#include <glad/glad.h>
#include "graph.h"
#include "shader_util.h"
#include "clamp.h"

// Define constants used for position and dimensions of graphs
#define GRAPH_LENGTH 1000
#define TOTAL_GRAPH_WIDTH 1.99f
#define MAIN_GRAPH_WIDTH 1.9f
#define OTHER_GRAPH_WIDTH (TOTAL_GRAPH_WIDTH/2)
#define MAIN_GRAPH_HEIGHT (0.5f)
#define OTHER_GRAPH_HEIGHT (0.25f)


GLuint graphShaderProgram;
GLuint graphVertexArray;
GLuint graphVertexBuffer;
GLint currentGraphVertexNumber;

void graph_init() {
    glGenVertexArrays(1, &graphVertexArray);
    glBindVertexArray(graphVertexArray);
    glGenBuffers(1, &graphVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, graphVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * GRAPH_LENGTH * 2 * 5, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    graphShaderProgram = glCreateProgram();
}

void update(GLfloat mainValue, const GLfloat *otherValues) {
    glBindBuffer(GL_ARRAY_BUFFER, graphVertexBuffer);

    GLfloat point[2] = {MAIN_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH - OTHER_GRAPH_WIDTH,
                        MAIN_GRAPH_HEIGHT * (mainValue + 1.0f)};
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * currentGraphVertexNumber * 2, sizeof(GLfloat) * 2,
                    point);

    for (int i = 0; i < 4; ++i) {
        div_t cell = div(i, 2);
        point[0] = OTHER_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH - OTHER_GRAPH_WIDTH * cell.rem;
        point[1] = OTHER_GRAPH_HEIGHT * ((float) clamp(otherValues[i], -1, 1) - 1 - 2 * cell.quot);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (currentGraphVertexNumber + GRAPH_LENGTH * (i + 1)) * 2,
                        sizeof(GLfloat) * 2, point);
    }

    currentGraphVertexNumber = (currentGraphVertexNumber + 1) % (GRAPH_LENGTH);
}

void graph_draw(GLfloat boatPosition, const GLfloat *otherValues) {
    update(boatPosition, otherValues);

    glUseProgram(graphShaderProgram);
    glBindVertexArray(graphVertexArray);

    glUniform1f(1, MAIN_GRAPH_WIDTH - MAIN_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
    glUniform1i(2, 0);
    glDrawArrays(GL_LINE_STRIP, 0, currentGraphVertexNumber);
    glUniform1f(1, -MAIN_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
    glDrawArrays(GL_LINE_STRIP, currentGraphVertexNumber, GRAPH_LENGTH - currentGraphVertexNumber);

    for (int i = 0; i < 4; ++i) {
        glUniform1f(1, OTHER_GRAPH_WIDTH - OTHER_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
        glUniform1i(2, i + 1);
        glDrawArrays(GL_LINE_STRIP, GRAPH_LENGTH * (i + 1), currentGraphVertexNumber);
        glUniform1f(1, -OTHER_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
        glDrawArrays(GL_LINE_STRIP, currentGraphVertexNumber + GRAPH_LENGTH * (i + 1),
                     GRAPH_LENGTH - currentGraphVertexNumber);
    }
}

void graph_reload() { shader_programInit(graphShaderProgram, "graph"); }