
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/VertexArrayFactory.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>

#include <GTCore/Path.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// assimp helpers
namespace GTEngine
{
    glm::mat4 AssimpToGLM(const aiMatrix4x4 &value)
    {
        // assimp matrix = row-major. GLM matrix = column-major. sigh.
        glm::mat4 result;
        result[0] = glm::vec4(value[0][0], value[1][0], value[2][0], value[3][0]);
        result[1] = glm::vec4(value[0][1], value[1][1], value[2][1], value[3][1]);
        result[2] = glm::vec4(value[0][2], value[1][2], value[2][2], value[3][2]);
        result[3] = glm::vec4(value[0][3], value[1][3], value[2][3], value[3][3]);

        return result;
    }

    glm::vec3 AssimpToGLM(const aiVector3D &value)
    {
        return glm::vec3(value.x, value.y, value.z);
    }

    glm::quat AssimpToGLM(const aiQuaternion &value)
    {
        return glm::quat(value.w, value.x, value.y, value.z);
    }



    void SetBoneData(BoneWithWeights &outputBone, const aiBone &inputBone)
    {
        aiVector3D   scale(1.0f, 1.0f, 1.0f);
        aiQuaternion rotation;
        aiVector3D   position;
        inputBone.mOffsetMatrix.Decompose(scale, rotation, position);

        outputBone.SetOffsetMatrix(
            glm::translate(position.x, position.y, position.z) *
            glm::mat4_cast(AssimpToGLM(rotation) /** glm::angleAxis(90.0f, glm::vec3(1.0f, 0.0f, 0.0f))*/) *        // <-- TODO: Check if this rotation is a bug in Assimp or Blender.
            glm::scale(scale.x, scale.y, scale.z));


        for (unsigned int i = 0; i < inputBone.mNumWeights; ++i)
        {
            outputBone.AddWeight(inputBone.mWeights[i].mVertexId, inputBone.mWeights[i].mWeight);
        }
    }

    BoneWithWeights* CreateEmptyBone(const aiNode &inputNode)
    {
        auto newBone = new BoneWithWeights;
        newBone->SetName(inputNode.mName.C_Str());
        
        aiVector3D   scale(1.0f, 1.0f, 1.0f);
        aiQuaternion rotation;
        aiVector3D   position;
        inputNode.mTransformation.Decompose(scale, rotation, position);

        newBone->SetPosition(AssimpToGLM(position));
        newBone->SetRotation(AssimpToGLM(rotation));
        newBone->SetScale(AssimpToGLM(scale));


        return newBone;
    }

    BoneWithWeights* CreateBone(const aiNode &inputNode, const aiBone &inputBone)
    {
        auto newBone = CreateEmptyBone(inputNode);
        assert(newBone != nullptr);

        SetBoneData(*newBone, inputBone);

        return newBone;
    }

    

    
    const aiNode* FindNodeByName(const aiScene &scene, const aiNode &node, const aiString &name)
    {
        if (node.mName == name)
        {
            return &node;
        }

        // We'll get here if the node was not found. We need to check the children.
        for (unsigned int iChild = 0; iChild < node.mNumChildren; ++iChild)
        {
            auto child = node.mChildren[iChild];
            assert(child != nullptr);

            auto result = FindNodeByName(scene, *child, name);
            if (result != nullptr)
            {
                // We'll get here if the child found the node. In this case, we just return that node.
                return result;
            }
        }

        // If we get here, not even the children could find it.
        return nullptr;
    }

    const aiNode* FindNodeByName(const aiScene &scene, const aiString &name)
    {
        assert(scene.mRootNode != nullptr);

        return FindNodeByName(scene, *scene.mRootNode, name);
    }
}


// Globals.
namespace GTEngine
{
    /// Structure containing information about a loaded model.
    struct LoadedModelInfo
    {
        // TODO: rename geomtries to meshGeometries and defaultMaterials to meshMaterials. Bit more descriptive that way.

        /// Constructor.
        LoadedModelInfo()
            : geometries(), defaultMaterials(), meshBones(),
              bones(),
              animations(),
              haveCreatedModel(false)
        {
        }

