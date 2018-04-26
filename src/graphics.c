#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>


int getVectorCountFromFile(const char *filename);

void getFloatVectorFromFile(const char *filename, int count, float *array);

void getIntVectorFromFile(const char *filename, int count, int *array);

GLuint shaderProgram;

void error_callback(int code, const char* description);

void resize_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

const GLchar *getShaderSource(const char *filename);

GLuint loadShaders();

void reload();

const GLuint WIDTH = 800, HEIGHT = 600;

GLuint VBO, VAO, EBO;

int triangles;

GLFWwindow *window;

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

const int VECTOR_BUFFER_SIZE = 20;

GLint getVectorCountFromFile(const char *filename) {
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

void getFloatVectorFromFile(const char *filename, GLint count, GLfloat *array) {
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

void getIntVectorFromFile(const char *filename, GLint count, GLint *array) {
    FILE *file = fopen(filename, "r");
    char *rest;
    char line[VECTOR_BUFFER_SIZE];

    for (int i = 0; i < count * 3; i += 3) {
        fgets(line, VECTOR_BUFFER_SIZE, file);
        rest = strtok(line, "#"); // everything after the first # is a comment
        for (int j = 0; j < 3; ++j) { // the third coordinate becomes 0 if not defined
            array[i + j] = (GLint) strtol(rest, &rest, 10);
        }
    }
    fclose(file);
}

const GLchar *getShaderSource(const char *filename) {
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

