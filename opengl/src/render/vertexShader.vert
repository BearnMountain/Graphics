#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 inColor; // name must match fragment shader input

void main() {
    gl_Position = vec4(aPos, 1.0); // Not 2.0 — that will break NDC
    inColor = aColor;
}

