
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/VertexArrayFactory.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
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



    void SetBoneData(Bone &outputBone, const aiBone &inputBone)
    {
        aiVector3D   scale(1.0f, 1.0f, 1.0f);
        aiQuaternion rotation;
        aiVector3D   position;
        inputBone.mOffsetMatrix.Decompose(scale, rotation, position);

        outputBone.SetOffsetMatrix(
            glm::translate(position.x, position.y, position.z) *
            glm::mat4_cast(AssimpToGLM(rotation) * glm::angleAxis(90.0f, glm::vec3(1.0f, 0.0f, 0.0f))) *        // <-- TODO: Check if this rotation is a bug in Assimp or Blender.
            glm::scale(scale.x, scale.y, scale.z));
    }

    Bone* CreateEmptyBone(const aiNode &inputNode)
    {
        auto newBone = new Bone;
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

    Bone* CreateBone(const aiNode &inputNode, const aiBone &inputBone)
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
        /// Constructor.
        LoadedModelInfo()
            : meshGeometries(), meshMaterials(), meshBones(),
              bones(),
              animation(), animationChannelBones(), animationKeyCache(),
              haveCreatedModel(false)
        {
        }

        /// Destructor.
        ~LoadedModelInfo()
        {
            for (size_t i = 0; i < this->meshGeometries.count; ++i)
            {
                delete this->meshGeometries[i];
            }

            for (size_t i = 0; i < this->meshMaterials.count; ++i)
            {
                MaterialLibrary::Delete(this->meshMaterials[i]);
            }

            for (size_t i = 0; i < this->meshBones.count; ++i)
            {
                auto meshBone = this->meshBones[i];
                if (meshBone != nullptr)
                {
                    for (size_t j = 0; j < meshBone->count; ++j)
                    {
                        delete meshBone->buffer[j];
                    }
                }

                delete this->meshBones[i];
            }

            for (size_t i = 0; i < this->bones.count; ++i)
            {
                delete this->bones.buffer[i]->value;
            }

            for (size_t i = 0; i < this->animationKeyCache.count; ++i)
            {
                delete this->animationKeyCache[i];
            }
        }


        /// Adds a bone, including it's ancestors.
        Bone* AddBone(const aiScene &scene, const aiBone &bone)
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

                SetBoneData(*newBone, bone);

                return newBone;
            }
        }

        // Adds an empty bone based only on a node. This will also add ancestors. If a bone of the same name already exists, this function will do nothing.
        Bone* AddBone(const aiNode &node)
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

            // Now the animation.
            model->CopyAnimation(this->animation, this->animationChannelBones);


            // Now we need to create the meshes. This must be done after adding the bones.
            for (size_t i = 0; i < this->meshGeometries.count; ++i)
            {
                if (this->meshBones[i] != nullptr)
                {
                    model->AttachMesh(this->meshGeometries[i], this->meshMaterials[i], *this->meshBones[i]);
                }
                else
                {
                    model->AttachMesh(this->meshGeometries[i], this->meshMaterials[i]);
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


        /// Creates an animation key for the given bone and returns it.
        TransformAnimationKey* CreateAnimationKey(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
        {
            auto newKey = new TransformAnimationKey(position, rotation, scale);
            this->animationKeyCache.PushBack(newKey);

            return newKey;
        }

        /// Maps a bone to an animation channel.
        void MapBoneToAnimationChannel(AnimationChannel &channel, Bone* bone)
        {
            this->animationChannelBones.Add(&channel, bone);
        }



        /// The list of vertex arrays containing the geometric data of each mesh.
        GTCore::Vector<VertexArray*> meshGeometries;

        /// The default materials. There will always be an equal number of materials as there are meshes. Each material in this list
        /// has a one-to-one correspondance with a mesh in <meshes>. For example, the material at index 0 is the material to use with
        /// the mesh at index 0.
        GTCore::Vector<Material*> meshMaterials;

        /// The list of bone weights for each mesh. If the mesh does not use any bones, the entry will be set to null.
        GTCore::Vector<GTCore::Vector<BoneWeights*>*> meshBones;


        /// A map of every bone of the model, indexed by it's name. We use a map here to make it easier for avoiding duplication and
        /// also fast lookups.
        GTCore::Dictionary<Bone*> bones;



        /// The model's animation object.
        Animation animation;

        /// The map for mapping a bone to an animation channel. The key is the channel index.
        GTCore::Map<AnimationChannel*, Bone*> animationChannelBones;

        /// The cache of animation keys.
        GTCore::Vector<TransformAnimationKey*> animationKeyCache;


        /// This keeps track of whether or not we have previously created a model from this info.
        bool haveCreatedModel;
    };

    /// The list of loaded models.
    static GTCore::Dictionary<LoadedModelInfo*> LoadedModels;


    /// Creates a model from a primitive's vertex array.
    ///
    /// @param name [in] The name of the primitive.
    /// @param va   [in] The vertex array of the primitive. This can be nullptr, but only if the primitive has already been created.
    Model* ModelLibrary_CreateFromPrimitive(const char* name, VertexArray* va);

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
    /// The flags to use with assimp's ReadFile() and ReadFileFromMemory().
    static const unsigned int AssimpReadFileFlags =
        aiProcess_Triangulate           |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType           |
        aiProcess_ImproveCacheLocality  |
        aiProcess_GenSmoothNormals      |
        aiProcess_RemoveComponent;

    static const int AssimpRemovedComponentsFlags =
        aiComponent_COLORS |
        aiComponent_LIGHTS |
        aiComponent_CAMERAS;


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

                model.meshGeometries.PushBack(va);
                model.meshMaterials.PushBack(nullptr);
            }


            // Here is where we create all of the bones for the mesh.
            if (mesh->mNumBones > 0)
            {
                auto localBones = new GTCore::Vector<BoneWeights*>;

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

                        localBones->PushBack(weights);
                    }
                }

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

    /// This function creates a new LoadedModelInfo structure from the given assimp scene.
    ///
    /// @remarks
    ///     The returned structure is allocated with 'new'.
    LoadedModelInfo* ModelLibrary_LoadFromAssimpScene(const aiScene &scene)
    {
        // We need to recursively read each scene node and attach the meshes where possible.
        auto root = scene.mRootNode;
        if (root != nullptr)
        {
            auto modelInfo = new LoadedModelInfo;

            // This is where we take the assimp meshes and create the GTEngine meshes.
            aiMatrix4x4 transform;
            CopyNodesWithMeshes(scene, *root, transform, *modelInfo);


            // Here is where we load up the animations. Assimp has multiple animations, but GTEngine uses only a single animation. To
            // resolve, we simply copy over each animation into the main animation and create a named segment for that animation.
            double segmentStartTime = 0.0;
            
            for (auto iAnimation = 0U; iAnimation < scene.mNumAnimations; ++iAnimation)
            {
                auto animation = scene.mAnimations[iAnimation];
                assert(animation != nullptr);

                // The starting keyframe will be equal to the number of keyframes in the animation at this point.
                size_t startKeyFrame = modelInfo->animation.GetKeyFrameCount();

                // Now we need to loop through and add the actual key frames to the animation. This is done a little strange, but the Animation class
                // will make sure everything is clean. Basically, we loop through every channel and then add the keys for each channel. It's slow, but
                // it should work nicely.
                for (unsigned int iChannel = 0; iChannel < animation->mNumChannels; ++iChannel)
                {
                    auto channel = animation->mChannels[iChannel];
                    assert(channel != nullptr);

                    // We need to retrieve the bone that this channel is modifying. There is a chance this bone is not part of the model's main bone list yet, in
                    // which case we need to add it. We include this instead of ignoring because the application may need the bone information, even though it's
                    // not affecting the mesh itself.
                    Bone* bone = nullptr;

                    auto iBone = modelInfo->bones.Find(channel->mNodeName.C_Str());
                    if (iBone == nullptr)
                    {
                        auto newNode = FindNodeByName(scene, channel->mNodeName);
                        assert(newNode != nullptr);

                        bone = modelInfo->AddBone(*newNode);
                    }
                    else
                    {
                        bone = iBone->value;
                    }

                    assert(bone != nullptr);

                    // TODO: Check that this assertion is valid. If not, we need to combine them all into a single list.
                    assert(channel->mNumPositionKeys == channel->mNumRotationKeys && channel->mNumPositionKeys == channel->mNumScalingKeys);



                    // Now we create the channel.
                    auto &newChannel = modelInfo->animation.CreateChannel();
                    modelInfo->MapBoneToAnimationChannel(newChannel, bone);

                    // Here is where we add the key frames. Since we are looping over the channels, each key frame will probably be creating twice. This is OK because
                    // Animation will make sure there are no duplicate key frames.
                    for (unsigned int iKey = 0; iKey < channel->mNumPositionKeys; ++iKey)
                    {
                        auto &positionKey = channel->mPositionKeys[iKey];
                        auto &rotationKey = channel->mRotationKeys[iKey];
                        auto &scaleKey    = channel->mScalingKeys[iKey];

                        size_t keyFrameIndex = modelInfo->animation.AppendKeyFrame(segmentStartTime + positionKey.mTime);

                        auto key = modelInfo->CreateAnimationKey(AssimpToGLM(positionKey.mValue), AssimpToGLM(rotationKey.mValue), AssimpToGLM(scaleKey.mValue));
                        newChannel.SetKey(keyFrameIndex, key);
                    }
                }
                


                

                // At this point we can now create the named segment.
                modelInfo->animation.AddNamedSegment(animation->mName.C_Str(), startKeyFrame, modelInfo->animation.GetKeyFrameCount());

                // The start time of the next segment will be equal to the previous start time plus the duration of iAnimation.
                if (animation->mTicksPerSecond > 0)
                {
                    segmentStartTime += animation->mDuration / animation->mTicksPerSecond;
                }
                else
                {
                    segmentStartTime += animation->mDuration;
                }
            }

            return modelInfo;
        }

        return nullptr;
    }




    Model* ModelLibrary::LoadFromFile(const char* fileName)
    {
        LoadedModelInfo* modelInfo = nullptr;

        // If the file is already loaded, we don't want to reload. Instead we create a new instance of the model using the existing information.
        auto iModelInfo = LoadedModels.Find(fileName);
        if (iModelInfo == nullptr)
        {
            Assimp::Importer importer;
            importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, AssimpRemovedComponentsFlags);

            auto scene = importer.ReadFile(fileName, AssimpReadFileFlags);
            if (scene != nullptr)
            {
                modelInfo = ModelLibrary_LoadFromAssimpScene(*scene);
                if (modelInfo != nullptr)
                {
                    // When we get here we will have a LoadedModelInfo object which needs to be added to the global list.
                    LoadedModels.Add(fileName, modelInfo);
                }
                else
                {
                    GTEngine::PostError("Error creating model info for %s: %s", fileName, importer.GetErrorString());
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

    Model* ModelLibrary::LoadFromNFF(const char* content, const char* name)
    {
        GTCore::String nffFileName("__nff:");
        nffFileName.Append(name);

        LoadedModelInfo* modelInfo = nullptr;

        // If the file is already loaded, we don't want to reload. Instead we create a new instance of the model using the existing information.
        auto iModelInfo = LoadedModels.Find(nffFileName.c_str());
        if (iModelInfo == nullptr)
        {
            Assimp::Importer importer;
            importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, AssimpRemovedComponentsFlags);

            auto scene = importer.ReadFileFromMemory(content, GTCore::Strings::SizeInBytes(content), AssimpReadFileFlags, "nff");
            if (scene != nullptr)
            {
                modelInfo = ModelLibrary_LoadFromAssimpScene(*scene);
                if (modelInfo != nullptr)
                {
                    // When we get here we will have a LoadedModelInfo object which needs to be added to the global list.
                    LoadedModels.Add(nffFileName.c_str(), modelInfo);
                }
                else
                {
                    GTEngine::PostError("Error creating model info for %s", content);
                }
            }
            else
            {
                GTEngine::PostError("Error importing %s: %s", content, importer.GetErrorString());
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


    Model* ModelLibrary::CreateBox(float halfWidth, float halfHeight, float halfDepth)
    {
        // We need a unique identifier for this mesh. We will base it on the size of the box.
        char name[128];
        GTCore::IO::snprintf(name, 128, "prim:box(%.4f %.4f %.4f)", halfWidth, halfHeight, halfDepth);

        // We create the model from a primitive. To do this we need a non-const vertex array.
        VertexArray* va = nullptr;

        bool exists = LoadedModels.Find(name) != nullptr;
        if (!exists)
        {
            va = VertexArrayLibrary::CreateBox(halfWidth, halfHeight, halfDepth);
        }

        return ModelLibrary_CreateFromPrimitive(name, va);
    }

    Model* ModelLibrary::CreateSphere(float radius)
    {
        // We need to create the content of the NFF file.
        char content[128];
        GTCore::IO::snprintf(content, 128, "s 0 0 0 %.4f", radius);

        // We need a unique identifier for this mesh. We will base it on the size of the box.
        char name[128];
        GTCore::IO::snprintf(name, 128, "sphere(%.4f)", radius);

        // Now that we have the content and the name, we simply load the NFF file.
        return ModelLibrary::LoadFromNFF(content, name);
    }

    Model* ModelLibrary::CreateCylinder(float radius, float length)
    {
        float halfLength = length * 0.5f;

        // We need to create the content of the NFF file.
        char content[128];
        GTCore::IO::snprintf(content, 128, "c\n0 -%.4f 0 %.4f\n0 %.4f 0 %.4f", halfLength, radius, halfLength, radius);

        // We need a unique identifier for this mesh. We will base it on the size of the box.
        char name[128];
        GTCore::IO::snprintf(name, 128, "cyl(%.4f %.4f)", radius, length);

        // We're going to check if we can find the model info before loading. This will allow us to determine whether or not we
        // need to apply the transformation.
        bool applyTransform = LoadedModels.Find((GTCore::String("__nff:") + name).c_str()) == nullptr;

        // For now the way we will do this is load an NFF file. We use a radius of 1, and then scale that with a transformation.
        auto model = ModelLibrary::LoadFromNFF(content, name);
        if (model != nullptr && applyTransform)
        {
            //model->ApplyTransformation(glm::scale(halfWidth, halfHeight, halfDepth));
        }

        return model;
    }


    void ModelLibrary::Delete(Model* model)
    {
        delete model;
    }
}



// These are private functions implementations for ModelLibrary.
namespace GTEngine
{
    Model* ModelLibrary_CreateFromPrimitive(const char* name, VertexArray* va)
    {
        LoadedModelInfo* modelInfo = nullptr;

        // We first need to retrieve our model info.
        auto iModelInfo = LoadedModels.Find(name);
        if (iModelInfo == nullptr)
        {
            modelInfo = new LoadedModelInfo;
            modelInfo->meshGeometries.PushBack(va);
            modelInfo->meshMaterials.PushBack(nullptr);
            modelInfo->meshBones.PushBack(nullptr);

            LoadedModels.Add(name, modelInfo);
        }
        else
        {
            modelInfo = iModelInfo->value;
        }


        // Now that we have the model information we can create a model from it.
        if (modelInfo != nullptr)
        {
            return modelInfo->CreateModel();
        }

        return nullptr;
    }
}
