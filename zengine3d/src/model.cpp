#include "model.h"

#include <SOIL.h>

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

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
                | aiProcess_GenSmoothNormals
                | aiProcess_FlipUVs
                | aiProcess_JoinIdenticalVertices
                | aiProcess_FixInfacingNormals
                );
        return [&]() {
            if (m_pScene) {
                this->directory = fileName.substr(0, fileName.find_last_of('/'));
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
        aiString mn;
        if (AI_SUCCESS != material->Get(AI_MATKEY_NAME, mn)) {
            // throw std::runtime_error("could not get property: AI_MATKEY_NAME from material");
            mn = "default name";
        }
        const std::string materialName(mn.data);
        //std::cout << "materialName: " << materialName << std::endl;

        aiColor3D cd;
        if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, cd))
            throw std::runtime_error("could not get property: AI_MATKEY_COLOR_DIFFUSE from material" + materialName);
        const glm::vec3 materialDiffuse(glm::vec3(cd.r, cd.g, cd.b));
        //std::cout << "materialDiffuse.x: " << materialDiffuse.x << std::endl;
        //std::cout << "materialDiffuse.y: " << materialDiffuse.y << std::endl;
        //std::cout << "materialDiffuse.z: " << materialDiffuse.z << std::endl;
        meshes[i].diffuseColor = materialDiffuse;

        aiColor3D cs;
        if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_SPECULAR, cs))
            throw std::runtime_error("could not get property: AI_MATKEY_COLOR_SPECULAR from material" + materialName);
        const glm::vec3 materialSpecular(glm::vec3(cs.r, cs.g, cs.b));
        //std::cout << "materialSpecular.x: " << materialSpecular.x << std::endl;
        //std::cout << "materialSpecular.y: " << materialSpecular.y << std::endl;
        //std::cout << "materialSpecular.z: " << materialSpecular.z << std::endl;
        meshes[i].specularColor = materialSpecular;
    }

    // Reserve space in the vectors for the vertex attributes and indices
    positions.reserve(numVertices);
    normals.reserve(numVertices);
    texCoords.reserve(numVertices);
    bones.resize(numVertices);
    indices.reserve(numIndices);

    // Initialize the meshes in the scene one by one
    for (uint i = 0 ; i < meshes.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        initMesh(i, paiMesh, positions, normals, texCoords, bones, indices);
    }
    assert(glGetError() == GL_NO_ERROR);

    if (!initMaterials(pScene, fileName)) {
        return false;
    }

    assert(glGetError() == GL_NO_ERROR);
    // Generate and populate the buffers with vertex attributes and the indices
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(positions[0]) * positions.size(),
                 &positions[0],
                 GL_STATIC_DRAW);
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
                     std::vector<glm::vec2>& texCoords,
                     std::vector<VertexBoneData>& bones,
                     std::vector<uint>& indices) {
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    // Populate the vertex attribute vectors
    for (uint i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        //std::cout << "pNormal->x: " << pNormal->x << std::endl;

        positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
        texCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
    }

    loadBones(meshIndex, paiMesh, bones);

    // Populate the index buffer
    for (uint i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
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

bool Model::initMaterials(const aiScene* pScene, const std::string& fileName) {
    // Initialize the materials
    assert(glGetError() == GL_NO_ERROR);
    for (uint i = 0 ; i < pScene->mNumMaterials ; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        aiTextureType type = aiTextureType_DIFFUSE;
        unsigned int textureCount = pMaterial->GetTextureCount(type);
        for (unsigned int j = 0; j < textureCount; j++) {
            aiString texPath;
            if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
            std::cout << "texturePath: " << texPath.data << std::endl;
          
            if (m_Textures.find(texPath.data) == m_Textures.end()) {
                Texture texture;
                texture.id = TextureFromFile(texPath.data, this->directory);
                texture.type = "texture_diffuse";
                texture.path = texPath;
                m_Textures[texPath.data] = texture;
                std::cout << "add new diffuse texture: " << texture.path.data << std::endl;
            }
        }

        type = aiTextureType_SPECULAR;
        textureCount = pMaterial->GetTextureCount(type);
        for (unsigned int j = 0; j < textureCount; j++) {
            aiString texPath;
            if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
            std::cout << "texturePath: " << texPath.data << std::endl;
          
            if (m_Textures.find(texPath.data) == m_Textures.end()) {
                Texture texture;
                texture.id = TextureFromFile(texPath.data, this->directory);
                texture.type = "texture_specular";
                texture.path = texPath;
                m_Textures[texPath.data] = texture;
                std::cout << "add new diffuse specular: " << texture.path.data << std::endl;
            }
        }

        type = aiTextureType_HEIGHT;
        textureCount = pMaterial->GetTextureCount(type);
        for (unsigned int j = 0; j < textureCount; j++) {
            aiString texPath;
            if (AI_SUCCESS != pMaterial->Get(AI_MATKEY_TEXTURE(type, j), texPath))
                throw std::runtime_error("could not get property: AI_MATKEY_TEXTURE(type,j) from material");
            std::cout << "texturePath: " << texPath.data << std::endl;
          
            if (m_Textures.find(texPath.data) == m_Textures.end()) {
                Texture texture;
                texture.id = TextureFromFile(texPath.data, this->directory);
                texture.type = "texture_height";
                texture.path = texPath;
                m_Textures[texPath.data] = texture;
                std::cout << "add new height texture: " << texture.path.data << std::endl;
            }
        }

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
    }
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    assert(glGetError() == GL_NO_ERROR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
        assert(glGetError() == GL_NO_ERROR);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, defaultTexture);

        assert(glGetError() == GL_NO_ERROR);
        MeshEntry entry(meshes[i]);
        //for (int j = 0; j < entry.textures.size(); j++) {
        for (int j = 0; j < entry.textures.size() && j < 2; j++) {
            Texture texture(m_Textures[entry.textures[j]]);
            glActiveTexture(GL_TEXTURE0+j);
            std::stringstream ss;
            std::string name = texture.type;
            assert(glGetError() == GL_NO_ERROR);

            glUniform1i(glGetUniformLocation(shader.Program, ("material."+name).c_str()), j);
            glBindTexture(GL_TEXTURE_2D, texture.id);
        }
        assert(glGetError() == GL_NO_ERROR);
        glUniform3fv(glGetUniformLocation(shader.Program, "material.diffuseColor"), 1, glm::value_ptr(entry.diffuseColor));
        glUniform3fv(glGetUniformLocation(shader.Program, "material.specularColor"), 1, glm::value_ptr(entry.specularColor));
        assert(glGetError() == GL_NO_ERROR);

        glDrawElementsBaseVertex(GL_TRIANGLES,
                                 entry.numIndices,
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(uint) * entry.baseIndex),
                                 entry.baseVertex);
        assert(glGetError() == GL_NO_ERROR);

        for (int i = 0; i < entry.textures.size() && i < 2; i++) {
            glActiveTexture(GL_TEXTURE0+i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

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
