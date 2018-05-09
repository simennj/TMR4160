#version 430 core

uniform vec4 defaultColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
out vec4 color;
void main() {
    color = defaultColor;
}