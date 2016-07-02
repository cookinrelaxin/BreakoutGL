#version 330 core

in vec2 TexCoords;
out vec4 out_color;

uniform sampler2D renderedScene;
uniform vec2 resolution;

void main() {
    /*vec4 center = vec4(texture(renderedScene, TexCoords).rgb, 1.0f);*/
    /*vec4 color = vec4(0.0f);*/
    /*float total = 0.0f;*/
    /*const float exponent = 15.0f;*/
    /*for (float x = -4.0f; x <= 4.0f; x += 1.0f) {*/
        /*for (float y = -4.0f; y <= 4.0f; y += 1.0f) {*/
            /*vec4 sample = vec4(texture(renderedScene, TexCoords + vec2(x, y) / resolution).rgb, 1.0f);*/
            /*float weight = 1.0f - abs(dot(sample.rgb - center.rgb, vec3(0.25f)));*/
            /*weight = pow(weight, exponent);*/
            /*color += sample * weight;*/
            /*total += weight;*/
        /*}*/
    /*}*/
    /*out_color = color / total;*/
    out_color = vec4(texture(renderedScene, TexCoords).rgb, 1.0f);
}
