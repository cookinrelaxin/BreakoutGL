#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

// layout (location = 3) in vec4 boneIDs;
// layout (location = 4) in vec4 boneWeights;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// const int MAX_BONES = 100;
// uniform mat4 bonesTransforms[MAX_BONES];
// 
void main() {
//     mat4 boneTransform = bones[boneIDs[0]] * boneWeights[0]
//                        + bones[boneIDs[1]] * boneWeights[1] 
//                        + bones[boneIDs[2]] * boneWeights[2] 
//                        + bones[boneIDs[3]] * boneWeights[3];
// 
    // gl_Position = projection * view * model * boneTransform * vec4(position, 1.0f);
    gl_Position = projection * view * model * vec4(position, 1.0f);
    TexCoords = texCoords;
}
