#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool shadows;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f);

    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
        }
    }
    shadow /= 9.0f;

    if (projCoords.z > 1.0f)
        shadow = 0.0f;

    return shadow;
}

void main() {
   vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb; 
   vec3 normal = normalize(fs_in.Normal);
   vec3 lightColor = vec3(50.0f);

   vec3 ambient = 0.2f * color;

   vec3 lightDir = normalize(lightPos - fs_in.FragPos);
   float diff = max(dot(lightDir, normal), 0.0f);
   vec3 diffuse = diff * lightColor;

   vec3 viewDir = normalize(viewPos - fs_in.FragPos);
   float spec = 0.0f;
   vec3 halfwayDir = normalize(lightDir + viewDir);
   spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
   vec3 specular = spec * lightColor;
   float shadow = shadows ? ShadowCalculation(fs_in.FragPosLightSpace) : 0.0f;
   shadow = min(shadow, 0.75);

   float distance = length(lightPos - fs_in.FragPos);
   float attenuation = 1.0f / (distance * distance);

   // vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * color * attenuation;
   vec3 result = ((1.0 - shadow) * (ambient + diffuse + specular)) * color * attenuation;
   result = pow(result, vec3(1.0/2.2));

   FragColor = vec4(result, 1.0f);
}
