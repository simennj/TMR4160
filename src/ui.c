#include <glad/glad.h>
#include "ui.h"
#include "file_utils.h"
#include "shader_util.h"

GLuint uiShaderProgram;
GLuint uiVertexBuffer;
GLuint uiVertexArray;
GLuint uiElementArray;
int uiTrianglesCount;

void bindPolygons(GLfloat *verts, GLint vertCount, GLint *indic,
                  GLint indicCount) {// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(uiVertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, uiVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 3, verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiElementArray);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * indicCount * 3, indic, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO
}

void ui_init() {
    glGenVertexArrays(1, &uiVertexArray);
    glGenBuffers(1, &uiVertexBuffer);
    glGenBuffers(1, &uiElementArray);
    uiShaderProgram = glCreateProgram();
}

void ui_draw() {
    glUseProgram(uiShaderProgram);
    glBindVertexArray(uiVertexArray);
    glDrawElements(GL_TRIANGLES, uiTrianglesCount * 3, GL_UNSIGNED_INT, 0);
}

void ui_reload() {
    GLint vertCount = getVectorCountFromFile("ui/vertices.txt");
    GLint indicCount = getVectorCountFromFile("ui/indices.txt");
    GLfloat verts[vertCount * 3];
    GLint indic[indicCount * 3];
    getFloatVectorFromFile("vertices.txt", vertCount, verts);
    getIntVectorFromFile("indices.txt", indicCount, indic);
    bindPolygons(verts, vertCount, indic, indicCount);
    uiTrianglesCount = indicCount;

    shader_programInit(uiShaderProgram, "ui");
}