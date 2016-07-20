#include "model.h"

#include <SOIL.h>

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4
#define TANGENT_LOCATION     5
#define BITANGENT_LOCATION   6

#define DIFFUSE_TEXTURE_LOCATION  0
#define SPECULAR_TEXTURE_LOCATION 1
#define HEIGHT_TEXTURE_LOCATION   2
#define NORMAL_TEXTURE_LOCATION   3

glm::mat4 aiToGlm(const aiMatrix4x4& v) {
    glm::mat4 out;
    assert(sizeof(out) == sizeof(v));
    memcpy(&out, &v, sizeof(v));
    return glm::transpose(out);
}

glm::vec3 aiToGlm(const aiVector3D& v) {
    glm::vec3 out;
    assert(sizeof(out) == sizeof(v));
    memcpy(&out, &v, sizeof(v));
    return out;
}

GLint TextureFromFile(const char* path, std::string directory);

void Model::VertexBoneData::addBoneData(uint boneID, float weight)
{
    for (uint i = 0 ; i < NUM_BONES_PER_VERTEX; i++) {
        if (weights[i] == 0.0) {
            IDs[i]     = boneID;
            weights[i] = weight;
            return;
        }
    }

    // should never get here - more bones than we have space for
    throw std::runtime_error("too many bones");
}

glm::quat aiToGlm(const aiQuaternion& v) {
    return glm::quat(v.w, v.x, v.y, v.z);
}

bool Model::loadMesh(const std::string& fileName) {
    return [&](){
        assert(glGetError() == GL_NO_ERROR);
        glGenVertexArrays(1, &VAO);

        assert(glGetError() == GL_NO_ERROR);
        glBindVertexArray(VAO);

        glGenBuffers(NUM_VBs, m_Buffers);

        this->m_pScene = m_Importer.ReadFile(fileName.c_str()
                , aiProcess_Triangulate
                //| aiProcess_GenSmoothNormals
                | aiProcess_CalcTangentSpace
                | aiProcess_FlipUVs
                //| aiProcess_JoinIdenticalVertices
                //| aiProcess_FixInfacingNormals
                );
        //return true;
        return [&]() {
            if (m_pScene) {
                this->directory = fileName.substr(0, fileName.find_last_of('/'));
                std::string format = fileName.substr(fileName.find_last_of('.')+1);
                if (format == "dae")
                    this->flipAxes = true;
                this->m_GlobalInverseTransform = glm::inverse(aiToGlm(m_pScene->mRootNode->mTransformation));

                bool ok = initFromScene(m_pScene, fileName);
                glBindVertexArray(0);
                return ok;
            }
            else {
                std::cout << "Error parsing ' " << fileName << " : " << m_Importer.GetErrorString() << std::endl;
                glBindVertexArray(0);
                return false;
            }
        }();
    }();

}


