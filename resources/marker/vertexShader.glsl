#version 430 core

layout (location = 1) uniform float displacement = 0;
layout (location = 0) in vec3 position;
void main() {
    gl_Position = vec4(position.x + .95f, position.y + displacement/2 + .5f, position.z, 1.0);
}