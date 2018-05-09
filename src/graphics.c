#include <stdio.h>
#include <glad/glad.h>
#include "file_utils.h"
#include "shader_util.h"

GLuint uiShaderProgram;
GLuint graphShaderProgram;
GLuint boatShaderProgram;

void graphics_reload();

GLuint VBO, VAO, EBO;

int triangles;

GLuint boatVertexArray;
GLuint boatVertexBuffer;

GLuint graphVertexArray;
GLuint graphVertexBuffer;

GLint currentGraphVertexNumber;
#define GRAPH_LENGTH 1000
#define TOTAL_GRAPH_WIDTH 1.99f
#define MAIN_GRAPH_WIDTH TOTAL_GRAPH_WIDTH
#define OTHER_GRAPH_WIDTH (TOTAL_GRAPH_WIDTH/2)
#define MAIN_GRAPH_HEIGHT (2.0f/3)
#define OTHER_GRAPH_HEIGHT (1.0f/3)

void bindPolygons(GLfloat *verts, GLint vertCount, GLint *indic,
                  GLint indicCount) {// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 3, verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * indicCount * 3, indic, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO
}

void ui_init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void ui_load() {
    GLint vertCount = getVectorCountFromFile("vertices.txt");
    GLint indicCount = getVectorCountFromFile("indices.txt");
    GLfloat verts[vertCount * 3];
    GLint indic[indicCount * 3];
    getFloatVectorFromFile("vertices.txt", vertCount, verts);
    getIntVectorFromFile("indices.txt", indicCount, indic);
    bindPolygons(verts, vertCount, indic, indicCount);
    triangles = indicCount;
}

void boat_init() {
    glGenVertexArrays(1, &boatVertexArray);
    glBindVertexArray(boatVertexArray);
    glGenBuffers(1, &boatVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, boatVertexBuffer);
    GLfloat boatVertices[6] = {-.05f, -.89f, -.05f, -.99f, .05f, -.94f};
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6, boatVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

}

void graph_init() {
    glGenVertexArrays(1, &graphVertexArray);
    glBindVertexArray(graphVertexArray);
    glGenBuffers(1, &graphVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, graphVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * GRAPH_LENGTH * 2 * 4, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    uiShaderProgram = glCreateProgram();
    graphShaderProgram = glCreateProgram();
    boatShaderProgram = glCreateProgram();
}

void graphics_init(void *(*loadProc)(const char)) {
    // Initialize glad, use glfw to retrieve GL function pointers
    gladLoadGLLoader((GLADloadproc) loadProc);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ui_init();
    boat_init();
    graph_init();

    graphics_reload();


    // Uncommenting this call will result in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void graphics_draw(GLfloat boatPosition, GLfloat targetPosition) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(uiShaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, 0);

    glUseProgram(boatShaderProgram);
    glBindVertexArray(boatVertexArray);
    glUniform1f(1, boatPosition);
    glUniform4f(2, 0.9f, 0.1f, 0.1f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUniform1f(1, targetPosition);
    glUniform4f(2, 1.0f, 0.5f, 0.5f, 0.5f);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(graphShaderProgram);
    glBindVertexArray(graphVertexArray);

    glUniform1f(1, MAIN_GRAPH_WIDTH - MAIN_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
    glUniform4f(2, 1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINE_STRIP, 0, currentGraphVertexNumber);
    glUniform1f(1, -MAIN_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
    glUniform4f(2, 1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINE_STRIP, currentGraphVertexNumber, GRAPH_LENGTH - currentGraphVertexNumber);

    glUniform1f(1, OTHER_GRAPH_WIDTH - OTHER_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
    glUniform4f(2, 0.0f, 1.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINE_STRIP, GRAPH_LENGTH, currentGraphVertexNumber);
    glUniform1f(1, -OTHER_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
    glUniform4f(2, 0.0f, 1.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINE_STRIP, currentGraphVertexNumber + GRAPH_LENGTH, GRAPH_LENGTH - currentGraphVertexNumber);

    glUniform1f(1, OTHER_GRAPH_WIDTH - OTHER_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
    glUniform4f(2, 0.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_LINE_STRIP, GRAPH_LENGTH * 2, currentGraphVertexNumber);
    glUniform1f(1, -OTHER_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH);
    glUniform4f(2, 0.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_LINE_STRIP, currentGraphVertexNumber + GRAPH_LENGTH * 2, GRAPH_LENGTH - currentGraphVertexNumber);

    glBindVertexArray(0);
}

void graphics_updateGraph(GLfloat position, GLfloat velocity, GLfloat acceleration) {
    glBindBuffer(GL_ARRAY_BUFFER, graphVertexBuffer);

    GLfloat positionPoint[2] = {MAIN_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH - OTHER_GRAPH_WIDTH,
                                MAIN_GRAPH_HEIGHT * (position + .5f)};
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * currentGraphVertexNumber * 2, sizeof(GLfloat) * 2,
                    positionPoint);

    GLfloat velocityPoint[2] = {OTHER_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH - OTHER_GRAPH_WIDTH,
                                OTHER_GRAPH_HEIGHT * (velocity - 2)};
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (currentGraphVertexNumber + GRAPH_LENGTH) * 2,
                    sizeof(GLfloat) * 2, velocityPoint);

    GLfloat accelerationPoint[2] = {OTHER_GRAPH_WIDTH * currentGraphVertexNumber / GRAPH_LENGTH,
                                    OTHER_GRAPH_HEIGHT * (acceleration - 2)};
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (currentGraphVertexNumber + GRAPH_LENGTH * 2) * 2,
                    sizeof(GLfloat) * 2, accelerationPoint);

    currentGraphVertexNumber = (currentGraphVertexNumber + 1) % (GRAPH_LENGTH);
}

void graphics_reload() {
    shader_programInit(uiShaderProgram, ".");
    shader_programInit(graphShaderProgram, "graph");
    shader_programInit(boatShaderProgram, "boat");
    ui_load();
}