bool Model::initFromScene(const aiScene* pScene, const std::string& fileName) {
    meshes.resize(pScene->mNumMeshes);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<glm::vec2> texCoords;
    std::vector<VertexBoneData> bones;
    std::vector<uint> indices;

    unsigned int numVertices = 0;
    unsigned int numIndices = 0;

    for (uint i = 0; i < meshes.size(); i++) {
        meshes[i].materialIndex = pScene->mMeshes[i]->mMaterialIndex;
        meshes[i].numIndices    = pScene->mMeshes[i]->mNumFaces * 3;
        meshes[i].baseVertex    = numVertices;
        meshes[i].baseIndex     = numIndices;

        numVertices += pScene->mMeshes[i]->mNumVertices;
        numIndices  += meshes[i].numIndices;
    }

    //map textures to meshes
    for (int i = 0; i < meshes.size(); i++) {
        //std::cout << "map textures to mesh" << std::endl;
        const unsigned int materialIndex = meshes[i].materialIndex; 
        const aiMaterial* material = pScene->mMaterials[materialIndex];
        //std::cout << "texture count: " << material->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;

        for (int j = 0; j < material->GetTextureCount(aiTextureType_DIFFUSE); j++) {
            aiString str;
            material->GetTexture(aiTextureType_DIFFUSE, j, &str);
            //std::cout << "diffuse texture name: " << str.C_Str() << std::endl;
            meshes[i].textures.push_back(str.C_Str());
        }
        // same for specular textures...
        for (int j = 0; j < material->GetTextureCount(aiTextureType_SPECULAR); j++) {
            aiString str;
            material->GetTexture(aiTextureType_SPECULAR, j, &str);
            //std::cout << "specular texture name: " << str.C_Str() << std::endl;
            meshes[i].textures.push_back(str.C_Str());
        }
        for (int j = 0; j < material->GetTextureCount(aiTextureType_NORMALS); j++) {
            aiString str;
            material->GetTexture(aiTextureType_NORMALS, j, &str);
            //std::cout << "booga: " << str.C_Str() << std::endl;
            meshes[i].textures.push_back(str.C_Str());
        }
        for (int j = 0; j < material->GetTextureCount(aiTextureType_HEIGHT); j++) {
            aiString str;
            material->GetTexture(aiTextureType_HEIGHT, j, &str);
            //std::cout << "mooga: " << str.C_Str() << std::endl;
            meshes[i].textures.push_back(str.C_Str());
        }
        for (int j = 0; j < material->GetTextureCount(aiTextureType_DISPLACEMENT); j++) {
            aiString str;
            material->GetTexture(aiTextureType_DISPLACEMENT, j, &str);
            //std::cout << "dooga: " << str.C_Str() << std::endl;
            meshes[i].textures.push_back(str.C_Str());
        }
    }

    // Reserve space in the vectors for the vertex attributes and indices
    positions.reserve(numVertices);
    normals.reserve(numVertices);
    tangents.reserve(numVertices);
    bitangents.reserve(numVertices);
    texCoords.reserve(numVertices);
    bones.resize(numVertices);
    indices.reserve(numIndices);

    // Initialize the meshes in the scene one by one
    for (uint i = 0 ; i < meshes.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        initMesh(i, paiMesh, positions, normals, tangents, bitangents, texCoords, bones, indices);
    }
    assert(glGetError() == GL_NO_ERROR);

    if (!initMaterials(pScene, fileName)) {
        return false;
    }

    assert(glGetError() == GL_NO_ERROR);
    // Generate and populate the buffers with vertex attributes and the indices
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
    assert(glGetError() == GL_NO_ERROR);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords[0]) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);
    assert(glGetError() == GL_NO_ERROR);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
    assert(glGetError() == GL_NO_ERROR);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BONE_ID_LOCATION);
    glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    assert(glGetError() == GL_NO_ERROR);
    glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
    glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
    assert(glGetError() == GL_NO_ERROR);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TANGENT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tangents[0]) * tangents.size(), &tangents[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TANGENT_LOCATION);
    glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
    assert(glGetError() == GL_NO_ERROR);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BITANGENT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bitangents[0]) * bitangents.size(), &bitangents[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BITANGENT_LOCATION);
    glVertexAttribPointer(BITANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
    assert(glGetError() == GL_NO_ERROR);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
    assert(glGetError() == GL_NO_ERROR);

    // return GLCheckError();
    return true;
}


void Model::initMesh(uint meshIndex,
                     const aiMesh* paiMesh,
                     std::vector<glm::vec3>& positions,
                     std::vector<glm::vec3>& normals,
                     std::vector<glm::vec3>& tangents,
                     std::vector<glm::vec3>& bitangents,
                     std::vector<glm::vec2>& texCoords,
                     std::vector<VertexBoneData>& bones,
                     std::vector<uint>& indices) {
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    // Populate the vertex attribute vectors
    for (uint i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos       = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal    = &(paiMesh->mNormals[i]);
        const aiVector3D* pTangent   = &(paiMesh->mTangents[i]);
        const aiVector3D* pBitangent = &(paiMesh->mBitangents[i]);
        const aiVector3D* pTexCoord  = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        if (flipAxes) {
            positions.push_back( glm::vec3(pPos->x,       pPos->z,       -pPos->y));
            normals.push_back(   glm::vec3(pNormal->x,    pNormal->z,    -pNormal->y));
            tangents.push_back(  glm::vec3(pTangent->x,   pTangent->z,   -pTangent->y));
            bitangents.push_back(glm::vec3(pBitangent->x, pBitangent->z, -pBitangent->y));
            texCoords.push_back( glm::vec2(pTexCoord->x,  pTexCoord->y));
        }
        else {
            positions.push_back( glm::vec3(pPos->x,       pPos->y,       pPos->z));
            normals.push_back(   glm::vec3(pNormal->x,    pNormal->y,    pNormal->z));
            tangents.push_back(  glm::vec3(pTangent->x,   pTangent->y,   pTangent->z));
            bitangents.push_back(glm::vec3(pBitangent->x, pBitangent->y, pBitangent->z));
            texCoords.push_back( glm::vec2(pTexCoord->x,  pTexCoord->y));
        }
    }

    loadBones(meshIndex, paiMesh, bones);

    // Populate the index buffer
    for (uint i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        //assert(Face.mNumIndices == 3);
        indices.push_back(Face.mIndices[0]);
        indices.push_back(Face.mIndices[1]);
        indices.push_back(Face.mIndices[2]);
    }
}


