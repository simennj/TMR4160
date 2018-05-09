#ifndef TMR4160_GRAPHICS_H
#define TMR4160_GRAPHICS_H

void graphics_init(void *(*loadProc)(const char));

void graphics_reload();

void graphics_draw(GLfloat boatPosition, GLfloat targetPosition);

void graphics_updateGraph(GLfloat position, GLfloat velocity, GLfloat acceleration);

#endif //TMR4160_GRAPHICS_H
