#include <glad/glad.h>
#include "ui.h"
#include "graph.h"
#include "marker.h"

void graphics_reload();

void graphics_init(void *(*loadProc)(const char)) {
    // Initialize glad, use glfw to retrieve GL function pointers
    gladLoadGLLoader((GLADloadproc) loadProc);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ui_init();
    marker_init();
    graph_init();

    graphics_reload();
}

void graphics_draw(GLfloat boatPosition, GLfloat targetPosition, float minorGraphValues[4]) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ui_draw();
    marker_draw(boatPosition, targetPosition);
    graph_draw(boatPosition, minorGraphValues);

    glBindVertexArray(0);
}

void graphics_reload() {
    ui_reload();
    graph_reload();
    marker_reload();
}