        /// Destructor.
        ~LoadedModelInfo()
        {
            for (size_t i = 0; i < this->geometries.count; ++i)
            {
                delete this->geometries[i];
            }

            for (size_t i = 0; i < this->defaultMaterials.count; ++i)
            {
                MaterialLibrary::Delete(this->defaultMaterials[i]);
            }

            for (size_t i = 0; i < this->meshBones.count; ++i)
            {
                auto meshBone = this->meshBones[i];
                if (meshBone != nullptr)
                {
                    for (size_t j = 0; j < meshBone->count; ++j)
                    {
                        delete meshBone->buffer[j]->value;
                    }
                }

                delete this->meshBones[i];
            }

            for (size_t i = 0; i < this->bones.count; ++i)
            {
                delete this->bones.buffer[i]->value;
            }

            for (size_t i = 0; i < this->animations.count; ++i)
            {
                delete this->animations.buffer[i]->value;
            }
        }


        /// Adds a bone, including it's ancestors.
        BoneWithWeights* AddBone(const aiScene &scene, const aiBone &bone)
        {
            auto node = FindNodeByName(scene, bone.mName);
            assert(node != nullptr);

            auto iExistingBone = this->bones.Find(bone.mName.C_Str());
            if (iExistingBone == nullptr)
            {
                // We now have enough information to create a GTEngine bone object.
                auto newBone = CreateBone(*node, bone);
                assert(newBone != nullptr);

                this->bones.Add(bone.mName.C_Str(), newBone);

                // Now we need to iterate over the ancestores and make sure we have bones for them.
                if (node->mParent != nullptr)
                {
                    auto parent = this->AddBone(*node->mParent);
                    if (parent != nullptr)
                    {
                        parent->AttachChild(*newBone);
                    }
                }

                return newBone;
            }
            else
            {
                // If it already exists, we need to ensure we have data.
                auto newBone = iExistingBone->value;
                assert(newBone != nullptr);

                if (newBone->IsEmpty())
                {
                    // Here is where we fill the bone with data.
                    SetBoneData(*newBone, bone);
                }

                return newBone;
            }
        }

        // Adds an empty bone based only on a node. This will also add ancestors. If a bone of the same name already exists, this function will do nothing.
        BoneWithWeights* AddBone(const aiNode &node)
        {
            auto iExistingBone = this->bones.Find(node.mName.C_Str());
            if (iExistingBone == nullptr)
            {
                auto newBone = CreateEmptyBone(node);
                assert(newBone != nullptr);

                this->bones.Add(node.mName.C_Str(), newBone);

                // Now we need to do ancestors.
                if (node.mParent != nullptr)
                {
                    auto parent = this->AddBone(*node.mParent);
                    if (parent != nullptr)
                    {
                        parent->AttachChild(*newBone);
                    }
                }

                return newBone;
            }
            else
            {
                return iExistingBone->value;
            }
        }


        /// Creates a Model object from this info.
        Model* CreateModel()
        {
            auto model = new Model;

            // We need to create copies of the bones. It is important that this is done before adding the meshes.
            model->CopyAndAttachBones(this->bones);

            // Now we'll copy over the animations. This must come after adding the bones.
            model->CopyAndAddAnimations(this->animations);

            // Now we need to create the meshes. This must be done after adding the bones.
            for (size_t i = 0; i < this->geometries.count; ++i)
            {
                if (this->meshBones[i] != nullptr)
                {
                    model->AttachMesh(this->geometries[i], this->defaultMaterials[i], *this->meshBones[i]);
                }
                else
                {
                    model->AttachMesh(this->geometries[i], this->defaultMaterials[i]);
                }
            }

            


            // If this is the first model we've created we need to generate tangents and bitangents for the meshes.
            if (!this->haveCreatedModel)
            {
                model->GenerateTangentsAndBitangents();

                this->haveCreatedModel = true;
            }

            return model;
        }


