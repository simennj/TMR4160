#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "file_utils.h"

GLuint shaderProgram;

void error_callback(int code, const char* description);

void resize_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

GLuint loadShaders();

void reload();

const GLuint WIDTH = 800, HEIGHT = 600;

GLuint VBO, VAO, EBO;

int triangles;

GLFWwindow *window;

GLuint boatVertexArray;
GLuint boatVertexBuffer;

GLuint graphVertexArray;
GLuint graphVertexBuffer;

GLint graphIndex;
#define graphLength 1000

void graphics_init() {
    glfwSetErrorCallback(error_callback);

    glfwInit();
    // Set required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a window object
    window = glfwCreateWindow(WIDTH, HEIGHT, "Dynamic Positioning", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, resize_callback);
    glfwSetKeyCallback(window, key_callback);

    // Initialize glad, use glfw to retrieve GL function pointers
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    shaderProgram = loadShaders();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &boatVertexArray);
    glGenVertexArrays(1, &graphVertexArray);

    glBindVertexArray(boatVertexArray);

    glGenBuffers(1, &boatVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, boatVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 9, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(0);
    glBindVertexArray(graphVertexArray);

    glGenBuffers(1, &graphVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, graphVertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * graphLength * 2, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    reload();


    // Uncommenting this call will result in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

int graphics_open() {
    return !glfwWindowShouldClose(window);
}


void graphics_update() {
    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(boatVertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(graphVertexArray);
    glDrawArrays(GL_LINE_STRIP, 0, graphIndex / 2);
    glDrawArrays(GL_LINE_STRIP, graphIndex / 2, graphLength - graphIndex / 2);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
}

void graphics_stop() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
}

GLuint loadShaders() {// Load vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar *vertexShaderSource = getShaderSource("vertexShader.glsl");
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex shader error:%s\n", infoLog);
    }


    // Load fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar *fragment = getShaderSource("fragmentShader.glsl");
    glShaderSource(fragmentShader, 1, &fragment, NULL);
    glCompileShader(fragmentShader);
    // Check for compile time errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader error:%s\n", infoLog);
    }

    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Shader linking error:%s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

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

void graphics_setBoatPosition(GLfloat position) {
    glBindBuffer(GL_ARRAY_BUFFER, boatVertexBuffer);
    GLfloat boatVertices[9] = {position - .05f, -.89f, 0, position - .05f, -.99f, 0, position + .05f, -.94f, 0};
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 9, boatVertices);

    glBindBuffer(GL_ARRAY_BUFFER, graphVertexBuffer);
    GLfloat point[2] = {0.99f * graphIndex / graphLength - .99f, position};
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * graphIndex, sizeof(GLfloat) * 2, point);
    graphIndex = (graphIndex + 2) % (graphLength * 2);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        reload();
    }
}

void reload() {
    shaderProgram = loadShaders();
    GLint vertCount = getVectorCountFromFile("vertices.txt");
    GLint indicCount = getVectorCountFromFile("indices.txt");
    GLfloat verts[vertCount * 3];
    GLint indic[indicCount * 3];
    getFloatVectorFromFile("vertices.txt", vertCount, verts);
    getIntVectorFromFile("indices.txt", indicCount, indic);
    bindPolygons(verts, vertCount, indic, indicCount);
    triangles = indicCount;
}

void error_callback(int code, const char *description) {
    printf("%i: ", code);
    printf(description);
    printf("\n");
}

void resize_callback(GLFWwindow *window, int width, int height) {

    if (height >= width)
        glViewport(0, (height - width) / 2, width, width);
    else
        glViewport((width - height) / 2, 0, height, height);


}

