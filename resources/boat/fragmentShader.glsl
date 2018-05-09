#version 430 core

layout(location = 2) uniform vec4 defaultColor;
out vec4 color;
void main() {
    color = defaultColor;
}