        /// The list of vertex arrays containing the geometric data of each mesh.
        GTCore::Vector<VertexArray*> geometries;

        /// The default materials. There will always be an equal number of materials as there are meshes. Each material in this list
        /// has a one-to-one correspondance with a mesh in <meshes>. For example, the material at index 0 is the material to use with
        /// the mesh at index 0.
        GTCore::Vector<Material*> defaultMaterials;

        /// The list of bone lists for each mesh. If the mesh does not use any bones, the entry will be set to null.
        //GTCore::Vector<GTCore::Vector<BoneWithWeights*>*> meshBones;

        /// The list of bone weights for each mesh. If the mesh does not use any bones, the entry will be set to null.
        GTCore::Vector<GTCore::Dictionary<BoneWeights*>*> meshBones;


        /// A map of every bone of the model, indexed by it's name. We use a map here to make it easier for avoiding duplication and
        /// also fast lookups.
        GTCore::Dictionary<BoneWithWeights*> bones;

        /// A map of animations.
        GTCore::Dictionary<SkeletalAnimation*> animations;


        /// This keeps track of whether or not we have previously created a model from this info.
        bool haveCreatedModel;
    };

    /// The list of loaded models.
    static GTCore::Dictionary<LoadedModelInfo*> LoadedModels;
}

// Startup/Shutdown
namespace GTEngine
{
    bool ModelLibrary::Startup()
    {
        return true;
    }

    void ModelLibrary::Shutdown()
    {
        // We need to unload the loaded model information.
        for (size_t i = 0; i < LoadedModels.count; ++i)
        {
            delete LoadedModels.buffer[i]->value;
        }
    }
}

