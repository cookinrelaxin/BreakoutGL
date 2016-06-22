#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 shadowMatrix;
out vec4 FragPos;

void main() {
    gl_Position = shadowMatrix * model * vec4(position, 1.0f);
    FragPos = model * vec4(position, 1.0f);
}
