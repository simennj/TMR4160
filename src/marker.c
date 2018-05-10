#include <glad/glad.h>
#include "marker.h"
#include "shader_util.h"

GLuint markerShaderProgram;
GLuint markerVertexArray;
GLuint markerVertexBuffer;

void marker_init() {
    glGenVertexArrays(1, &markerVertexArray);
    glBindVertexArray(markerVertexArray);
    glGenBuffers(1, &markerVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, markerVertexBuffer);
    GLfloat markerVertices[4] = {-.05f, .0f, .05f, .0f};
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4, markerVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    markerShaderProgram = glCreateProgram();
}

void marker_draw(GLfloat boatPosition, GLfloat targetPosition) {
    glUseProgram(markerShaderProgram);
    glBindVertexArray(markerVertexArray);
    glUniform1f(1, boatPosition);
    glUniform4f(2, 0.9f, 0.1f, 0.1f, 1.0f);
    glDrawArrays(GL_LINES, 0, 2);
    glUniform1f(1, targetPosition);
    glUniform4f(2, 1.0f, 0.5f, 0.5f, 0.5f);
    glDrawArrays(GL_LINES, 0, 2);
}

void marker_reload() { shader_programInit(markerShaderProgram, "marker"); }