// Loading
namespace GTEngine
{
    void CopyNodesWithMeshes(const aiScene &scene, const aiNode &node, const aiMatrix4x4 &accumulatedTransform, LoadedModelInfo &model)
    {
        // First we need to grab the transformation to apply to the mesh.
        aiMatrix4x4 transform = accumulatedTransform * node.mTransformation;
        aiMatrix3x3 normalTransform(transform);
        normalTransform.Transpose();
        normalTransform.Inverse();

        // Here we need to loop over each mesh and add it to the model. The mesh needs to be transformed based on the transformation.
        for (unsigned int iMesh = 0; iMesh < node.mNumMeshes; ++iMesh)
        {
            auto mesh = scene.mMeshes[node.mMeshes[iMesh]];
            assert(mesh != nullptr);

            auto positions = mesh->mVertices;
            auto normals   = mesh->mNormals;
            auto texCoords = mesh->mTextureCoords;

            // For now, only support triangle formats.
            if (mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE)
            {
                auto format = VertexFormat::P3T2N3T3B3;

                auto va = new VertexArray(VertexArrayUsage_Static, format);
                va->SetData(nullptr, mesh->mNumVertices, nullptr, mesh->mNumFaces * 3);

                auto vertexData = va->MapVertexData();
                auto indexData  = va->MapIndexData();

                auto vertexSize = format.GetSize();
                auto positionOffset = format.GetAttributeOffset(VertexAttribs::Position);
                auto texCoordOffset = format.GetAttributeOffset(VertexAttribs::TexCoord);
                auto normalOffset   = format.GetAttributeOffset(VertexAttribs::Normal);

                // Here we will copy over the vertex data. We will generate tangents and bitangents afterwards.
                for (unsigned int iVertex = 0; iVertex < mesh->mNumVertices; ++iVertex)
                {
                    auto vertexDst = vertexData + (iVertex * vertexSize);

                    auto positionDst = vertexDst + positionOffset;
                    auto texCoordDst = vertexDst + texCoordOffset;
                    auto normalDst   = vertexDst + normalOffset;

                    // First the position.
                    if (positions != nullptr)
                    {
                        aiVector3D position = transform * positions[iVertex];

                        positionDst[0] = position.x;
                        positionDst[1] = position.y;
                        positionDst[2] = position.z;
                    }

                    // Now the texture coordinates. We use the first set here.
                    if (texCoords != nullptr)
                    {
                        if (texCoords[0] != nullptr)
                        {
                            aiVector3D texCoord = texCoords[0][iVertex];

                            texCoordDst[0] = texCoord.x;
                            texCoordDst[1] = texCoord.y;
                        }
                    }

                    // Now the normals.
                    if (normals != nullptr)
                    {
                        aiVector3D normal = normalTransform * normals[iVertex];

                        normalDst[0] = normal.x;
                        normalDst[1] = normal.y;
                        normalDst[2] = normal.z;
                    }
                }

                // Now we'll copy over the indices.
                for (unsigned int iFace = 0; iFace < mesh->mNumFaces; ++iFace)
                {
                    auto &face = mesh->mFaces[iFace];
                    assert(face.mNumIndices == 3);

                    auto indexDst = indexData + (iFace * 3);
                    
                    indexDst[0] = face.mIndices[0];
                    indexDst[1] = face.mIndices[1];
                    indexDst[2] = face.mIndices[2];
                }

                va->UnmapVertexData();
                va->UnmapIndexData();

                model.geometries.PushBack(va);
                model.defaultMaterials.PushBack(nullptr);
            }


            // Here is where we create all of the bones for the mesh.
            if (mesh->mNumBones > 0)
            {
                //auto localBones = new GTCore::Vector<BoneWithWeights*>;
                auto localBones = new GTCore::Dictionary<BoneWeights*>;

                for (unsigned int iBone = 0; iBone < mesh->mNumBones; ++iBone)
                {
                    auto bone = mesh->mBones[iBone];
                    assert(bone != nullptr);

                    auto newBone = model.AddBone(scene, *bone);
                    if (newBone != nullptr)
                    {
                        // Here we create the VertexWeights object for this bone.
                        auto weights = new BoneWeights(bone->mName.C_Str());
                        weights->weights.Reserve(bone->mNumWeights);

                        for (unsigned int iWeight = 0; iWeight < bone->mNumWeights; ++iWeight)
                        {
                            weights->weights.PushBack(VertexWeightPair(bone->mWeights[iWeight].mVertexId, bone->mWeights[iWeight].mWeight));
                        }

                        localBones->Add(newBone->GetName(), weights);
                        //localBones->PushBack(newBone);
                    }
                }

                
                // Now we need to create our bone weights

                model.meshBones.PushBack(localBones);
            }
            else
            {
                model.meshBones.PushBack(nullptr);
            }
        }

        // Now we need to iterate over the children and copy them.
        for (unsigned int iChild = 0; iChild < node.mNumChildren; ++iChild)
        {
            auto child = node.mChildren[iChild];
            assert(child != nullptr);

            CopyNodesWithMeshes(scene, *child, transform, model);
        }
    }

