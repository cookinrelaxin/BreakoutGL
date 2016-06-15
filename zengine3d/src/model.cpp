#include "model.h"
#include <SOIL.h>

#include <cassert>

GLint TextureFromFile(const char* path, std::string directory);
glm::mat4 toGLM(aiMatrix4x4 mat);

Model::Model(GLchar* path) {
   this->loadModel(path);
}

void Model::updateAnimation(timeInSeconds) {

}

void Model::Draw(Shader shader) {

    for (int i = 0; i < this->meshes.size(); i++) {
       this->meshes[i].Draw(shader);
    }
}

aiNodeAnim* findAnimationChannel(const aiScene* scene, const aiNode* node) {
    //for now lets assume there is one animation only
    const aiAnimation* animation = scene->mAnimations[0];
    for (int i = 0; i < animation->mNumChannels; i++) {
       const aiNodeAnim* nodeAnimation = animation->channels[i]; 
       if (std::string(nodeAnimation->mNodeName.data) == std::string(node->mName.data)) {
           return nodeAnimation;
       }
    }
    throw std::runtime_error("No animation channel for mesh");
    return NULL;
}

void Model::loadModel(std::string path) {
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path,
                                           aiProcess_Triangulate
                                           | aiProcess_FlipUVs
                                           | aiProcess_GenSmoothNormals);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR:ASSIMP:: " << import.GetErrorString() << std::endl;
        return;
    }
    this->directory = path.substr(0, path.find_last_of('/'));
    this->initializeNode(scene->mRootNode, scene);
}

void Model::generatePositionKeyFrames(aiNodeAnim* animation, Node* node) {
    for (int i = 0; i < nodeAnim->mNumPositionKeys; i++) {
        double time = nodeAnim->mPositionKeys[i].mTime;
        aiVector3D vec = nodeAnim->mPositionKeys[i].mValue;

        PositionKey key;
        key.time = time;
        key.transformation = glm::mat4(vec.x, vec.y, vec.z);

        node->positionKeys.push_back(key);
    }
}

void Model::generateRotationKeyFrames(aiNodeAnim* animation, Node* node) {
    for (int i = 0; i < nodeAnim->mNumRotationKeys; i++) {
        double time = nodeAnim->mRotationKeys[i].mTime;
        aiVector3D vec = nodeAnim->mRotationKeys[i].mValue;

        RotationKey key;
        key.time = time;
        key.transformation = glm::mat4(vec.x, vec.y, vec.z);

        node->rotationKeys.push_back(key);
    }
}

void Model::generateScalingKeyFrames(aiNodeAnim* animation, Node* node) {
    for (int i = 0; i < nodeAnim->mNumScalingKeys; i++) {
        double time = nodeAnim->mScalingKeys[i].mTime;
        aiVector3D vec = nodeAnim->mScalingKeys[i].mValue;

        ScalingKey key;
        key.time = time;
        key.transformation = glm::mat4(vec.x, vec.y, vec.z);

        node->scalingKeys.push_back(key);
    }
}

void Model::generateKeyFrames(aiNodeAnim* animation, Node* node) {
    generatePositionKeyFrames(animation, Node*);
    generateRotationKeyFrames(animation, Node*);
    generateScalingKeyFrames(animation, Node*);
}

void generateMeshes(aiScene* scene, Node* node) {
    for (int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        node->meshes.push_back(generateMesh(mesh, scene))
    }
}

void Model::generateNode(aiNode* ai_node, const aiScene* scene) {
    std::cout << "node->mName.C_Str(): " << node->mName.C_Str() << std::endl;

    Node* node = new Node;
    if (rootNode == nullptr) rootNode = node;
    node->name = std::string(ai_node->mName.data);

    aiNodeAnim* nodeAnim = findAnimationChannel(scene, ai_node);
    generateKeyFrames(nodeAnim, node);
    generateMeshes(scene, node);

    for (int i = 0; i < ai_node->mNumChildren; i++) {
        this->processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        glm::vec3 vec;
        vec.x = mesh->mVertices[i].x;
        vec.y = mesh->mVertices[i].y;
        vec.z = mesh->mVertices[i].z;
        vertex.Position = vec;

        vec.x = mesh->mNormals[i].x;
        vec.y = mesh->mNormals[i].y;
        vec.z = mesh->mNormals[i].z;
        vertex.Normal = vec;

        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x,
                                         mesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }
    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps
            = this->loadMaterialTextures(material,
                                         aiTextureType_DIFFUSE,
                                         "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps
            = this->loadMaterialTextures(material,
                                         aiTextureType_SPECULAR,
                                         "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    for (GLuint i = 0; i < mesh->mNumBones; i++) {

        aiBone* bone = mesh->mBones[i];
        for (GLuint j = 0; j < bone->mNumWeights; j++) {

            float weight = bone->mWeights[i].mWeight;
            // std::cout << "bone weight: " << bone->mWeights[i].mWeight << std::endl;

            int vertexID = bone->mWeights[i].mVertexId;

            assert(vertexID < mesh->mNumVertices);

            vertices[vertexID].boneIDs.push_back(i);
            vertices[vertexID].boneWeights.push_back(weight);
        }

        std::string name(mesh->mBones[i]->mName.data);
        std::cout << "bone name: " << name << std::endl;

        Bone b;
        aiMatrix4x4 oM = mesh->mBones[i]->mOffsetMatrix;
        b.boneSpaceTransform = toGLM(oM);
        bones.push_back(b);

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat,
                                                 aiTextureType type,
                                                 std::string typeName) {
    std::vector<Texture> textures;
    for (int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        GLboolean skip = false;
        for (int j = 0; j < textures_loaded.size(); j++) {
            if (textures_loaded[j].path == str) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
           Texture texture;
           texture.id = TextureFromFile(str.C_Str(), this->directory);
           texture.type = typeName;
           texture.path = str;
           textures.push_back(texture);
        }
    }
    return textures;
}

glm::mat4 toGLM(aiMatrix4x4 mat) {
    return glm::mat4(
            mat[0][1], mat[0][2], mat[0][3], mat[0][4],
            mat[1][1], mat[1][2], mat[1][3], mat[1][4],
            mat[2][1], mat[2][2], mat[2][3], mat[2][4],
            mat[3][1], mat[3][2], mat[3][3], mat[3][4]);
}

GLint TextureFromFile(const char* path, std::string directory) {
    std::string filename(path);
    filename = directory + '/' + filename;
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height;
    std::cout << "load file: " << filename << std::endl;
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 width,
                 height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 image);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}
