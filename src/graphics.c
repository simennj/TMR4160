#include <stdio.h>
#include <glad/glad.h>
#include "file_utils.h"
#include "shader_util.h"

GLuint uiShaderProgram;
GLuint graphShaderProgram;

void graphics_reload();

GLuint VBO, VAO, EBO;

int triangles;

GLuint boatVertexArray;
GLuint boatVertexBuffer;

GLuint graphVertexArray;
GLuint graphVertexBuffer;

GLint currentGraphVertexNumber;
#define graphLength 1000


void bindPolygons(GLfloat *verts, GLint vertCount, GLint *indic,
                  GLint indicCount) {// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 3, verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * indicCount * 3, indic, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,
                 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray(
            0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 9, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

}

void graph_init() {
    glGenVertexArrays(1, &graphVertexArray);
    glBindVertexArray(graphVertexArray);
    glGenBuffers(1, &graphVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, graphVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * graphLength * 2, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    uiShaderProgram = glCreateProgram();
    graphShaderProgram = glCreateProgram();
}

void graphics_init(void *(*loadProc)(const char)) {
    // Initialize glad, use glfw to retrieve GL function pointers
    gladLoadGLLoader((GLADloadproc) loadProc);

    ui_init();
    boat_init();
    graph_init();

    graphics_reload();


    // Uncommenting this call will result in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void graphics_update() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(uiShaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(boatVertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(graphShaderProgram);
    glBindVertexArray(graphVertexArray);
    glDrawArrays(GL_LINE_STRIP, 0, currentGraphVertexNumber);
    glDrawArrays(GL_LINE_STRIP, currentGraphVertexNumber, graphLength - currentGraphVertexNumber);
    glBindVertexArray(0);
}

void graphics_setBoatPosition(GLfloat position) {
    glBindBuffer(GL_ARRAY_BUFFER, boatVertexBuffer);
    GLfloat boatVertices[9] = {position - .05f, -.89f, 0, position - .05f, -.99f, 0, position + .05f, -.94f, 0};
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 9, boatVertices);

    glBindBuffer(GL_ARRAY_BUFFER, graphVertexBuffer);
    GLfloat point[2] = {1.99f * currentGraphVertexNumber / graphLength - .99f, position};
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * currentGraphVertexNumber * 2, sizeof(GLfloat) * 2, point);
    currentGraphVertexNumber = (currentGraphVertexNumber + 1) % (graphLength);
}

void graphics_reload() {
    shader_programInit(uiShaderProgram, ".");
    shader_programInit(graphShaderProgram, "graph");
    ui_load();
}
