#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform bool normalMapping;

void main() {
    vec3 normal;
    if (normalMapping) {
        normal = texture(normalMap, fs_in.TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
    }
    else
        normal = vec3(0.0f, 0.0f, 1.0f);

    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;
    vec3 ambient = 0.1 * color;

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * color;

    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);
    vec3 specular = vec3(0.2f) * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}