void Model::loadBones(uint meshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones) {
    //std::cout << "num bones: " << pMesh->mNumBones << std::endl;
    for (uint i = 0 ; i < pMesh->mNumBones ; i++) {
        uint BoneIndex = 0;
        std::string BoneName(pMesh->mBones[i]->mName.data);

        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
            // Allocate an index for a new bone
            BoneIndex = numBones;
            numBones++;
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
            m_BoneInfo[BoneIndex].boneOffset = aiToGlm(pMesh->mBones[i]->mOffsetMatrix);
            m_BoneMapping[BoneName] = BoneIndex;
        }
        else {
            BoneIndex = m_BoneMapping[BoneName];
        }

        for (uint j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) {
            uint VertexID = meshes[meshIndex].baseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight  = pMesh->mBones[i]->mWeights[j].mWeight;
            Bones[VertexID].addBoneData(BoneIndex, Weight);
        }
    }
}

struct Material {
    std::string name;
    glm::vec3   diffuseColor;
    glm::vec3   specularColor;
    glm::vec3   ambientColor;
    glm::vec3   emissiveColor;
    glm::vec3   transparentColor;

    float phongExponent;
    float shininessCoefficient;

    Texture diffuseTexture;
    Texture specularTexture;
    Texture ambientTexture;
    Texture emissiveTexture;
    Texture heightTexture;
    Texture normalTexture;
    Texture shininessTexture;
    Texture opacityTexture;
    Texture displacementTexture;
    Texture lightTexture;
    Texture reflectionTexture;

    void print();
};

void Texture::print() {
    std::cout << "Texture Properties" << std::endl;
    std::cout << "-------------------" << std::endl;

    std::cout << "id: "
        << id
        << std::endl;

    std::cout << "type: "
        << type
        << std::endl;

    std::cout << "path: "
        << path.C_Str()
        << std::endl;
}

void Material::print() {
    std::cout << "Material Properties" << std::endl;
    std::cout << "-------------------" << std::endl;

    std::cout << "name: "
        << name
        << std::endl;

    std::cout << "diffuseColor: "
        << diffuseColor.x
        << " "
        << diffuseColor.y
        << " "
        << diffuseColor.z
        << " "
        << std::endl;

    std::cout << "specularColor: "
        << specularColor.x
        << " "
        << specularColor.y
        << " "
        << specularColor.z
        << " "
        << std::endl;

    std::cout << "ambientColor: "
        << ambientColor.x
        << " "
        << ambientColor.y
        << " "
        << ambientColor.z
        << " "
        << std::endl;

    std::cout << "emissiveColor: "
        << emissiveColor.x
        << " "
        << emissiveColor.y
        << " "
        << emissiveColor.z
        << " "
        << std::endl;

    std::cout << "transparentColor: "
        << transparentColor.x
        << " "
        << transparentColor.y
        << " "
        << transparentColor.z
        << " "
        << std::endl;

    std::cout << "phongExponent: "
        << phongExponent
        << std::endl;

    std::cout << "shininessCoefficient: "
        << shininessCoefficient
        << std::endl;

    diffuseTexture      .print();
    std::cout << std::endl;
    specularTexture     .print();
    std::cout << std::endl;
    ambientTexture      .print();
    std::cout << std::endl;
    emissiveTexture     .print();
    std::cout << std::endl;
    heightTexture       .print();
    std::cout << std::endl;
    normalTexture       .print();
    std::cout << std::endl;
    shininessTexture    .print();
    std::cout << std::endl;
    opacityTexture      .print();
    std::cout << std::endl;
    displacementTexture .print();
    std::cout << std::endl;
    lightTexture        .print();
    std::cout << std::endl;
    reflectionTexture   .print();
    std::cout << std::endl;
}

