#version 330 core

struct Material {
    vec3 diffuseColor;
    vec3 specularColor;

    float blendCoefficient;
    sampler2D diffuse;
    sampler2D specular;
    sampler2D height;
    sampler2D normal;
    float shininess;
};

struct Light {
    /*vec3 position;*/
    vec3 color;
    float intensity;
};

in vec2 TexCoords;
in vec3 Normal;
/*in vec3 FragPos;*/

in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform float far_plane;

uniform samplerCube depthMap;
uniform float time;

// vec2 ParallaxOcclusionMapping(vec2 texCoords, vec3 viewDir) { 
//     const float height_scale = 0.025f;
//     const float minLayers = 10;
//     const float maxLayers = 20;
//     float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0f, 0.0f, 1.0f), viewDir)));
//     float layerDepth = 1.0f / numLayers;
//     float currentLayerDepth = 0.0f;
// 
//     vec2 P = viewDir.xy / viewDir.z * height_scale;
//     vec2 deltaTexCoords = P / numLayers;
// 
//     vec2 currentTexCoords = texCoords;
//     float currentDepthMapValue = texture(material.height, currentTexCoords).r;
// 
//     while (currentLayerDepth < currentDepthMapValue) {
//         currentTexCoords -= deltaTexCoords;
//         currentDepthMapValue = texture(material.height, currentTexCoords).r;
//         currentLayerDepth += layerDepth;
//     }
// 
//     vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
// 
//     float afterDepth = currentDepthMapValue - currentLayerDepth;
//     float beforeDepth = texture(material.height, prevTexCoords).r - currentLayerDepth + layerDepth;
// 
//     float weight = afterDepth / (afterDepth - beforeDepth);
//     vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0f - weight);
// 
//     return finalTexCoords;
// }

float gamma = 2.2f;
float colorDepth = mix(2.0f, 255.0f, pow(clamp(mix(-0.2f, 1.2f, abs(2.0 * fract(time / 11.0f) - 1.0f)), 0.0f, 1.0f), 2.0f));

float random(vec3 seed, float freq) {
    float dt = dot(floor(seed * freq), vec3(53.1215f, 21.1352f, 9.1322f));
    return fract(sin(dt) * 2105.2354f);
}

vec3 screenSpaceDither(vec2 fragPos) {
    vec3 vDither = vec3(dot(vec2(131.0f, 312.0f), fragPos.xy));
    vDither.rgb = fract(vDither.rgb / vec3(103.0f, 71.0f, 97.0f));
    return vDither.rgb / 255.0f;
}

float InterleavedGradientNoise(vec2 fragPos) {
    const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(fragPos, magic.xy)));
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

// float ShadowCalculation(vec3 fragPos) {
//     vec3 fragToLight = fragPos - light.position;
//     float currentDepth = length(fragToLight);
// 
//     float shadow = 0.0f;
//     /*float bias = 0.15f;*/
//     float minBias = 0.0f;
//     float maxBias = .30f;
//     float bias = minBias + maxBias * (1.0f - clamp(dot(Normal, fragToLight), 0.0f, 1.0f));
//     const int samples = 20;
//     float viewDistance = length(viewPos - fragPos);
//     /*float diskRadius = (1.0f + (viewDistance / far_plane)) / 20.0f;*/
//     /*float diskRadius = (1.0f + currentDepth) / 100.0f;*/
//     float diskRadius = (1.0f + (1.0f - currentDepth) * 3) / textureSize(depthMap, 0).x;
//     for (int i = 0; i < samples; ++i) {
//         float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
//         closestDepth *= far_plane;
//         if (currentDepth - bias > closestDepth)
//             shadow += 1.0f;
//     }
//     shadow /= float(samples);
// 
//     return shadow;
// }

void main() {
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    /*vec3 norm = normalize(Normal);*/

    /*vec2 texCoords = ParallaxOcclusionMapping(TexCoords, viewDir);*/
    vec2 texCoords = TexCoords;
    vec3 norm = texture(material.normal, texCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0);

    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);

    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffTex = vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = material.diffuseColor * light.color * light.intensity * diff * diffTex;
    vec3 ambient = 0.0001f * light.intensity * material.diffuseColor * diffTex;

    float spec = 0.0f;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(norm,halfwayDir), 0.0f), material.shininess);

    /*vec3 specular = material.specularColor * light.color * light.intensity * spec * vec3(texture(material.specular, TexCoords));*/
    vec3 specular = light.color * light.intensity * spec * vec3(texture(material.specular, texCoords));


    float distance = length(TangentLightPos - TangentFragPos);
    float attenuation = 1.0f / (distance * distance);

    /*float shadow = (dot(norm, lightDir) < 0.0f) ? 0.0f : ShadowCalculation(FragPos);*/
    float shadow = 0.0f;

    /*vec3 result = ambient + (1.0f - shadow) * (diffuse + specular) * attenuation;*/
    /*vec3 result = specular * attenuation;*/
    vec3 result = diffuse * attenuation;

    // gamma correct
    result = pow(result, vec3(1.0f/gamma));

    // dither to fix gradient banding
    result += vec3(InterleavedGradientNoise(gl_FragCoord.xy + time) / 255.0);

    color = vec4(result, 1.0f);
}