    Model* ModelLibrary::LoadFromFile(const char* fileName)
    {
        LoadedModelInfo* modelInfo = nullptr;
        bool firstLoad = false;

        // If the file is already loaded, we don't want to reload. Instead we create a new instance of the model using the existing information.
        auto iModelInfo = LoadedModels.Find(fileName);
        if (iModelInfo == nullptr)
        {
            Assimp::Importer importer;
            importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_COLORS | aiComponent_LIGHTS | aiComponent_CAMERAS);

            auto scene = importer.ReadFile(fileName,
                aiProcess_Triangulate           |
                aiProcess_JoinIdenticalVertices |
                aiProcess_SortByPType           |
                aiProcess_ImproveCacheLocality  |
                aiProcess_GenSmoothNormals      |
                aiProcess_RemoveComponent);
            if (scene != nullptr)
            {
                // We need to recursively read each scene node and attach the meshes where possible.
                auto root = scene->mRootNode;
                if (root != nullptr)
                {
                    modelInfo = new LoadedModelInfo;

                    // If we get here, it means this is the first time this model is being loaded. In this situation we need to do some first-load operations.
                    firstLoad = true;

                    // We're going to use a different transformation depending on whether or not we are importing a blender file. This is a bit of a hacky, so this may
                    // be moved layer on.
                    aiMatrix4x4 transform;

                    // With blender files we need to rotate on the X axis to get everything right.
                    if (GTCore::Path::ExtensionEqual(fileName, "blend"))
                    {
                        transform.RotationX(glm::radians(-90.0f), transform);
                    }

                    // This is where we take the assimp meshes and create the GTEngine meshes.
                    CopyNodesWithMeshes(*scene, *root, transform, *modelInfo);


                    // Here we will grab the animations.
                    for (unsigned int iAnimation = 0; iAnimation < scene->mNumAnimations; ++iAnimation)
                    {
                        auto animation = scene->mAnimations[iAnimation];
                        assert(animation != nullptr);

                        auto newAnimation = new SkeletalAnimation(animation->mName.C_Str());

                        // Duration.
                        if (animation->mTicksPerSecond > 0)
                        {
                            newAnimation->SetDurationInSeconds(animation->mDuration / animation->mTicksPerSecond);
                        }
                        else
                        {
                            newAnimation->SetDurationInSeconds(animation->mDuration);
                        }

                        // Channels.
                        for (unsigned int iChannel = 0; iChannel < animation->mNumChannels; ++iChannel)
                        {
                            auto channel = animation->mChannels[iChannel];
                            assert(channel != nullptr);

                            // We need to retrieve the bone that this channel is modifying. There is a chance this bone is not part of the model's main bone list yet, in
                            // which case we need to add it. We include this instead of ignoring because the application may need the bone information, even though it's
                            // not affecting the mesh itself.
                            BoneWithWeights* bone = nullptr;

                            auto iBone = modelInfo->bones.Find(channel->mNodeName.C_Str());
                            if (iBone == nullptr)
                            {
                                auto newNode = FindNodeByName(*scene, channel->mNodeName);
                                assert(newNode != nullptr);

                                bone = modelInfo->AddBone(*newNode);
                            }
                            else
                            {
                                bone = iBone->value;
                            }

                            assert(bone != nullptr);

                            auto newChannel = newAnimation->AddChannel(*bone);
                            assert(newChannel != nullptr);


                            // TODO: Check that this assertion is valid. If not, we need to combine them all into a single list.
                            assert(channel->mNumPositionKeys == channel->mNumRotationKeys && channel->mNumPositionKeys == channel->mNumScalingKeys);

                            unsigned int keyCount = channel->mNumPositionKeys;
                            for (unsigned int iKey = 0; iKey < keyCount; ++iKey)
                            {
                                auto &positionKey = channel->mPositionKeys[iKey];
                                auto &rotationKey = channel->mRotationKeys[iKey];
                                auto &scaleKey    = channel->mScalingKeys[iKey];

                                newChannel->AddKey(positionKey.mTime, AssimpToGLM(positionKey.mValue), AssimpToGLM(rotationKey.mValue), AssimpToGLM(scaleKey.mValue));
                            }
                        }

                        // Can't forget to add the new animation to the model's info structure.
                        modelInfo->animations.Add(newAnimation->GetName(), newAnimation);
                    }


                    // When we get here we will have a LoadedModelInfo object which needs to be added to the global list.
                    LoadedModels.Add(fileName, modelInfo);
                }
                else
                {
                    GTEngine::PostError("Error importing %s: Root node not found.", fileName);
                }
            }
            else
            {
                GTEngine::PostError("Error importing %s: %s", fileName, importer.GetErrorString());
            }
        }
        else
        {
            modelInfo = iModelInfo->value;
        }

        
        // Now that we have information about the model, we can create a new Model object and return it.
        if (modelInfo != nullptr)
        {
            // We create a model instantiation from the model info.
            return modelInfo->CreateModel();
        }


        return nullptr;
    }


    Model* ModelLibrary::CreatePlaneXZ(float width, float height, VertexFormat &format)
    {
        auto model = new Model;

        model->AttachMesh(VertexArrayFactory::CreatePlaneXZ(width, height, format), nullptr);

        return model;
    }


    void ModelLibrary::Delete(Model* model)
    {
        delete model;
    }
}
