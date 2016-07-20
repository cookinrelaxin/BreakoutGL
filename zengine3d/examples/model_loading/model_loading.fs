#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D diffuse;

void main() {
    color = vec4(texture(diffuse, TexCoords));
    color = vec4(pow(color.rgb, vec3(1.0/2.2)), 1.0);
}
