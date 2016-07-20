#version 330 core

in vec3 Normal;
in vec3 Position;
out vec4 color;

uniform vec3 cameraPos;
uniform samplerCube skybox;

//refractive indices
//float air = 1.00;
//float water = 1.33;
//float ice = 1.309;
//float glass = 1.52;
//float diamond = 2.42;
//eg. air to glass would be air/glass

void main() {
    vec3 I = normalize(Position - cameraPos);
    // vec3 R = reflect(I, normalize(Normal));
    vec3 R = refract(I, normalize(Normal), 1.00/1.33);
    color = texture(skybox, R);
}
