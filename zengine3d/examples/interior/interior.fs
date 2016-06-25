#version 330 core

struct Material {
    vec3 diffuseColor;
    vec3 specularColor;

    float blendCoefficient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 color;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform float far_plane;

uniform samplerCube depthMap;

float random(vec3 seed, float freq) {
    float dt = dot(floor(seed * freq), vec3(53.1215f, 21.1352f, 9.1322f));
    return fract(sin(dt) * 2105.2354f);
}
/*const int numSamplingPositions = 9;*/
/*uniform vec2 kernel[9] = vec2[] (*/
    /*vec2(0.95581, -0.18159), vec2(0.50147, -0.35807), vec2(0.69607, 0.35559),*/
    /*vec2(-0.0036825, -0.59150),	vec2(0.15930, 0.089750), vec2(-0.65031, 0.058189),*/
    /*vec2(0.11915, 0.78449), vec2(-0.34296, 0.51575), vec2(-0.60380, -0.41527));*/

/*float randomAngle(vec3 seed, float freq) {*/
    /*return random(seed, freq) * 6.283285;*/
/*};*/

/*float rotatedPoissonShadow(vec3 fragPos) {*/
    /*vec3 fragToLight = fragPos - light.position;*/
    /*float shadow = 0.0f;*/
    /*float angle = randomAngle(fragPos, 15.0f);*/
    /*float s = sin(angle);*/
    /*float c = cos(angle);*/
    /*float PCFRadius = 20.0f;*/
    /*for (int i = 4; i < numSamplingPositions; < i++) {*/
        /*vec2 rotatedOffset = vec2(kernel[i].x * c + kernel[i].y * s,*/
                                  /*kernel[i].x * -s + kernel[i] * c);*/
        /*factor += texture(depthMap, vec3(frag.xy + offset, coords.z));*/
        /*numSamplesUsed += 1;*/
    /*}*/
/*}*/

int randInt(vec3 seed, float freq, int maxInt) {
    return int(float(maxInt) * random(seed, freq)) % maxInt;
}

vec3 gridSamplingDisk[20] = vec3[](
        vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1, 1),  vec3(-1, 1,  1), 
        vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
        vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
        vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
        vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos) {
    vec3 fragToLight = fragPos - light.position;
    float currentDepth = length(fragToLight);

    float shadow = 0.0f;
    /*float bias = 0.15f;*/
    float minBias = 0.0f;
    float maxBias = .30f;
    float bias = minBias + maxBias * (1.0f - clamp(dot(Normal, fragToLight), 0.0f, 1.0f));
    const int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0f + (viewDistance / far_plane)) / 100.0f;
    for (int i = 0; i < samples; ++i) {
        /*int randomIndex = randInt(fragToLight * i, 100, samples);*/
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;
        if (currentDepth - bias > closestDepth)
            shadow += 1.0f;
        /*if (currentDepth - bias - closestDepth > 0.5f)*/
    }
    shadow /= float(samples);

    return shadow;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffTex = vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = material.diffuseColor * light.color * diff * diffTex;
    vec3 ambient = 0.01f * material.diffuseColor * diffTex;

    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0f;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    /*spec = pow(max(dot(norm,halfwayDir), 0.0f), material.shininess);*/
    spec = pow(max(dot(norm,halfwayDir), 0.0f), 32.0f);

    /*vec3 specular = material.specularColor * light.color * spec * vec3(texture(material.specular, TexCoords));*/
    vec3 specular = light.color * spec * vec3(texture(material.specular, TexCoords));

    /*float shadow = 0.0f;*/

    float distance = length(light.position - FragPos);
    float attenuation = 1.0f / (distance * distance);

    float shadow = (dot(norm, lightDir) < 0.0f) ? 0.0f : ShadowCalculation(FragPos);
    /*shadow -= attenuation;*/
    /*shadow *= attenuation;*/

    vec3 result = ambient + (1.0f - shadow) * (diffuse + specular) * attenuation;
    result = pow(result, vec3(1.0/2.2));
    color = vec4(result, 1.0f);
}
