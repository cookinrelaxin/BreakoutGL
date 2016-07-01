#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D renderedScene;

void main() {
    color = vec4(texture(renderedScene, TexCoords).rgb, 1.0f);
}
