#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

layout (location = 3) in ivec4 boneIDs;
layout (location = 4) in vec4 boneWeights;

layout (location = 5) in vec3 tangent;
layout (location = 6) in vec3 bitangent;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
    FragPos = vec3(model * vec4(position, 1.0f));
    /*Normal = mat3(transpose(inverse(model))) * normal;*/
    TexCoords = texCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * bitangent);
    vec3 N = normalize(normalMatrix * normal);

    //2nd approach
    /*vec3 T = normalize(vec3(model * vec4(tangent, 0.0f)));*/
    /*vec3 N = normalize(vec3(model * vec4(normal, 0.0f)));*/
    /*T = normalize(T - dot(T, N) * N);*/
    /*vec3 B = cross(T, N);*/

    mat3 TBN = mat3(T, B, N);

    Normal = N;
    TangentLightPos = TBN * lightPos;
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * FragPos;
}