bool Model::initMaterials(const aiScene* pScene, const std::string& fileName) {
    // Initialize the materials
    assert(glGetError() == GL_NO_ERROR);
    std::cout << "num materials: " << pScene->mNumMaterials << std::endl;
    for (uint i = 0 ; i < pScene->mNumMaterials ; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];
        Material m;
        {
            aiString ai_materialName;
            pMaterial->Get(AI_MATKEY_NAME, ai_materialName);
            std::string materialName(ai_materialName.C_Str());
            m.name = materialName;
        }

        {
            aiColor3D ai_colorDiffuse;
            pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, ai_colorDiffuse);
            glm::vec3 colorDiffuse(ai_colorDiffuse.r, ai_colorDiffuse.g, ai_colorDiffuse.b);
            m.diffuseColor = colorDiffuse;
        }

        {
            aiColor3D ai_colorSpecular;
            pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, ai_colorSpecular);
            glm::vec3 colorSpecular(ai_colorSpecular.r, ai_colorSpecular.g, ai_colorSpecular.b);
            m.specularColor = colorSpecular;
        }

        {
            aiColor3D ai_colorAmbient;
            pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ai_colorAmbient);
            glm::vec3 colorAmbient(ai_colorAmbient.r, ai_colorAmbient.g, ai_colorAmbient.b);
            m.ambientColor = colorAmbient;
        }

        {
            aiColor3D ai_colorEmissive;
            pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, ai_colorEmissive);
            glm::vec3 colorEmissive(ai_colorEmissive.r, ai_colorEmissive.g, ai_colorEmissive.b);
            m.emissiveColor = colorEmissive;
        }

        {
            aiColor3D ai_colorTransparent;
            pMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, ai_colorTransparent);
            glm::vec3 colorTransparent(ai_colorTransparent.r, ai_colorTransparent.g, ai_colorTransparent.b);
            m.transparentColor = colorTransparent;
        }

        {
            float phongExponent;
            pMaterial->Get(AI_MATKEY_SHININESS, phongExponent);
            m.phongExponent = phongExponent;
        }

        {
            float shininessCoefficient;
            pMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, shininessCoefficient);
            //m.shininessCoefficient = shininessCoefficient;
            m.shininessCoefficient = 1.0f;
        }

        {
            aiTextureType type = aiTextureType_DIFFUSE;
            unsigned int textureCount = pMaterial->GetTextureCount(type);
            std::cout << "diffuse texture count: " << textureCount << std::endl;
            for (unsigned int j = 0; j < textureCount; j++) {
                aiString texPath;
                if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                    throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
                std::cout << "texturePath: " << texPath.data << std::endl;

                if (m_Textures.find(texPath.data) == m_Textures.end()) {
                    m.diffuseTexture.id = TextureFromFile(texPath.data, this->directory);
                    m.diffuseTexture.type = "diffuse";
                    m.diffuseTexture.path = texPath;
                    m_Textures[texPath.data] = m.diffuseTexture;
                }
            }
        }

        {
            aiTextureType type = aiTextureType_SPECULAR;
            unsigned int textureCount = pMaterial->GetTextureCount(type);
            std::cout << "specular texture count: " << textureCount << std::endl;
            for (unsigned int j = 0; j < textureCount; j++) {
                aiString texPath;
                if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                    throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
                std::cout << "texturePath: " << texPath.data << std::endl;

                if (m_Textures.find(texPath.data) == m_Textures.end()) {
                    m.specularTexture.id = TextureFromFile(texPath.data, this->directory);
                    m.specularTexture.type = "specular";
                    m.specularTexture.path = texPath;
                    m_Textures[texPath.data] = m.specularTexture;
                }
            }
        }

        {
            aiTextureType type = aiTextureType_AMBIENT;
            unsigned int textureCount = pMaterial->GetTextureCount(type);
            std::cout << "ambient texture count: " << textureCount << std::endl;
            for (unsigned int j = 0; j < textureCount; j++) {
                aiString texPath;
                if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                    throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
                std::cout << "texturePath: " << texPath.data << std::endl;

                if (m_Textures.find(texPath.data) == m_Textures.end()) {
                    m.ambientTexture.id = TextureFromFile(texPath.data, this->directory);
                    m.ambientTexture.type = "ambient";
                    m.ambientTexture.path = texPath;
                    m_Textures[texPath.data] = m.ambientTexture;
                }
            }
        }

        {
            aiTextureType type = aiTextureType_EMISSIVE;
            unsigned int textureCount = pMaterial->GetTextureCount(type);
            std::cout << "emissive texture count: " << textureCount << std::endl;
            for (unsigned int j = 0; j < textureCount; j++) {
                aiString texPath;
                if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                    throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
                std::cout << "texturePath: " << texPath.data << std::endl;

                if (m_Textures.find(texPath.data) == m_Textures.end()) {
                    m.emissiveTexture.id = TextureFromFile(texPath.data, this->directory);
                    m.emissiveTexture.type = "emissive";
                    m.emissiveTexture.path = texPath;
                    m_Textures[texPath.data] = m.emissiveTexture;
                }
            }
        }

        {
            aiTextureType type = aiTextureType_HEIGHT;
            unsigned int textureCount = pMaterial->GetTextureCount(type);
            std::cout << "height texture count: " << textureCount << std::endl;
            for (unsigned int j = 0; j < textureCount; j++) {
                aiString texPath;
                if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                    throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
                std::cout << "texturePath: " << texPath.data << std::endl;

                if (m_Textures.find(texPath.data) == m_Textures.end()) {
                    m.heightTexture.id = TextureFromFile(texPath.data, this->directory);
                    m.heightTexture.type = "height";
                    m.heightTexture.path = texPath;
                    m_Textures[texPath.data] = m.heightTexture;
                }
            }
        }

        {
            aiTextureType type = aiTextureType_NORMALS;
            unsigned int textureCount = pMaterial->GetTextureCount(type);
            std::cout << "normals texture count: " << textureCount << std::endl;
            for (unsigned int j = 0; j < textureCount; j++) {
                aiString texPath;
                if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                    throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
                std::cout << "texturePath: " << texPath.data << std::endl;

                if (m_Textures.find(texPath.data) == m_Textures.end()) {
                    m.normalTexture.id = TextureFromFile(texPath.data, this->directory);
                    m.normalTexture.type = "normal";
                    m.normalTexture.path = texPath;
                    m_Textures[texPath.data] = m.normalTexture;
                }
            }
        }

        {
            aiTextureType type = aiTextureType_SHININESS;
            unsigned int textureCount = pMaterial->GetTextureCount(type);
            std::cout << "shininess texture count: " << textureCount << std::endl;
            for (unsigned int j = 0; j < textureCount; j++) {
                aiString texPath;
                if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                    throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
                std::cout << "texturePath: " << texPath.data << std::endl;

                if (m_Textures.find(texPath.data) == m_Textures.end()) {
                    m.shininessTexture.id = TextureFromFile(texPath.data, this->directory);
                    m.shininessTexture.type = "shininess";
                    m.shininessTexture.path = texPath;
                    m_Textures[texPath.data] = m.shininessTexture;
                }
            }
        }

        {
            aiTextureType type = aiTextureType_OPACITY;
            unsigned int textureCount = pMaterial->GetTextureCount(type);
            std::cout << "opacity texture count: " << textureCount << std::endl;
            for (unsigned int j = 0; j < textureCount; j++) {
                aiString texPath;
                if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                    throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
                std::cout << "texturePath: " << texPath.data << std::endl;

                if (m_Textures.find(texPath.data) == m_Textures.end()) {
                    m.opacityTexture.id = TextureFromFile(texPath.data, this->directory);
                    m.opacityTexture.type = "opacity";
                    m.opacityTexture.path = texPath;
                    m_Textures[texPath.data] = m.opacityTexture;
                }
            }
        }

        {
            aiTextureType type = aiTextureType_DISPLACEMENT;
            unsigned int textureCount = pMaterial->GetTextureCount(type);
            std::cout << "displacement texture count: " << textureCount << std::endl;
            for (unsigned int j = 0; j < textureCount; j++) {
                aiString texPath;
                if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                    throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
                std::cout << "texturePath: " << texPath.data << std::endl;

                if (m_Textures.find(texPath.data) == m_Textures.end()) {
                    m.displacementTexture.id = TextureFromFile(texPath.data, this->directory);
                    m.displacementTexture.type = "displacement";
                    m.displacementTexture.path = texPath;
                    m_Textures[texPath.data] = m.displacementTexture;
                }
            }
        }

        {
            aiTextureType type = aiTextureType_LIGHTMAP;
            unsigned int textureCount = pMaterial->GetTextureCount(type);
            std::cout << "light texture count: " << textureCount << std::endl;
            for (unsigned int j = 0; j < textureCount; j++) {
                aiString texPath;
                if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                    throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
                std::cout << "texturePath: " << texPath.data << std::endl;

                if (m_Textures.find(texPath.data) == m_Textures.end()) {
                    m.lightTexture.id = TextureFromFile(texPath.data, this->directory);
                    m.lightTexture.type = "light";
                    m.lightTexture.path = texPath;
                    m_Textures[texPath.data] = m.lightTexture;
                }
            }
        }

        {
            aiTextureType type = aiTextureType_REFLECTION;
            unsigned int textureCount = pMaterial->GetTextureCount(type);
            std::cout << "reflection texture count: " << textureCount << std::endl;
            for (unsigned int j = 0; j < textureCount; j++) {
                aiString texPath;
                if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                    throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
                std::cout << "texturePath: " << texPath.data << std::endl;

                if (m_Textures.find(texPath.data) == m_Textures.end()) {
                    m.reflectionTexture.id = TextureFromFile(texPath.data, this->directory);
                    m.reflectionTexture.type = "reflection";
                    m.reflectionTexture.path = texPath;
                    m_Textures[texPath.data] = m.reflectionTexture;
                }
            }
        }


        //type = aiTextureType_SPECULAR;
        //textureCount = pMaterial->GetTextureCount(type);
        //std::cout << "specular texture count: " << textureCount << std::endl;
        //for (unsigned int j = 0; j < textureCount; j++) {
            //aiString texPath;
            //if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                //throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
            //std::cout << "texturePath: " << texPath.data << std::endl;
          
            //if (m_Textures.find(texPath.data) == m_Textures.end()) {
                //Texture texture;
                //texture.id = TextureFromFile(texPath.data, this->directory);
                //texture.type = "specular";
                //texture.path = texPath;
                //m_Textures[texPath.data] = texture;
                //std::cout << "add new specular texture: " << texture.path.data << std::endl;
            //}
        //}

        //std::string heightMapName("hammer_HEIGHT.png");
        ////std::cout << "this->directory: " << this->directory << std::endl;
        //std::ifstream heightMapFile(this->directory + "/" + heightMapName);
        //if (heightMapFile.good()) {
            //if (m_Textures.find(heightMapName) == m_Textures.end()) {
                //Texture texture;
                //texture.id = TextureFromFile(heightMapName.c_str(), this->directory);
                //texture.type = "height";
                //texture.path = this->directory + heightMapName;
                //m_Textures[heightMapName] = texture;
                //std::cout << "add new height texture: " << heightMapName << std::endl;
            //}
        //}
        ////type = aiTextureType_HEIGHT;
        ////textureCount = pMaterial->GetTextureCount(type);
        ////std::cout << "height texture count: " << textureCount << std::endl;
        ////for (unsigned int j = 0; j < textureCount; j++) {
            ////aiString texPath;
            ////if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                ////throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
            ////std::cout << "texturePath: " << texPath.data << std::endl;
          
            ////if (m_Textures.find(texPath.data) == m_Textures.end()) {
                ////Texture texture;
                ////texture.id = TextureFromFile(texPath.data, this->directory);
                ////texture.type = "height";
                ////texture.path = texPath;
                ////m_Textures[texPath.data] = texture;
                ////std::cout << "add new height texture: " << texture.path.data << std::endl;
            ////}
        ////}

        //type = aiTextureType_NORMALS;
        //textureCount = pMaterial->GetTextureCount(type);
        //std::cout << "normal texture count: " << textureCount << std::endl;
        //for (unsigned int j = 0; j < textureCount; j++) {
            //aiString texPath;
            //if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                //throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
            //std::cout << "texturePath: " << texPath.data << std::endl;
          
            //if (m_Textures.find(texPath.data) == m_Textures.end()) {
                //Texture texture;
                //texture.id = TextureFromFile(texPath.data, this->directory);
                ////if (std::string(texPath.data).find("HEIGHT") != std::string::npos) {
                    ////texture.type = "height";
                    ////std::cout << "add new height texture: " << texture.path.data << std::endl;
                ////}
                ////else {
                //texture.type = "normal";
                //std::cout << "add new normal texture: " << texture.path.data << std::endl;
                ////}
                //texture.path = texPath;
                //m_Textures[texPath.data] = texture;
            //}
        //}

        // for (int i = 0; i < pMaterial->GetTextureCount(type); i++) {
        //     aiString str;
        //     mat->GetTexture(type, i, &str);
        //     //std::cout << "texture name: " << str.C_Str() << std::endl;

        //     if (m_Textures.find(str.C_Str()) == m_Textures.end()) {
        //         Texture texture;
        //         texture.id = TextureFromFile(str.C_Str(), this->directory);
        //         texture.type = typeName;
        //         texture.path = str;
        //         textures[str.C_Str()] = texture;
        //     }
        // }

        //std::map<std::string, Texture> diffuseMaps = this->loadMaterialTextures(
                //pMaterial,
                //aiTextureType_DIFFUSE,
                //"texture_diffuse");
        //m_Textures.insert(diffuseMaps.begin(), diffuseMaps.end());

        //std::map<std::string, Texture> specularMaps = this->loadMaterialTextures(
                //pMaterial,
                //aiTextureType_SPECULAR,
                //"texture_specular");
        //m_Textures.insert(specularMaps.begin(), specularMaps.end());
        assert(glGetError() == GL_NO_ERROR);
    m.print();
    }
    abort();
    //std::cout << "number of textures loaded: " << this->m_Textures.size() << std::endl;

    // return Ret;
    return true;
}

