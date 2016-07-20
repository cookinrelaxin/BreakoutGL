#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;

void main() {
    /*vec3 result = (ambient + diffuse + specular) * attenuation;*/
    /*if (gamma)*/
        /*result = pow(result, vec3(1.0/2.2));*/
    color = vec4(texture(texture_diffuse1, TexCoords));
    color = vec4(pow(color.rgb, vec3(1.0/2.2)), 1.0);
}
