#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D height;
    sampler2D normal;
    float shininess;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
};

#define NR_POINT_LIGHTS 4

in VS_OUT {
    vec3 WorldFragPos;
    vec2 TexCoords;
    /*vec3 TangentLightPos;*/
    vec3 TangentViewPos;
    vec3 TangentViewDirection;
    vec3 TangentFragPos;
    mat3 TBN;
    vec3 WorldNORMAL;
    vec3 TangentNORMAL;
} fs_in;

out vec4 color;
/*in vec3 tempNormal;*/

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

const float height_scale = 0.005f;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) {
    float height = texture(material.height, texCoords).r;
    return texCoords - viewDir.xy / viewDir.z * (height * height_scale);
}

vec2 ParallaxOcclusionMapping(vec2 texCoords, vec3 viewDir) { 
    float numLayers = 10;
    float layerDepth = 1.0f / numLayers;
    float currentLayerDepth = 0.0f;

    vec2 P = viewDir.xy / viewDir.z * height_scale;
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = (0.0f - texture(material.height, currentTexCoords).r) * 2.0f - 1.0f;

    while (currentLayerDepth < currentDepthMapValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = (0.0f - texture(material.height, currentTexCoords).r) * 2.0f - 1.0f;
        currentLayerDepth += layerDepth;
    }
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = (0.0f - texture(material.height, prevTexCoords).r * 2.0f - 1.0f) - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0f - weight);

    return finalTexCoords;
}

void main() {
    vec3 norm;
    /*norm = fs_in.TangentNORMAL;*/
    /*[>[>vec2 texCoords;<]<]*/
    /*[>color = vec4(fs_in.tempNormal, 1.0f);<]*/
    /*norm = fs_in.TBN[2];*/
    /*color = vec4(fs_in.NORMAL * 0.5f + 0.5f, 1.0f);*/
    /*color = vec4(norm * 0.5f + 0.5f, 1.0f);*/
    /*color = vec4(fs_in.TexCoords, 1.0f, 1.0f);*/

    bool parallaxMapping = false;
    if (parallaxMapping) {
        vec2 texCoords = ParallaxOcclusionMapping(fs_in.TexCoords, fs_in.TangentViewDirection);
        /*vec2 texCoords = ParallaxMapping(fs_in.TexCoords, fs_in.TangentViewDirection);*/
        /*if (texCoords.x > 1.0f || texCoords.y > 1.0f || texCoords.x < 0.0f || texCoords.y < 0.0f)*/
            /*discard;*/
        norm = texture(material.normal, texCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0);
    }
    else {
        norm = texture(material.normal, fs_in.TexCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0);
        /*color = vec4(norm, 1.0f);*/
        /*return;*/
    }

    vec3 result = vec3(0.0f);
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        PointLight light = pointLights[i];
        vec3 tangentLightPos = (fs_in.TBN * light.position);
        vec3 lightDir = normalize(tangentLightPos - fs_in.TangentFragPos);
        float diff = max(dot(lightDir, norm), 0.0);

        vec3 halfwayDir = normalize(lightDir + fs_in.TangentViewDirection);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0f);

        float distance = length(tangentLightPos - fs_in.TangentFragPos);
        float attenuation = 1.0f / (distance*distance);

        vec3 ambient = vec3(0.0f);
        vec3 diffuse = vec3(0.0f);
        vec3 specular = vec3(0.0f);
        ambient = light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
        diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
        specular = spec * vec3(texture(material.specular, fs_in.TexCoords));
        result += (ambient + diffuse + specular) * attenuation;
    }
    color = vec4(result, 1.0f);
    color = vec4(pow(color.rgb, vec3(1.0/2.2)), 1.0);
}