GLint TextureFromFile(const char* path, std::string directory) {
    assert(glGetError() == GL_NO_ERROR);
    std::string filename(path);
    filename = directory + '/' + filename;

    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height;
    std::cout << "filename: " << filename << std::endl;
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;
    assert(glGetError() == GL_NO_ERROR);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    assert(glGetError() == GL_NO_ERROR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    assert(glGetError() == GL_NO_ERROR);

    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    assert(glGetError() == GL_NO_ERROR);
    return textureID;
}

void Model::render(Shader& shader)
{
    glBindVertexArray(VAO);

    assert(glGetError() == GL_NO_ERROR);
    for (uint i = 0 ; i < meshes.size() ; i++) {
        //bind default white diffuse texture
        assert(glGetError() == GL_NO_ERROR);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, defaultTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, defaultTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, defaultTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, defaultTexture);

        assert(glGetError() == GL_NO_ERROR);
        MeshEntry entry(meshes[i]);
        //for (int j = 0; j < entry.textures.size(); j++) {
        for (int j = 0; j < entry.textures.size() && j < 4; j++) {
            Texture texture(m_Textures[entry.textures[j]]);
            //glActiveTexture(GL_TEXTURE0+j);
            std::string name = texture.type;
            if (name == "diffuse") {
                glActiveTexture(GL_TEXTURE0+DIFFUSE_TEXTURE_LOCATION);
                glUniform1i(glGetUniformLocation(shader.Program, "material.diffuse"), DIFFUSE_TEXTURE_LOCATION);
                glBindTexture(GL_TEXTURE_2D, texture.id);
                assert(glGetError() == GL_NO_ERROR);
            }
            else if (name == "specular") {
                glActiveTexture(GL_TEXTURE0+SPECULAR_TEXTURE_LOCATION);
                glUniform1i(glGetUniformLocation(shader.Program, "material.specular"), SPECULAR_TEXTURE_LOCATION);
                glBindTexture(GL_TEXTURE_2D, texture.id);
                assert(glGetError() == GL_NO_ERROR);
            }
            else if (name == "height") {
                glActiveTexture(GL_TEXTURE0+HEIGHT_TEXTURE_LOCATION);
                glUniform1i(glGetUniformLocation(shader.Program, "material.height"), HEIGHT_TEXTURE_LOCATION);
                glBindTexture(GL_TEXTURE_2D, texture.id);
                assert(glGetError() == GL_NO_ERROR);
            }
            else if (name == "normal") {
                glActiveTexture(GL_TEXTURE0+NORMAL_TEXTURE_LOCATION);
                glUniform1i(glGetUniformLocation(shader.Program, "material.normal"), NORMAL_TEXTURE_LOCATION);
                glBindTexture(GL_TEXTURE_2D, texture.id);
                assert(glGetError() == GL_NO_ERROR);
            }

            //glUniform1i(glGetUniformLocation(shader.Program, ("material."+name).c_str()), j);
           
            //std::cout << "texture.name: " << texture.path.data << std::endl;
            //std::cout << "type: " << name << std::endl;
            //std::cout << "material.name: " << name << std::endl;
          
        }
        assert(glGetError() == GL_NO_ERROR);
        glUniform3fv(glGetUniformLocation(shader.Program, "material.diffuseColor"), 1, glm::value_ptr(entry.diffuseColor));
        glUniform3fv(glGetUniformLocation(shader.Program, "material.specularColor"), 1, glm::value_ptr(entry.specularColor));
        glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), entry.shininess);
        assert(glGetError() == GL_NO_ERROR);

        glDrawElementsBaseVertex(GL_TRIANGLES,
                                 entry.numIndices,
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(uint) * entry.baseIndex),
                                 entry.baseVertex);
        assert(glGetError() == GL_NO_ERROR);

        for (int i = 0; i < entry.textures.size() && i < 4; i++) {
            glActiveTexture(GL_TEXTURE0+i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    //exit(0);

    glBindVertexArray(0);
}

void Model::renderShadows(Shader& shader)
{
    glBindVertexArray(VAO);

    for (uint i = 0 ; i < meshes.size() ; i++) {
        MeshEntry entry(meshes[i]);
        glDrawElementsBaseVertex(GL_TRIANGLES,
                                 entry.numIndices,
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(uint) * entry.baseIndex),
                                 entry.baseVertex);
    }

    glBindVertexArray(0);
}


