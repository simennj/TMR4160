#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


typedef struct {
    int count;
    GLfloat *array;
} FloatVectors;

typedef struct {
    int count;
    GLint *array;
} IntVectors;

FloatVectors *createFloatVectorFromFile(const char *filename);

IntVectors *createIntVectorFromFile(const char *filename);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

const GLchar *getShaderSource(const char *filename);

void bindPolygons(const FloatVectors *verts, const IntVectors *indic);

const GLuint WIDTH = 800, HEIGHT = 600;

GLuint VBO, VAO, EBO;

int test() {
    glfwInit();
    // Set required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a window object
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Dynamic Positioning", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    // Initialize glad, use glfw to retrieve GL function pointers
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);


    // Load vertex shader
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


    FloatVectors *verts = createFloatVectorFromFile("vertices.txt");

    IntVectors *indic = createIntVectorFromFile("indices.txt");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    bindPolygons(verts, indic);


    // Uncommenting this call will result in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}

void bindPolygons(const FloatVectors *verts, const IntVectors *indic) {// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verts->count * 3, verts->array, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * indic->count * 3, indic->array, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,
                 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray(
            0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO


}

const int VECTOR_BUFFER_SIZE = 20;

FloatVectors *createFloatVectorFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    char *rest;
    char line[VECTOR_BUFFER_SIZE];
    fgets(line, VECTOR_BUFFER_SIZE, file);

    FloatVectors *verts = malloc(sizeof(FloatVectors));
    verts->count = strtol(line, &rest, 10);
    verts->array = malloc(3 * verts->count * sizeof(GLfloat));

    for (int i = 0; i < verts->count * 3; i += 3) {
        fgets(line, VECTOR_BUFFER_SIZE, file);
        rest = strtok(line, "#"); // everything after the first # is a comment
        for (int j = 0; j < 3; ++j) { // the third coordinate becomes 0 if not defined
            verts->array[i + j] = strtof(rest, &rest);
        }
    }
    return verts;
}

IntVectors *createIntVectorFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    char *rest;
    char line[VECTOR_BUFFER_SIZE];
    fgets(line, VECTOR_BUFFER_SIZE, file);

    IntVectors *verts = malloc(sizeof(FloatVectors));
    verts->count = strtol(line, &rest, 10);
    verts->array = malloc(3 * verts->count * sizeof(GLint));

    for (int i = 0; i < verts->count * 3; i += 3) {
        fgets(line, VECTOR_BUFFER_SIZE, file);
        rest = strtok(line, "#"); // everything after the first # is a comment
        for (int j = 0; j < 3; ++j) { // the third coordinate becomes 0 if not defined
            verts->array[i + j] = strtol(rest, &rest, 10);
        }
    }
    return verts;
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
        FloatVectors *verts = createFloatVectorFromFile("vertices.txt");
        IntVectors *indic = createIntVectorFromFile("indices.txt");
        bindPolygons(verts, indic);
    }
}
