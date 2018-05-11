#include <glad/glad.h>
#include "graph.h"
#include "shader_util.h"
#include "../utils/clamp.h"

// Define amount of coordinates per vertex
#define VERTEX_COORDINATE_COUNT 2

// Amount of datapoints a graph will hold
#define GRAPH_LENGTH 1000

// Define constants used for amount, position, and dimensions of graphs
#define TOTAL_GRAPH_WIDTH 1.99f
#define MAIN_GRAPH_WIDTH 1.9f
#define GRAPH_COLUMNS 2
#define OTHER_GRAPH_WIDTH (TOTAL_GRAPH_WIDTH/GRAPH_COLUMNS)
#define MAIN_GRAPH_HEIGHT (0.5f)
#define OTHER_GRAPH_HEIGHT (0.25f)
#define GRAPHS_COUNT 5

// OpenGL identifiers
GLuint graphShaderProgram;
GLuint graphVertexArray;
GLuint graphVertexBuffer;
GLint currentGraphVertexNumber;

/*
 * Various generation, binding, etc for Opengl
 */
void graph_init() {
    graphShaderProgram = glCreateProgram();
    glGenVertexArrays(1, &graphVertexArray);
    glBindVertexArray(graphVertexArray);
    glGenBuffers(1, &graphVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, graphVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * GRAPH_LENGTH * VERTEX_COORDINATE_COUNT * GRAPHS_COUNT, NULL,
                 GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, VERTEX_COORDINATE_COUNT, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

/*
 * Updates the graph with the new values
 */
void graph_update(GLfloat mainValue, const GLfloat *otherValues) {
    glBindBuffer(GL_ARRAY_BUFFER, graphVertexBuffer);

    // Calculate x and y position for mainValue and creates a 2D point
    GLfloat point[VERTEX_COORDINATE_COUNT] = {
            MAIN_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH - OTHER_GRAPH_WIDTH,
            MAIN_GRAPH_HEIGHT * (mainValue + 1.0f)
    };
    // Put the point in current index of buffer
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * currentGraphVertexNumber * VERTEX_COORDINATE_COUNT,
                    sizeof(GLfloat) * VERTEX_COORDINATE_COUNT, point);

    // Do the same for all other graphs, only with a buffer offset
    for (int i = 0; i < GRAPHS_COUNT - 1; ++i) {
        div_t cell = div(i, GRAPH_COLUMNS);
        point[0] = OTHER_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH - OTHER_GRAPH_WIDTH * cell.rem;
        point[1] = OTHER_GRAPH_HEIGHT * ((float) clamp(otherValues[i], -1, 1) - 1 - 2 * cell.quot);
        glBufferSubData(
                GL_ARRAY_BUFFER,
                sizeof(GLfloat) * (currentGraphVertexNumber + GRAPH_LENGTH * (i + 1)) * VERTEX_COORDINATE_COUNT,
                sizeof(GLfloat) * VERTEX_COORDINATE_COUNT,
                point
        );
    }

    // Increment buffer position, start writing to the start of the buffer again when the end is reached
    // Shaders are used later to stitch the parts before and after the current position to a seamless graph
    currentGraphVertexNumber = (currentGraphVertexNumber + 1) % (GRAPH_LENGTH);
}

/*
 * Updates and draws the graph with the new values
 */
void graph_draw(GLfloat boatPosition, const GLfloat *otherValues) {
    graph_update(boatPosition, otherValues);

    glUseProgram(graphShaderProgram);
    glBindVertexArray(graphVertexArray);

    // Draw the graph with an offset so that the vertex at the current buffer position is at the far right
    // And the vertices before it in the buffer to the left of it
    glUniform1f(1, MAIN_GRAPH_WIDTH - MAIN_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
    // Sets the color to be the first of the colors defined in the shader
    glUniform1i(2, 0);
    glDrawArrays(GL_LINE_STRIP, 0, currentGraphVertexNumber);
    // Draw the graph with an offset so that the vertex right after the current buffer position is at the far left
    // and the vertices after it in the buffer to the right of it
    glUniform1f(1, -MAIN_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
    glDrawArrays(GL_LINE_STRIP, currentGraphVertexNumber, GRAPH_LENGTH - currentGraphVertexNumber);

    // Do the same for the rest of the graphs, but with different positions and colors
    for (int i = 0; i < GRAPHS_COUNT - 1; ++i) {
        glUniform1f(1, OTHER_GRAPH_WIDTH - OTHER_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
        glUniform1i(2, i + 1);
        glDrawArrays(GL_LINE_STRIP, GRAPH_LENGTH * (i + 1), currentGraphVertexNumber);
        glUniform1f(1, -OTHER_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
        glDrawArrays(GL_LINE_STRIP, currentGraphVertexNumber + GRAPH_LENGTH * (i + 1),
                     GRAPH_LENGTH - currentGraphVertexNumber);
    }
}

/*
 * Reload graph shader program from file
 */
void graph_reload() { shader_programInit(graphShaderProgram, "graph"); }