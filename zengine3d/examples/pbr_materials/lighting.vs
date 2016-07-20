#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 5) in vec3 tangent;
layout (location = 6) in vec3 bitangent;

/*#define POSITION_LOCATION    0*/
/*#define TEX_COORD_LOCATION   1*/
/*#define NORMAL_LOCATION      2*/
/*#define BONE_ID_LOCATION     3*/
/*#define BONE_WEIGHT_LOCATION 4*/
/*#define TANGENT_LOCATION     5*/
/*#define BITANGENT_LOCATION   6*/

out VS_OUT {
    vec3 WorldFragPos;
    vec2 TexCoords;
    /*vec3 TangentLightPos;*/
    vec3 TangentViewPos;
    vec3 TangentViewDirection;
    vec3 TangentFragPos;

    /*vec3 WorldLightPos;*/
    /*vec3 WorldViewPos;*/
    /*vec3 WorldFragPos;*/
    mat3 TBN;
    vec3 WorldNORMAL;
    vec3 TangentNORMAL;
} vs_out;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

/*uniform vec3 lightPos;*/
uniform vec3 viewPos;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.WorldFragPos = vec3(model * vec4(position, 1.0f));
    vs_out.TexCoords = texCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * bitangent);
    vec3 N = normalize(normalMatrix * normal);
    vs_out.WorldNORMAL = N;

    //2nd approach
    /*vec3 T = normalize(vec3(model * vec4(tangent, 0.0f)));*/
    /*vec3 N = normalize(vec3(model * vec4(normal, 0.0f)));*/
    /*T = normalize(T - dot(T, N) * N);*/
    /*vec3 B = cross(T, N);*/

    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentNORMAL = inverse(transpose(TBN)) * vs_out.WorldNORMAL;

    /*vs_out.TangentLightPos = TBN * lightPos;*/
    vs_out.TangentViewPos =  TBN * viewPos;
    vs_out.TangentFragPos =  TBN * vs_out.WorldFragPos;
    vs_out.TangentViewDirection =  normalize(vs_out.TangentViewPos - vs_out.TangentFragPos);

    /*[>mat3 TBN = mat3(T, B, N);<]*/
    vs_out.TBN = TBN;

    /*[>vs_out.WorldLightPos = lightPos;<]*/
    /*vs_out.WorldViewPos  = viewPos;*/
    /*[>vs_out.WorldFragPos  = vs_out.FragPos;<]*/
    /*vs_out.WorldFragPos = vec3(model * vec4(position, 1.0f));*/
}
