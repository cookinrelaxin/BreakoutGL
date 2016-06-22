#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;

vec3 gridSamplingDisk[20] = vec3[](
        vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1, 1),  vec3(-1, 1,  1), 
        vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
        vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
        vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
        vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0f;
    float bias = 0.15f;
    const int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0f + (viewDistance / far_plane)) / 100.0f;
    for (int i = 0; i < samples; ++i) {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;
        if (currentDepth - bias > closestDepth)
            shadow += 1.0f;
    }
    shadow /= float(samples);

    /*FragColor = vec4(vec3(closestDepth / far_plane), 1.0f);*/

    return shadow;
}

void main() {
   vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb; 
   vec3 normal = normalize(fs_in.Normal);
   vec3 lightColor = vec3(10.0f);

   vec3 ambient = 0.2f * color;

   vec3 lightDir = normalize(lightPos - fs_in.FragPos);
   float diff = max(dot(lightDir, normal), 0.0f);
   vec3 diffuse = diff * lightColor;

   vec3 viewDir = normalize(viewPos - fs_in.FragPos);
   float spec = 0.0f;
   vec3 halfwayDir = normalize(lightDir + viewDir);
   spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
   vec3 specular = spec * lightColor;

   float shadow = shadows ? ShadowCalculation(fs_in.FragPos) : 0.0f;

   float distance = length(lightPos - fs_in.FragPos);
   float attenuation = 1.0f / (distance * distance);

   vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * color * attenuation;
   result = pow(result, vec3(1.0/2.2));

   FragColor = vec4(result, 1.0f);
}