uint Model::findPosition(float animationTime, const aiNodeAnim* pNodeAnim) {
    for (uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (animationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


uint Model::findRotation(float animationTime, const aiNodeAnim* pNodeAnim) {
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (animationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


uint Model::findScaling(float animationTime, const aiNodeAnim* pNodeAnim) {
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (animationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


void Model::calcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim) {
    if (pNodeAnim->mNumPositionKeys == 1) {
        out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    uint positionIndex = findPosition(animationTime, pNodeAnim);
    uint nextPositionIndex = (positionIndex + 1);
    assert(nextPositionIndex < pNodeAnim->mNumPositionKeys);
    float deltaTime = (float)(pNodeAnim->mPositionKeys[nextPositionIndex].mTime - pNodeAnim->mPositionKeys[positionIndex].mTime);
    float factor = (animationTime - (float)pNodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    const aiVector3D& start = pNodeAnim->mPositionKeys[positionIndex].mValue;
    const aiVector3D& end = pNodeAnim->mPositionKeys[nextPositionIndex].mValue;
    aiVector3D delta = end - start;
    out = start + factor * delta;
}


void Model::calcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* pNodeAnim) {
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint RotationIndex = findRotation(animationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (animationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
    out = out.Normalize();
}


void Model::calcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim) {
    if (pNodeAnim->mNumScalingKeys == 1) {
        out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint ScalingIndex = findScaling(animationTime, pNodeAnim);
    uint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (animationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    out = Start + Factor * Delta;
}


void Model::readNodeHierarchy(float animationTime, const aiNode* pNode, const glm::mat4& ParentTransform) {
    std::string NodeName(pNode->mName.data);

    const aiAnimation* pAnimation = m_pScene->mAnimations[0];

    //glm::mat4 NodeTransformation(pNode->mTransformation);
    glm::mat4 NodeTransformation = aiToGlm(pNode->mTransformation);

    const aiNodeAnim* pNodeAnim = findNodeAnim(pAnimation, NodeName);

    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        calcInterpolatedScaling(Scaling, animationTime, pNodeAnim);
        glm::mat4 ScalingM;
        ScalingM = glm::scale(ScalingM, glm::vec3(Scaling.x, Scaling.y, Scaling.z));

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        calcInterpolatedRotation(RotationQ, animationTime, pNodeAnim);
        glm::mat4 RotationM = glm::mat4_cast(aiToGlm(RotationQ));

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        calcInterpolatedPosition(Translation, animationTime, pNodeAnim);
        glm::mat4 TranslationM;
        TranslationM = glm::translate(TranslationM, glm::vec3(Translation.x, Translation.y, Translation.z));

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    glm::mat4 globalTransformation = ParentTransform * NodeTransformation;

    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].finalTransformation = m_GlobalInverseTransform * globalTransformation * m_BoneInfo[BoneIndex].boneOffset;
    }

    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        readNodeHierarchy(animationTime, pNode->mChildren[i], globalTransformation);
    }
}

void Model::boneTransform(float timeInSeconds, std::vector<glm::mat4>& transforms) {
    glm::mat4 identity;

    float ticksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float timeInTicks = timeInSeconds * ticksPerSecond;
    float animationTime = fmod(timeInTicks, (float)m_pScene->mAnimations[0]->mDuration);

    readNodeHierarchy(animationTime, m_pScene->mRootNode, identity);

    transforms.resize(numBones);

    for (uint i = 0 ; i < numBones ; i++) {
        transforms[i] = m_BoneInfo[i].finalTransformation;
    }
}


const aiNodeAnim* Model::findNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
    for (uint i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return NULL;
}
