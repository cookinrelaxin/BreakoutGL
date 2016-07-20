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
uniform sampler2D depthMap;

uniform bool parallaxMapping;
uniform float height_scale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) {
    float height = texture(depthMap, texCoords).r;
    return texCoords - viewDir.xy / viewDir.z * (height * height_scale);
}

/*vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir) { */
    /*const float minLayers = 10;*/
    /*const float maxLayers = 20;*/
    /*float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0f, 0.0f, 1.0f), viewDir)));*/

    /*vec2 P = viewDir.xy / viewDir.z * height_scale;*/
    /*vec2 deltaTexCoords = P / numLayers;*/

    /*vec2 currentTexCoords = texCoords;*/
    /*float currentDepthMapValue = texture(depthMap, currentTexCoords).r;*/

    /*while (currentLayerDepth < currentDepthMapValue) {*/
        /*currentTexCoords -= deltaTexCoords;*/
        /*currentDepthMapValue = texture(depthMap, currentTexCoords).r;*/
        /*currentLayerDepth += layerDepth;*/
    /*}*/
    /*return currentTexCoords;*/
/*}*/

vec2 ParallaxOcclusionMapping(vec2 texCoords, vec3 viewDir) { 
    const float minLayers = 10;
    const float maxLayers = 20;
    /*float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0f, 0.0f, 1.0f), viewDir)));*/
    float numLayers = maxLayers;
    float layerDepth = 1.0f / numLayers;
    float currentLayerDepth = 0.0f;

    vec2 P = viewDir.xy / viewDir.z * height_scale;
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0f - weight);

    return finalTexCoords;
}

void main() {
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    vec2 texCoords;
    vec3 normal;
    if (parallaxMapping) {
        texCoords = ParallaxOcclusionMapping(fs_in.TexCoords, viewDir);
        if (texCoords.x > 1.0f || texCoords.y > 1.0f || texCoords.x < 0.0f || texCoords.y < 0.0f)
            discard;
        normal = texture(normalMap, texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
    }
    else {
        texCoords = fs_in.TexCoords;
        normal = vec3(0.0f, 0.0f, 1.0f);
    }

    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;
    vec3 ambient = 0.1 * color;

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * color;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);
    vec3 specular = vec3(0.2f) * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}
