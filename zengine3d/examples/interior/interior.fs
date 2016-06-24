#version 330 core

struct Material {
    vec3 baseColor;
    float blendCoefficient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 color;
};

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    float distance = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 result = (ambient + diffuse + specular) * attenuation;
    color = vec4(result, 1.0f);
}
