
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/VertexArrayFactory.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>

#include <GTCore/Path.hpp>
#include <GTCore/Timing.hpp>
#include <GTCore/List.hpp>
#include <GTCore/IO.hpp>

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


    // Adds an empty bone based only on a node to the given definition. This will also add ancestors. If a bone of the same name already exists, this function will do nothing.
    Bone* AddBone(const aiNode &node, ModelDefinition &definition)
    {
        auto iExistingBone = definition.bones.Find(node.mName.C_Str());
        if (iExistingBone == nullptr)
        {
            auto newBone = CreateEmptyBone(node);
            assert(newBone != nullptr);

            definition.bones.Add(node.mName.C_Str(), newBone);

            // Now we need to do ancestors.
            if (node.mParent != nullptr)
            {
                auto parent = AddBone(*node.mParent, definition);
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


    /// Adds a bone to the given definition, including it's ancestors.
    Bone* AddBone(const aiScene &scene, const aiBone &bone, ModelDefinition &definition)
    {
        auto node = FindNodeByName(scene, bone.mName);
        assert(node != nullptr);

        auto iExistingBone = definition.bones.Find(bone.mName.C_Str());
        if (iExistingBone == nullptr)
        {
            // We now have enough information to create a GTEngine bone object.
            auto newBone = CreateBone(*node, bone);
            assert(newBone != nullptr);

            definition.bones.Add(bone.mName.C_Str(), newBone);

            // Now we need to iterate over the ancestores and make sure we have bones for them.
            if (node->mParent != nullptr)
            {
                auto parent = AddBone(*node->mParent, definition);
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

    
}


// Globals.
namespace GTEngine
{
    /// The map of model definitions mapping a definition to a file name.
    static GTCore::Dictionary<ModelDefinition*> LoadedDefinitions;

    /// We need to keep track of the models that are using each definition. What we do here is keep a map with the key being a pointer
    /// to each loaded definition, and the value being a list of every loaded model that is using that definition.
    static GTCore::Map<ModelDefinition*, GTCore::Vector<Model*>*> LoadedModels;


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
        // All models and definitions need to be deleted.
        for (size_t iDefinition = 0; iDefinition < LoadedModels.count; ++iDefinition)
        {
            auto definition = LoadedModels.buffer[iDefinition]->key;
            auto models     = LoadedModels.buffer[iDefinition]->value;

            if (models != nullptr)
            {
                for (size_t iModel = 0; iModel < models->count; ++iModel)
                {
                    delete models->buffer[iModel];
                }
            }

            delete models;
            delete definition;
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


    void CopyNodesWithMeshes(const aiScene &scene, const aiNode &node, const aiMatrix4x4 &accumulatedTransform, ModelDefinition &definition, GTCore::Vector<GTCore::Vector<BoneWeights*>*> &meshBones)
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

                definition.meshGeometries.PushBack(va);
                definition.meshMaterials.PushBack(MaterialLibrary::Create("engine/materials/simple-diffuse.material"));
            }


            // Here is where we create all of the bones for the mesh.
            if (mesh->mNumBones > 0)
            {
                auto localBones = new GTCore::Vector<BoneWeights*>;

                for (unsigned int iBone = 0; iBone < mesh->mNumBones; ++iBone)
                {
                    auto bone = mesh->mBones[iBone];
                    assert(bone != nullptr);

                    auto newBone = AddBone(scene, *bone, definition);
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

                meshBones.PushBack(localBones);
            }
            else
            {
                meshBones.PushBack(nullptr);
            }
        }

        // Now we need to iterate over the children and copy them.
        for (unsigned int iChild = 0; iChild < node.mNumChildren; ++iChild)
        {
            auto child = node.mChildren[iChild];
            assert(child != nullptr);

            CopyNodesWithMeshes(scene, *child, transform, definition, meshBones);
        }


        // With the meshes created, we can now generate the tangents+bitangents.
        definition.GenerateTangentsAndBitangents();
    }

    /// This function creates a new model definition from the given assimp scene.
    bool ModelLibrary_LoadFromAssimpScene(const aiScene &scene, ModelDefinition &definition)
    {
        // We need to recursively read each scene node and attach the meshes where possible.
        auto root = scene.mRootNode;
        if (root != nullptr)
        {
            GTCore::Vector<GTCore::Vector<BoneWeights*>*> meshBones;

            // This is where we take the assimp meshes and create the GTEngine meshes.
            aiMatrix4x4 transform;
            CopyNodesWithMeshes(scene, *root, transform, definition, meshBones);


            // Now what we do is iterate over the bones of each mesh and create the skinning vertex attributes. It's important that we do this after creating the local bones
            // of the mesh so that we get the correct indices.
            for (size_t i = 0; i < definition.meshGeometries.count; ++i)
            {
                auto localBones = meshBones[i];
                if (localBones != nullptr)
                {
                    auto skinningVertexAttributes = new SkinningVertexAttribute[definition.meshGeometries[i]->GetVertexCount()];

                    for (size_t j = 0; j < localBones->count; ++j)
                    {
                        auto bone = localBones->buffer[j];
                        assert(bone != nullptr);

                        size_t boneIndex = definition.bones.Find(bone->name.c_str())->index;

                        for (size_t k = 0; k < bone->weights.count; ++k)
                        {
                            skinningVertexAttributes[bone->weights[k].vertexID].AddBoneWeightPair(boneIndex, bone->weights[k].weight);
                        }
                    }

                    definition.meshSkinningVertexAttributes.PushBack(skinningVertexAttributes);
                }
                else
                {
                    definition.meshSkinningVertexAttributes.PushBack(nullptr);
                }
            }



            // Here is where we load up the animations. Assimp has multiple animations, but GTEngine uses only a single animation. To
            // resolve, we simply copy over each animation into the main animation and create a named segment for that animation.
            double segmentStartTime = 0.0;
            
            for (auto iAnimation = 0U; iAnimation < scene.mNumAnimations; ++iAnimation)
            {
                auto animation = scene.mAnimations[iAnimation];
                assert(animation != nullptr);

                // The starting keyframe will be equal to the number of keyframes in the animation at this point.
                size_t startKeyFrame = definition.animation.GetKeyFrameCount();

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

                    auto iBone = definition.bones.Find(channel->mNodeName.C_Str());
                    if (iBone == nullptr)
                    {
                        auto newNode = FindNodeByName(scene, channel->mNodeName);
                        assert(newNode != nullptr);

                        bone = AddBone(*newNode, definition);
                    }
                    else
                    {
                        bone = iBone->value;
                    }

                    assert(bone != nullptr);

                    // TODO: Check that this assertion is valid. If not, we need to combine them all into a single list.
                    assert(channel->mNumPositionKeys == channel->mNumRotationKeys && channel->mNumPositionKeys == channel->mNumScalingKeys);



                    // Now we create the channel.
                    auto &newChannel = definition.animation.CreateChannel();
                    definition.MapAnimationChannelToBone(*bone, newChannel);

                    // Here is where we add the key frames. Since we are looping over the channels, each key frame will probably be creating twice. This is OK because
                    // Animation will make sure there are no duplicate key frames.
                    for (unsigned int iKey = 0; iKey < channel->mNumPositionKeys; ++iKey)
                    {
                        auto &positionKey = channel->mPositionKeys[iKey];
                        auto &rotationKey = channel->mRotationKeys[iKey];
                        auto &scaleKey    = channel->mScalingKeys[iKey];

                        size_t keyFrameIndex = definition.animation.AppendKeyFrame(segmentStartTime + positionKey.mTime);

                        auto key = definition.CreateAnimationKey(AssimpToGLM(positionKey.mValue), AssimpToGLM(rotationKey.mValue), AssimpToGLM(scaleKey.mValue));
                        newChannel.SetKey(keyFrameIndex, key);
                    }
                }
                


                // At this point we can now create the named segment.
                definition.animation.AddNamedSegment(animation->mName.C_Str(), startKeyFrame, definition.animation.GetKeyFrameCount());

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

            return true;
        }

        return nullptr;
    }




    Model* ModelLibrary::LoadFromFile(const char* fileName)
    {
        // We will first find an existing model definition. If we don't find it, we create one and the load into it.
        auto definition = ModelLibrary::FindDefinition(fileName);
        if (definition == nullptr)
        {
            GTCore::String absolutePath;
            if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
            {
                definition = new ModelDefinition(fileName);
                
                // We need to load the model.
                if (ModelLibrary::Load(fileName, *definition))
                {
                    LoadedDefinitions.Add(absolutePath.c_str(), definition);
                    LoadedModels.Add(definition, new GTCore::Vector<Model*>);
                }
                else
                {
                    delete definition;
                    definition = nullptr;
                }
            }
        }

        // Now all we do is 
        if (definition != nullptr)
        {
            return ModelLibrary::CreateFromDefinition(*definition);
        }
        else
        {
            return nullptr;
        }
    }


    Model* ModelLibrary::LoadFromNFF(const char* content, const char* name)
    {
        GTCore::String nffFileName("__nff:");
        nffFileName.Append(name);

        ModelDefinition* definition = nullptr;

        // If the file is already loaded, we don't want to reload. Instead we create a new instance of the model using the existing information.
        auto iDefinition = LoadedDefinitions.Find(nffFileName.c_str());
        if (iDefinition == nullptr)
        {
            Assimp::Importer importer;
            importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, AssimpRemovedComponentsFlags);

            auto scene = importer.ReadFileFromMemory(content, GTCore::Strings::SizeInBytes(content), AssimpReadFileFlags, "nff");
            if (scene != nullptr)
            {
                definition = new ModelDefinition;
                if (ModelLibrary_LoadFromAssimpScene(*scene, *definition))
                {
                    // When we get here we will have a LoadedModelInfo object which needs to be added to the global list.
                    LoadedDefinitions.Add(nffFileName.c_str(), definition);
                }
                else
                {
                    delete definition;
                    definition = nullptr;

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
            definition = iDefinition->value;
        }

        
        // Now that we have information about the model, we can create a new Model object and return it.
        if (definition != nullptr)
        {
            return ModelLibrary::CreateFromDefinition(*definition);
        }


        return nullptr;
    }


    bool ModelLibrary::ReloadModel(const char* fileName)
    {
        (void)fileName;
        return false;

        /*
        // We need to find the definition that we're updating.
        auto definition = ModelLibrary::FindDefinition(fileName);
        if (definition != nullptr)
        {
            ModelLibrary::Load(fileName, *definition);
            return true;
        }

        return false;
        */
    }


    bool ModelLibrary::WriteToFile(const ModelDefinition &definition, const char* fileNameIn)
    {
        // We have a model, so now we need to check that we can open the file.
        GTCore::String fileName(fileNameIn);
        if (!GTCore::Path::ExtensionEqual(fileNameIn, "gtmodel"))
        {
            fileName += ".gtmodel";
        }

        auto file = GTCore::IO::Open(fileName.c_str(), GTCore::IO::OpenMode::Write);
        if (file != nullptr)
        {
            // First we write four bytes: "gtem".
            GTCore::IO::Write(file, "gtem", 4);

            // Now the version.
            uint32_t version = 1;
            GTCore::IO::Write(file, &version, 4);


            // Now we write the counts of everything.
            uint32_t boneCount     = static_cast<uint32_t>(definition.bones.count);
            uint32_t meshCount     = static_cast<uint32_t>(definition.meshGeometries.count);
            uint32_t keyFrameCount = static_cast<uint32_t>(definition.animation.GetKeyFrameCount());
            GTCore::IO::Write(file, &boneCount,     4);
            GTCore::IO::Write(file, &meshCount,     4);
            GTCore::IO::Write(file, &keyFrameCount, 4);


            // Bones.
            //
            // We start with bones because everything else depends on them. Everything accesses the bone information by indexing into this array with an integer. Everything here is in
            // alphabetical order to work nicely in a map data structure.
            //
            // The index of parent bones are written as a seperate chunk straight after this chunk.
            for (uint32_t iBone = 0; iBone < boneCount; ++iBone)
            {
                auto bone = definition.bones.buffer[iBone]->value;
                assert(bone != nullptr);

                // Name.
                uint32_t nameLength = GTCore::Strings::SizeInBytes(bone->GetName());
                GTCore::IO::Write(file, &nameLength, 4);
                GTCore::IO::Write(file, bone->GetName(), nameLength);


                // Relative transformation.
                glm::vec3 position = bone->GetPosition();
                glm::quat rotation = bone->GetRotation();
                glm::vec3 scale    = bone->GetScale();
                GTCore::IO::Write(file, &position, sizeof(float) * 3);
                GTCore::IO::Write(file, &rotation, sizeof(float) * 4);
                GTCore::IO::Write(file, &scale,    sizeof(float) * 3);

                
                // 4x4 offset matrix.
                const glm::mat4 &offsetMatrix = bone->GetOffsetMatrix();
                GTCore::IO::Write(file, &offsetMatrix, sizeof(float) * 16);
            }

            // Bone parents. We use -1 for bones without parents.
            for (uint32_t iBone = 0; iBone < boneCount; ++iBone)
            {
                auto bone = definition.bones.buffer[iBone]->value;
                assert(bone != nullptr);

                uint32_t parentIndex = static_cast<uint32_t>(-1);

                if (bone->GetParent() != nullptr)
                {
                    auto iParentBone = definition.bones.Find(bone->GetParent()->GetName());
                    assert(iParentBone != nullptr);

                    parentIndex = static_cast<uint32_t>(iParentBone->index);
                }

                GTCore::IO::Write(file, &parentIndex, 4);
            }


            // Meshes.
            //
            // The vertex data is always saved as P3T2N3T3B3 format. Bones are referenced using an integer to index into the list of bones defined above.
            for (uint32_t iMesh = 0; iMesh < meshCount; ++iMesh)
            {
                // Geometry
                auto va = definition.meshGeometries[iMesh];
                assert(va != nullptr);
                assert(va->GetFormat().GetSize() == VertexFormat::P3T2N3T3B3.GetSize());

                // First is the vertex data.
                uint32_t vertexCount = static_cast<uint32_t>(va->GetVertexCount());
                GTCore::IO::Write(file, &vertexCount, 4);
                if (vertexCount > 0) GTCore::IO::Write(file, va->GetVertexDataPtr(), sizeof(float) * va->GetFormat().GetSize() * vertexCount);

                // Now the index data.
                uint32_t indexCount = static_cast<uint32_t>(va->GetIndexCount());
                GTCore::IO::Write(file, &indexCount, 4);
                if (indexCount > 0) GTCore::IO::Write(file, va->GetIndexDataPtr(), sizeof(unsigned int) * indexCount);


                // Skinning vertex attributes. We save two chunks of data here. The first is a bone count and the second is the actual bone/weight pairs. We keep these
                // separate to encourage batched loading.
                uint32_t skinningVertexAttributeCount = (definition.meshSkinningVertexAttributes[iMesh] != nullptr) ? vertexCount : 0;
                GTCore::IO::Write(file, &skinningVertexAttributeCount, 4);

                if (skinningVertexAttributeCount > 0)
                {
                    uint32_t totalBoneWeights = 0;

                    // 1) Counts. Stored as 16-bit integers here.
                    for (uint32_t iVertex = 0; iVertex < skinningVertexAttributeCount; ++iVertex)
                    {
                        uint16_t count = static_cast<uint16_t>(definition.meshSkinningVertexAttributes[iMesh][iVertex].bones.count);
                        GTCore::IO::Write(file, &count, 2);

                        totalBoneWeights += count;
                    }

                    // 2) Bone/Weight pairs. We write a total count to make it easier to calculate buffers at load time.
                    GTCore::IO::Write(file, &totalBoneWeights, 4);

                    for (uint32_t iVertex = 0; iVertex < skinningVertexAttributeCount; ++iVertex)
                    {
                        auto &bones = definition.meshSkinningVertexAttributes[iMesh][iVertex].bones;

                        for (uint32_t iBone = 0; iBone < bones.count; ++iBone)
                        {
                            uint32_t boneIndex = static_cast<uint32_t>(bones[iBone].boneIndex);
                            float    weight    = static_cast<float>(bones[iBone].weight);

                            GTCore::IO::Write(file, &boneIndex, 4);
                            GTCore::IO::Write(file, &weight,    4);
                        }
                    }
                }
            }


            // Now we need to write the animation keys. There a channel for every bone at every key frame. The data is the transformation of the bone.
            // We loop over each key, and then save the bone transformation at every channel on that key.
            for (uint32_t iKeyFrame = 0; iKeyFrame < keyFrameCount; ++iKeyFrame)
            {
                float time = static_cast<float>(definition.animation.GetKeyFrameTimeByIndex(iKeyFrame));
                GTCore::IO::Write(file, &time, 4);

                // Now we need to iterate over every channel and get the key data for this key frame.
                uint32_t channelCount = definition.animationChannelBones.count;
                GTCore::IO::Write(file, &channelCount, 4);

                for (size_t j = 0; j < channelCount; ++j)
                {
                    auto jChannel = definition.animationChannelBones.buffer[j];
                    assert(jChannel != nullptr);

                    auto channel = jChannel->value;
                    assert(channel != nullptr);

                    auto bone = jChannel->key;
                    assert(bone != nullptr);


                    // We need to save the index of the bone that this channel is referring to.
                    auto iBone = definition.bones.Find(bone->GetName());
                    assert(iBone != nullptr);

                    uint32_t boneIndex = static_cast<uint32_t>(iBone->index);
                    GTCore::IO::Write(file, &boneIndex, 4);

                    
                    // Here we save the key. The key will always be a TransformAnimationKey.
                    glm::vec3 position;
                    glm::quat rotation;
                    glm::vec3 scale;

                    auto key = static_cast<TransformAnimationKey*>(channel->GetKey(iKeyFrame));
                    if (key != nullptr)
                    {
                        position = key->position;
                        rotation = key->rotation;
                        scale    = key->scale;
                    }
                    else
                    {
                        // We should never actually get here, but for completeness we'll just write identities.
                        position = bone->GetPosition();
                        rotation = bone->GetRotation();
                        scale    = bone->GetScale();
                    }

                    GTCore::IO::Write(file, &position, sizeof(float) * 3);
                    GTCore::IO::Write(file, &rotation, sizeof(float) * 4);
                    GTCore::IO::Write(file, &scale,    sizeof(float) * 3);
                }
            }


            // Here is where we write the engine-specific metadata.
            ModelLibrary::WriteGTMODELMetadata(file, definition);

            
            // Can't forget to close the file.
            GTCore::IO::Close(file);


            return true;
        }

        return false;
    }

    bool ModelLibrary::WriteToFile(const char* fileName)
    {
        // We first need to find the definition. If we can't find it with the original file name, we add '.gtmodel' and try again. If both fail, we need
        // to return false.
        auto iDefinition = LoadedDefinitions.Find(fileName);
        if (iDefinition == nullptr)
        {
            iDefinition = LoadedDefinitions.Find((GTCore::String(fileName) + ".gtmodel").c_str());
        }

        if (iDefinition != nullptr)
        {
            return ModelLibrary::WriteToFile(*iDefinition->value, fileName);
        }
        
        return false;
    }

    
    

    ////////////////////////////////////////////////////////////////////////
    // Create functions.

    Model* ModelLibrary::CreateFromDefinition(const ModelDefinition &definition)
    {
        auto model = new Model(definition);

        auto iDefinitionModels = LoadedModels.Find(const_cast<ModelDefinition*>(&definition));        // <-- Naughty const_cast is OK here.
        assert(iDefinitionModels        != nullptr);
        assert(iDefinitionModels->value != nullptr);

        iDefinitionModels->value->PushBack(model);

        return model;
    }

    Model* ModelLibrary::CreatePlaneXZ(float width, float height, VertexFormat &format)
    {
        // We need a unique identifier for this mesh. We will base it on the size of the box.
        char name[128];
        GTCore::IO::snprintf(name, 128, "prim:planexz(%.4f %.4f)", width, height);

        // We create the model from a primitive. To do this we need a non-const vertex array.
        VertexArray* va = nullptr;

        bool exists = LoadedDefinitions.Find(name) != nullptr;
        if (!exists)
        {
            va = VertexArrayFactory::CreatePlaneXZ(width, height, format);
        }

        return ModelLibrary_CreateFromPrimitive(name, va);
    }

    Model* ModelLibrary::CreatePlaneXY(float width, float height, VertexFormat &format)
    {
        // We need a unique identifier for this mesh. We will base it on the size of the box.
        char name[128];
        GTCore::IO::snprintf(name, 128, "prim:planexy(%.4f %.4f)", width, height);

        // We create the model from a primitive. To do this we need a non-const vertex array.
        VertexArray* va = nullptr;

        bool exists = LoadedDefinitions.Find(name) != nullptr;
        if (!exists)
        {
            va = VertexArrayFactory::CreatePlaneXY(width, height, format);
        }

        return ModelLibrary_CreateFromPrimitive(name, va);
    }


    Model* ModelLibrary::CreateBox(float halfWidth, float halfHeight, float halfDepth)
    {
        // We need a unique identifier for this mesh. We will base it on the size of the box.
        char name[128];
        GTCore::IO::snprintf(name, 128, "prim:box(%.4f %.4f %.4f)", halfWidth, halfHeight, halfDepth);

        // We create the model from a primitive. To do this we need a non-const vertex array.
        VertexArray* va = nullptr;

        bool exists = LoadedDefinitions.Find(name) != nullptr;
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
        bool applyTransform = LoadedDefinitions.Find((GTCore::String("__nff:") + name).c_str()) == nullptr;

        // For now the way we will do this is load an NFF file. We use a radius of 1, and then scale that with a transformation.
        auto model = ModelLibrary::LoadFromNFF(content, name);
        if (model != nullptr && applyTransform)
        {
            //model->ApplyTransformation(glm::scale(halfWidth, halfHeight, halfDepth));
        }

        return model;
    }

    static int ConvexHullCount = 0;
    Model* ModelLibrary::CreateFromConvexHull(const ConvexHull &convexHull)
    {
        // We need a unique identifier for this mesh. We will base it on the size of the box.
        char name[128];
        GTCore::IO::snprintf(name, 128, "convexhull(%d)", ConvexHullCount++);

        // We create the model from a primitive. To do this we need a non-const vertex array.
        VertexArray* va = nullptr;

        bool exists = LoadedDefinitions.Find(name) != nullptr;
        if (!exists)
        {
            va = VertexArrayLibrary::CreateFromConvexHull(convexHull);
        }

        return ModelLibrary_CreateFromPrimitive(name, va);
    }


    void ModelLibrary::Delete(Model* model)
    {
        if (model != nullptr)
        {
            // We need to find the list this model is part of and remove it.
            auto iDefinitionModels = LoadedModels.Find(const_cast<ModelDefinition*>(&model->GetDefinition()));      // <-- const_cast is safe here.
            assert(iDefinitionModels        != nullptr);
            assert(iDefinitionModels->value != nullptr);

            iDefinitionModels->value->RemoveFirst(model);;
            delete model;
        }
    }

    void ModelLibrary::DeleteUnreferenceDefinitions()
    {
        GTCore::List<ModelDefinition*> definitionsToDelete;

        for (size_t i = 0; i < LoadedModels.count; ++i)
        {
            auto iDefinitionModels = LoadedModels.buffer[i];
            assert(iDefinitionModels        != nullptr);
            assert(iDefinitionModels->value != nullptr);

            if (iDefinitionModels->value->count == 0)
            {
                auto definition = iDefinitionModels->key;
                auto modelsList = iDefinitionModels->value;

                definitionsToDelete.Append(definition);
                
                delete definition;
                delete modelsList;
            }
        }


        // Now all we do is remove the old pointers.
        while (definitionsToDelete.root != nullptr)
        {
            auto definition = definitionsToDelete.root->value;

            for (size_t i = 0; i < LoadedDefinitions.count; ++i)
            {
                if (LoadedDefinitions.buffer[i]->value == definition)
                {
                    LoadedDefinitions.RemoveByIndex(i);
                    break;
                }
            }
            
            LoadedModels.Remove(definition);
        }
    }


    bool ModelLibrary::IsExtensionSupported(const char* extension)
    {
        GTCore::String assimpExt(".");
        assimpExt += extension;

        Assimp::Importer importer;
        return importer.IsExtensionSupported(assimpExt.c_str());
    }



    ////////////////////////////////////////////////////////
    // Private

    bool ModelLibrary::Load(const char* fileName, ModelDefinition &definition)
    {
        if (GTCore::Path::ExtensionEqual(fileName, "gtmodel"))
        {
            return ModelLibrary::LoadFromGTMODEL(fileName, definition);
        }
        else
        {
            return ModelLibrary::LoadFromAssimp(fileName, definition);
        }
    }

    bool ModelLibrary::LoadFromAssimp(const char* fileName, ModelDefinition &definition)
    {
        // What we're going to do is load two file info's. The first will be the original source file. The other will be the .gtmodel file. If
        // the .gtmodel file does not exist, we create it from the source model.
        GTCore::IO::FileInfo sourceInfo;
        GTCore::IO::GetFileInfo(fileName, sourceInfo);

        GTCore::IO::FileInfo gtmodelInfo;
        if (sourceInfo.exists)
        {
            GTCore::String gtmodelFileName(sourceInfo.absolutePath);
            gtmodelFileName += ".gtmodel";

            GTCore::IO::GetFileInfo(gtmodelFileName.c_str(), gtmodelInfo);
        }
        else
        {
            GTCore::String gtmodelFileName(fileName);
            gtmodelFileName += ".gtmodel";

            GTCore::IO::GetFileInfo(gtmodelFileName.c_str(), gtmodelInfo);
        }


        if ((sourceInfo.exists && sourceInfo.lastModifiedTime > gtmodelInfo.lastModifiedTime) || !gtmodelInfo.exists)
        {
            return ModelLibrary::LoadFromAssimp(sourceInfo, gtmodelInfo, definition);
        }
        else
        {
            if ((!sourceInfo.exists && gtmodelInfo.exists) || gtmodelInfo.lastModifiedTime > sourceInfo.lastModifiedTime)
            {
                return ModelLibrary::LoadFromGTMODEL(gtmodelInfo.absolutePath.c_str(), definition);
            }
            else
            {
                GTEngine::PostError("Could not find file: %s", fileName);
            }
        }

        return false;
    }

    bool ModelLibrary::LoadFromGTMODEL(const char* fileName, ModelDefinition &definition)
    {
        auto file = GTCore::IO::Open(fileName, GTCore::IO::OpenMode::Read);
        if (file != nullptr)
        {
            // The first thing we load is the header, which is 8 bytes. The first 4 bytes should equal "gtem".
            struct
            {
                char     id[4];
                uint32_t version;
            }header;

            GTCore::IO::Read(file, &header, 8);

            // We need to check the first 4 bytes to ensure they read "gtem".
            if (header.id[0] == 'g' && header.id[1] == 't' && header.id[2] == 'e' && header.id[3] == 'm')
            {
                // We load differently depending on the version. Unknown version means return null.
                switch (header.version)
                {
                case 1:
                    {
                        // The counts of the main objects. We batch these into a single call at the top of everything.
                        struct
                        {
                            uint32_t boneCount;
                            uint32_t meshCount;
                            uint32_t keyFrameCount;
                        }counts;

                        GTCore::IO::Read(file, &counts, 12);


                        // Bones.
                        //
                        // Bones are first because they are referenced by everything else.
                        for (uint32_t iBone = 0; iBone < counts.boneCount; ++iBone)
                        {
                            // Name.
                            uint32_t nameLength;
                            GTCore::IO::Read(file, &nameLength, 4);

                            auto name = static_cast<char*>(malloc(nameLength + 1));                 // <-- +1 for null terminator.
                            GTCore::IO::Read(file, name, nameLength); name[nameLength] = '\0';


                            // Relative transform and offset matrix.
                            struct
                            {
                                glm::vec3 position;
                                glm::quat rotation;
                                glm::vec3 scale;
                                glm::mat4 offsetMatrix;
                            }transformOffset;

                            GTCore::IO::Read(file, &transformOffset, sizeof(transformOffset));


                            // Now we create the bone and add it to the list.
                            auto bone = new Bone;
                            bone->SetName(name);
                            bone->SetPosition(transformOffset.position);
                            bone->SetRotation(transformOffset.rotation);
                            bone->SetScale(transformOffset.scale);
                            bone->SetOffsetMatrix(transformOffset.offsetMatrix);
                            definition.bones.Add(name, bone);


                            // We need to create a channel for this bone. We then need to map that channel to a bone.
                            auto &channel = definition.animation.CreateChannel();
                            definition.animationChannelBones.Add(bone, &channel);


                            free(name);
                        }

                        // Bone parents.
                        auto boneParentIndices = static_cast<uint32_t*>(malloc(counts.boneCount * 4));
                        GTCore::IO::Read(file, boneParentIndices, counts.boneCount * 4);

                        for (uint32_t iBone = 0; iBone < counts.boneCount; ++iBone)
                        {
                            uint32_t parentIndex = boneParentIndices[iBone];

                            if (parentIndex != static_cast<uint32_t>(-1))
                            {
                                definition.bones.buffer[parentIndex]->value->AttachChild(*definition.bones.buffer[iBone]->value);
                            }
                        }

                        free(boneParentIndices);


                        // Meshes.
                        //
                        // Meshes reference bones, so we do these afterwards.
                        for (uint32_t i = 0; i < counts.meshCount; ++i)
                        {
                            // Geometry.
                            {
                                auto va = new VertexArray(VertexArrayUsage_Static, VertexFormat::P3T2N3T3B3);

                                // Vertices.
                                uint32_t vertexCount;
                                GTCore::IO::Read(file, &vertexCount, 4);

                                if (vertexCount > 0)
                                {
                                    va->SetVertexData(nullptr, static_cast<size_t>(vertexCount));
                                    auto vertexData = va->MapVertexData();
                                    {
                                        GTCore::IO::Read(file, vertexData, vertexCount * VertexFormat::P3T2N3T3B3.GetSize() * sizeof(float));
                                    }
                                    va->UnmapVertexData();
                                }


                                // Indices.
                                uint32_t indexCount;
                                GTCore::IO::Read(file, &indexCount, 4);
                                
                                if (indexCount > 0)
                                {
                                    va->SetIndexData(nullptr, static_cast<size_t>(indexCount));
                                    auto indexData = va->MapIndexData();
                                    {
                                        GTCore::IO::Read(file, indexData, indexCount * sizeof(unsigned int));
                                    }
                                    va->UnmapIndexData();
                                }

                                
                                definition.meshGeometries.PushBack(va);
                            }


                            // Skinning vertex attributes.
                            {
                                uint32_t skinningVertexAttributeCount;
                                GTCore::IO::Read(file, &skinningVertexAttributeCount, 4);

                                if (skinningVertexAttributeCount > 0)
                                {
                                    auto skinningVertexAttributes = new SkinningVertexAttribute[skinningVertexAttributeCount];


                                    auto counts = static_cast<uint16_t*>(malloc(skinningVertexAttributeCount * 2));
                                    GTCore::IO::Read(file, counts, skinningVertexAttributeCount * 2);

                                    uint32_t totalBoneWeights;
                                    GTCore::IO::Read(file, &totalBoneWeights, 4);

                                    auto boneWeights = static_cast<BoneWeightPair*>(malloc(totalBoneWeights * 8));
                                    GTCore::IO::Read(file, boneWeights, totalBoneWeights * 8);


                                    auto currentBoneWeight = boneWeights;
                                    for (uint32_t iVertex = 0; iVertex < skinningVertexAttributeCount; ++iVertex)
                                    {
                                        auto count = counts[iVertex];

                                        // Here we allocate the buffer for the bones. We trick the vector here by modifying attributes directly.
                                        skinningVertexAttributes[iVertex].bones.Reserve(count);
                                        skinningVertexAttributes[iVertex].bones.count = count;

                                        for (uint16_t iBone = 0; iBone < count; ++iBone)
                                        {
                                            skinningVertexAttributes[iVertex].bones[iBone] = *currentBoneWeight++;
                                        }
                                    }


                                    definition.meshSkinningVertexAttributes.PushBack(skinningVertexAttributes);


                                    free(counts);
                                    free(boneWeights);
                                }
                                else
                                {
                                    definition.meshSkinningVertexAttributes.PushBack(nullptr);
                                }
                            }
                        }


                        // Now we need to load the key frame animation data.
                        for (uint32_t iFrame = 0; iFrame < counts.keyFrameCount; ++iFrame)
                        {
                            float time;
                            GTCore::IO::Read(file, &time, 4);

                            size_t keyFrameIndex = definition.animation.AppendKeyFrame(static_cast<double>(time));

                            // With the key frame added, we now need to iterate over each channel in the key frame.
                            uint32_t channelCount;
                            GTCore::IO::Read(file, &channelCount, 4);

                            for (uint32_t iChannel = 0; iChannel < channelCount; ++iChannel)
                            {
                                // The first attribute is the bone index.
                                uint32_t boneIndex;
                                GTCore::IO::Read(file, &boneIndex, 4);

                                auto bone = definition.bones.buffer[boneIndex]->value;

                                auto channel = definition.animationChannelBones.Find(bone);
                                assert(channel != nullptr);


                                // Transformation.
                                struct
                                {
                                    glm::vec3 position;
                                    glm::quat rotation;
                                    glm::vec3 scale;
                                }transform;

                                GTCore::IO::Read(file, &transform, sizeof(transform));


                                // All of the data has been read, so now we just create a TransformAnimationKey and add it to the cache and channel.
                                auto key = new TransformAnimationKey(transform.position, transform.rotation, transform.scale);
                                channel->value->SetKey(keyFrameIndex, key);

                                // We need to cache the key.
                                definition.animationKeyCache.PushBack(key);
                            }
                        }


                        // Here is the engine-specific metadata.
                        ModelLibrary::LoadGTMODELMetadata(file, definition);
                            

                        // Can't forget to close the file.
                        GTCore::IO::Close(file);

                        return true;
                    }

                default:
                    {
                        GTCore::IO::Close(file);

                        GTEngine::PostError("Can not load .gtmodel file: Unknown version.");
                        return false;
                    }
                }
            }
            else
            {
                GTCore::IO::Close(file);

                GTEngine::PostError("Can not load .gtmodel file: Invalid header ID.");
                return false;
            }
        }
        else
        {
            GTEngine::PostError("Can not open file: %s.", fileName);
            return false;
        }
    }


    bool ModelLibrary::LoadFromAssimp(const GTCore::IO::FileInfo &sourceInfo, const GTCore::IO::FileInfo &gtmodelInfo, ModelDefinition &definition)
    {
        Assimp::Importer importer;
        importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, AssimpRemovedComponentsFlags);

        auto scene = importer.ReadFile(sourceInfo.absolutePath.c_str(), AssimpReadFileFlags);
        if (scene != nullptr)
        {
            if (!ModelLibrary_LoadFromAssimpScene(*scene, definition))
            {
                GTEngine::PostError("Error creating model info for %s: %s", sourceInfo.absolutePath.c_str(), importer.GetErrorString());
                return false;
            }
        }
        else
        {
            GTEngine::PostError("Error importing %s: %s", sourceInfo.absolutePath.c_str(), importer.GetErrorString());
            return false;
        }


        // Here is where we save the corresponding .gtmodel file. Note here how we use the aboslute path in 'sourceInfo' instead
        // of 'gtmodelInfo'. This is because it's possible that the gtmodel file does not exists and may have an incorrect
        // absolute path as a result.
        GTCore::String gtmodelFileName(sourceInfo.absolutePath);
        gtmodelFileName += ".gtmodel";

        // If the gtmodel file exists, we need to read in it's meta data so we can re-save that info in the new file. The way we
        // do this, is we actually load the gtmodel meta data into the definition.
        if (gtmodelInfo.exists)
        {
            ModelLibrary::LoadGTMODELMetadata(gtmodelInfo.absolutePath.c_str(), definition);
        }
        else
        {
            ModelLibrary::LoadDefaultMetadata(definition);
        }

        // Now we just save the .gtmodel like normal.
        ModelLibrary::WriteToFile(definition, gtmodelInfo.absolutePath.c_str());

        return true;
    }

    bool ModelLibrary::LoadGTMODELMetadata(FILE* file, ModelDefinition &definition)
    {
        assert(file != nullptr);

        // The next 8 bytes should read 'metadata'.
        uint8_t header[8];
        GTCore::IO::Read(file, header, 8);

        if (header[0] == 'm' && header[1] == 'e' && header[2] == 't' && header[3] == 'a' && header[4] == 'd' && header[5] == 'a' && header[6] == 't' && header[7] == 'a')
        {
            // We're at the metadata. Below we will have chunks of metadata for different things. Each chunk has a header or 8 bytes, with the
            // first 4 bytes being an ID, and the other 4 bytes being the size of the metadata. We need to store the size so we can skip over
            // it if it's unused.
            //
            // We keep skipping over until we hit the end of the file.
            struct
            {
                uint8_t  id[4];
                uint32_t sizeInBytes;

            }metadataHeader;

            while (GTCore::IO::Read(file, &metadataHeader, 8) == 8)
            {
                if (metadataHeader.id[0] == 'm' && metadataHeader.id[1] == 't' && metadataHeader.id[2] == 'l' && metadataHeader.id[3] == 's')           // <-- Materials: 'mtls'
                {
                    uint32_t materialCount;
                    GTCore::IO::Read(file, &materialCount, 4);

                    if (definition.meshMaterials.count == 0)
                    {
                        for (uint32_t iMaterial = 0; iMaterial < materialCount; ++iMaterial)
                        {
                            definition.meshMaterials.PushBack(nullptr);
                        }
                    }

                    for (uint32_t iMaterial = 0; iMaterial < materialCount; ++iMaterial)
                    {
                        uint32_t meshIndex;
                        GTCore::IO::Read(file, &meshIndex, 4);

                        uint32_t fileNameLength;
                        GTCore::IO::Read(file, &fileNameLength, 4);

                        GTCore::String fileName;

                        if (length > 0)
                        {
                            auto tempFileName = static_cast<char*>(malloc(fileNameLength + 1));                         // <-- +1 for null terminator. Not included in 'fileNameLength'.
                            GTCore::IO::Read(file, tempFileName, fileNameLength); tempFileName[fileNameLength] = '\0';

                            fileName = tempFileName;

                            free(tempFileName);
                        }
                        else
                        {
                            fileName = "engine/materials/simple-diffuse.material";
                        }

                        if (meshIndex < definition.meshMaterials.count)
                        {
                            definition.meshMaterials[meshIndex] = MaterialLibrary::Create(fileName.c_str());
                        }
                    }
                }
                else if (metadataHeader.id[0] == 'n' && metadataHeader.id[1] == 'a' && metadataHeader.id[2] == 's' && metadataHeader.id[3] == ' ')      // <-- Named animation segments: 'nas '.
                {
                    uint32_t segmentCount;
                    GTCore::IO::Read(file, &segmentCount, 4);

                    for (uint32_t iSegment = 0; iSegment < segmentCount; ++iSegment)
                    {
                        uint32_t nameLength;
                        GTCore::IO::Read(file, &nameLength, 4);

                        auto name = static_cast<char*>(malloc(nameLength + 1));                     // <-- +1 for null terminator.
                        GTCore::IO::Read(file, name, nameLength); name[nameLength] = '\0';

                        struct
                        {
                            uint32_t start;
                            uint32_t end;
                        }frameIndices;

                        GTCore::IO::Read(file, &frameIndices, 8);

                        // The data has been read, so now we just add it to the animation.
                        definition.animation.AddNamedSegment(name, frameIndices.start, frameIndices.end);

                        free(name);
                    }
                }
                else if (metadataHeader.id[0] == 'c' && metadataHeader.id[1] == 'x' && metadataHeader.id[2] == 'h' && metadataHeader.id[3] == 'l')      // <-- Convex Hulls: 'cxhl'
                {
                    uint32_t hullCount;
                    GTCore::IO::Read(file, &hullCount, 4);

                    if (hullCount > 0)
                    {
                        // Here is where we load the convex hull information. This stuff is all batched so it may seem a little unintuitive.
                        uint32_t* vertexCounts = new uint32_t[hullCount];
                        uint32_t* indexCounts  = new uint32_t[hullCount];

                        GTCore::IO::Read(file, vertexCounts, hullCount * 4);
                        GTCore::IO::Read(file, indexCounts,  hullCount * 4);

                        struct
                        {
                            uint32_t totalVertexCount;
                            uint32_t totalIndexCount;
                        }vertexIndexCounts;
                        GTCore::IO::Read(file, &vertexIndexCounts, 8);

                        float* vertices = new float[vertexIndexCounts.totalVertexCount];
                        GTCore::IO::Read(file, vertices, vertexIndexCounts.totalVertexCount * 4);

                        uint32_t* indices = new uint32_t[vertexIndexCounts.totalIndexCount];
                        GTCore::IO::Read(file, indices, vertexIndexCounts.totalIndexCount * 4);


                        auto currentVertices = vertices;
                        auto currentIndices  = indices;

                        for (uint32_t iHull = 0; iHull < hullCount; ++iHull)
                        {
                            size_t vertexCount = static_cast<size_t>(vertexCounts[iHull]);
                            size_t indexCount  = static_cast<size_t>(indexCounts[iHull]);

                            definition.convexHulls.PushBack(new ConvexHull(currentVertices, vertexCount, currentIndices, indexCount));

                            // Now we need to move our pointers forward.
                            currentVertices += vertexCount * 3;
                            currentIndices  += indexCount;
                        }

                        delete [] vertexCounts;
                        delete [] indexCounts;
                        delete [] vertices;
                        delete [] indices;
                    }


                    // These are the build settings used to build the convex hull.
                    uint32_t settingsSizeInBytes;
                    GTCore::IO::Read(file, &settingsSizeInBytes, 4);

                    if (settingsSizeInBytes == sizeof(definition.convexHullBuildSettings))
                    {
                        GTCore::IO::Read(file, &definition.convexHullBuildSettings, settingsSizeInBytes);
                    }
                    else
                    {
                        Log("Warning loading .gtmodel: sizeof(chbs) != sizeof(ConvexHullBuildSettings). Skipping.");
                        GTCore::IO::Seek(file, settingsSizeInBytes, GTCore::IO::SeekOrigin::Current);
                    }

                }
                else
                {
                    // We don't recognize this chunk. We'll skip over it. What we should probably look into is the ability to allow custom metadata
                    // and use some kind of callback.
                    GTCore::IO::Seek(file, metadataHeader.sizeInBytes, GTCore::IO::SeekOrigin::Current);
                }
            }


            // We need to do some validation here.
            //
            // The first thing to validate is the materials. If we have any unset materials, we need to give it the default material.
            if (definition.meshMaterials.count == 0)
            {
                for (size_t i = 0; i < definition.meshGeometries.count; ++i)
                {
                    definition.meshMaterials.PushBack(nullptr);
                }
            }

            for (size_t iMaterial = 0; iMaterial < definition.meshMaterials.count; ++iMaterial)
            {
                if (definition.meshMaterials[iMaterial] == nullptr)
                {
                    definition.meshMaterials[iMaterial] = MaterialLibrary::Create("engine/materials/simple-diffuse.material");
                }
            }

            return true;
        }
        else
        {
            return false;
        }
    }


    bool ModelLibrary::WriteGTMODELMetadata(FILE* file, const ModelDefinition &definition)
    {
        assert(file != nullptr);

        GTCore::IO::Write(file, "metadata", 8);


        // Materials.
        {
            uint32_t materialCount = static_cast<uint32_t>(definition.meshMaterials.count);

            // We first need to determine the size of the chunk.
            uint32_t mtlsSizeInBytes = 0;
            mtlsSizeInBytes += 4;                                       // <-- Variable for storing the material count.

            for (uint32_t iMaterial = 0; iMaterial < materialCount; ++iMaterial)
            {
                mtlsSizeInBytes += 4;                                   // <-- Variable for storing the index of the mesh this material is applied to.
                mtlsSizeInBytes += 4;                                   // <-- Variable for storing the size of the 
                mtlsSizeInBytes += definition.meshMaterials[iMaterial]->GetDefinition().fileName.GetLengthInTs();
            }


            // Now we write the actual data.
            GTCore::IO::Write(file, "mtls", 4);             // <-- Note the space at the end of 'nas '.
            GTCore::IO::Write(file, &mtlsSizeInBytes, 4);

            GTCore::IO::Write(file, &materialCount, 4);

            for (uint32_t iMaterial = 0; iMaterial < materialCount; ++iMaterial)
            {
                auto material = definition.meshMaterials[iMaterial];

                GTCore::IO::Write(file, &iMaterial, 4);
                
                if (material == nullptr)
                {
                    uint32_t length = 0;
                    GTCore::IO::Write(file, &length, 4);
                }
                else
                {
                    uint32_t length = static_cast<uint32_t>(material->GetDefinition().fileName.GetLength());
                    GTCore::IO::Write(file, &length, 4);
                    GTCore::IO::Write(file, material->GetDefinition().fileName.c_str(), static_cast<size_t>(length));
                }
            }
        }


        // Named Animation Segments.
        {
            uint32_t segmentCount = static_cast<uint32_t>(definition.animation.GetNamedSegmentCount());

            // We first need to determine the size of the metadata.
            uint32_t nasSizeInBytes = 0;
            nasSizeInBytes += 4;                                        // <-- Variable for storing the number of segments.

            for (uint32_t iSegment = 0; iSegment < segmentCount; ++iSegment)
            {
                auto name    = definition.animation.GetNamedSegmentNameByIndex(iSegment);
                auto segment = definition.animation.GetNamedSegmentByIndex(iSegment);
                assert(name    != nullptr);
                assert(segment != nullptr);

                nasSizeInBytes += 4;                                    // <-- Variable for storing the name length.
                nasSizeInBytes += GTCore::Strings::SizeInBytes(name);   // <-- The actual name length.
                nasSizeInBytes += 4;                                    // <-- Variable for storing the start key frame.
                nasSizeInBytes += 4;                                    // <-- Variable for storing the end key frame.
            }


            GTCore::IO::Write(file, "nas ", 4);             // <-- Note the space at the end of 'nas '.
            GTCore::IO::Write(file, &nasSizeInBytes, 4);

            GTCore::IO::Write(file, &segmentCount, 4);

            for (uint32_t iSegment = 0; iSegment < segmentCount; ++iSegment)
            {
                auto name    = definition.animation.GetNamedSegmentNameByIndex(iSegment);
                auto segment = definition.animation.GetNamedSegmentByIndex(iSegment);
                assert(name    != nullptr);
                assert(segment != nullptr);

                uint32_t nameLength = GTCore::Strings::SizeInBytes(name);
                GTCore::IO::Write(file, &nameLength, 4);
                GTCore::IO::Write(file, name, nameLength);

                uint32_t start = static_cast<uint32_t>(segment->startKeyFrame);
                uint32_t end   = static_cast<uint32_t>(segment->endKeyFrame);
                GTCore::IO::Write(file, &start, 4);
                GTCore::IO::Write(file, &end,   4);
            }
        }


        // Convex Hull.
        {
            // We first need to determine the size of the metadata.
            uint32_t cxhlSizeInBytes = 0;
            cxhlSizeInBytes += 4;                                       // <-- Variable for storing the convex hull count.

            if (definition.convexHulls.count > 0)
            {
                cxhlSizeInBytes += definition.convexHulls.count * 4;        // <-- Variable for storing the counts of vertices.
                cxhlSizeInBytes += definition.convexHulls.count * 4;        // <-- Variable for storing the counds of indices.

                for (size_t iHull = 0; iHull < definition.convexHulls.count; ++iHull)
                {
                    auto hull = definition.convexHulls[iHull];
                    assert(hull != nullptr);

                    cxhlSizeInBytes += 4;                                   // <-- Variable for storing the vertex count.
                    cxhlSizeInBytes += 4;                                   // <-- Variable for storing the index count.

                    cxhlSizeInBytes += hull->GetVertexCount() * 4 * 3;      // <-- Each vertex, 3x floats each.
                    cxhlSizeInBytes += hull->GetIndexCount()  * 4;          // <-- Each index, 1x uint32 each.
                }
            }

            cxhlSizeInBytes += sizeof(ConvexHullBuildSettings);             // <-- The size of the build settings.




            // Now we can actually write the data.
            GTCore::IO::Write(file, "cxhl",           4);
            GTCore::IO::Write(file, &cxhlSizeInBytes, 4);

            GTCore::IO::Write(file, &definition.convexHulls.count, 4);

            if (definition.convexHulls.count > 0)
            {
                // Here we pack up our data.
                GTCore::Vector<uint32_t> vertexCounts(definition.convexHulls.count);
                GTCore::Vector<uint32_t> indexCounts(definition.convexHulls.count);
                GTCore::Vector<float>    vertices;
                GTCore::Vector<uint32_t> indices;

                for (uint32_t iHull = 0; iHull < definition.convexHulls.count; ++iHull)
                {
                    auto hull = definition.convexHulls[iHull];
                    assert(hull != nullptr);

                    uint32_t vertexCount = static_cast<uint32_t>(hull->GetVertexCount());
                    uint32_t indexCount  = static_cast<uint32_t>(hull->GetIndexCount());

                    vertexCounts.PushBack(vertexCount);
                    indexCounts.PushBack( indexCount);

                    for (uint32_t iVertex = 0; iVertex < vertexCount; ++iVertex)
                    {
                        vertices.PushBack(hull->GetVertices()[iVertex * 3 + 0]);
                        vertices.PushBack(hull->GetVertices()[iVertex * 3 + 1]);
                        vertices.PushBack(hull->GetVertices()[iVertex * 3 + 2]);
                    }

                    for (uint32_t iIndex = 0; iIndex < indexCount; ++iIndex)
                    {
                        indices.PushBack(hull->GetIndices()[iIndex]);
                    }
                }


                // Now we write the data.
                GTCore::IO::Write(file, &vertexCounts[0], vertexCounts.count * 4);
                GTCore::IO::Write(file, &indexCounts[0],  indexCounts.count  * 4);
                
                uint32_t totalVertices = static_cast<uint32_t>(vertices.count);
                uint32_t totalIndices  = static_cast<uint32_t>(indices.count);

                GTCore::IO::Write(file, &totalVertices, 4);
                GTCore::IO::Write(file, &totalIndices,  4);

                GTCore::IO::Write(file, &vertices[0],     vertices.count * 4);
                GTCore::IO::Write(file, &indices[0],      indices.count  * 4);
            }

            // We need to write the build settings.
            uint32_t chbsSize = sizeof(definition.convexHullBuildSettings);
            GTCore::IO::Write(file, &chbsSize, 4);
            GTCore::IO::Write(file, &definition.convexHullBuildSettings, chbsSize);
        }

        return true;
    }


    bool ModelLibrary::LoadGTMODELMetadata(const char* fileNameIn, ModelDefinition &definition)
    {
        auto file = GTCore::IO::Open(fileNameIn, GTCore::IO::OpenMode::Read);
        if (file != nullptr)
        {
            // The first thing we load is the header, which is 8 bytes. The first 4 bytes should equal "gtem".
            struct
            {
                char     id[4];
                uint32_t version;
            }header;
            GTCore::IO::Read(file, &header, 8);

            // We need to check the first 4 bytes to ensure they read "gtem".
            if (header.id[0] == 'g' && header.id[1] == 't' && header.id[2] == 'e' && header.id[3] == 'm')
            {
                // We load differently depending on the version. Unknown version means return null.
                switch (header.version)
                {
                case 1:
                    {
                        // The counts of the main objects. We batch these into a single call at the top of everything.
                        struct
                        {
                            uint32_t boneCount;
                            uint32_t meshCount;
                            uint32_t keyFrameCount;
                        }counts;
                        GTCore::IO::Read(file, &counts, 12);


                        // Bones.
                        //
                        // Bones are first because they are referenced by everything else.
                        for (uint32_t iBone = 0; iBone < counts.boneCount; ++iBone)
                        {
                            // Name.
                            uint32_t nameLength;
                            GTCore::IO::Read(file, &nameLength, 4);
                            GTCore::IO::Seek(file, nameLength, GTCore::IO::SeekOrigin::Current);


                            // Relative transform and offset matrix.
                            GTCore::IO::Seek(file, 4 * 3,     GTCore::IO::SeekOrigin::Current);        // Position
                            GTCore::IO::Seek(file, 4 * 4,     GTCore::IO::SeekOrigin::Current);        // Rotation
                            GTCore::IO::Seek(file, 4 * 3,     GTCore::IO::SeekOrigin::Current);        // Scale
                            GTCore::IO::Seek(file, 4 * 4 * 4, GTCore::IO::SeekOrigin::Current);        // Offset Matrix
                        }

                        // Bone parents.
                        GTCore::IO::Seek(file, counts.boneCount * 4, GTCore::IO::SeekOrigin::Current);


                        // Meshes.
                        for (uint32_t i = 0; i < counts.meshCount; ++i)
                        {
                            // Geometry.
                            {
                                // Vertices.
                                uint32_t vertexCount;
                                GTCore::IO::Read(file, &vertexCount, 4);

                                if (vertexCount > 0)
                                {
                                    GTCore::IO::Seek(file, vertexCount * VertexFormat::P3T2N3T3B3.GetSize() * sizeof(float), GTCore::IO::SeekOrigin::Current);
                                }


                                // Indices.
                                uint32_t indexCount;
                                GTCore::IO::Read(file, &indexCount, 4);
                                
                                if (indexCount > 0)
                                {
                                    GTCore::IO::Seek(file, indexCount * sizeof(unsigned int), GTCore::IO::SeekOrigin::Current);
                                }
                            }

                                
                            // Materials
                            {
                                uint32_t length;
                                GTCore::IO::Read(file, &length, 4);
                                GTCore::IO::Seek(file, length, GTCore::IO::SeekOrigin::Current);
                            }


                            // Skinning vertex attributes.
                            {
                                uint32_t skinningVertexAttributeCount;
                                GTCore::IO::Read(file, &skinningVertexAttributeCount, 4);

                                if (skinningVertexAttributeCount > 0)
                                {
                                    GTCore::IO::Seek(file, skinningVertexAttributeCount * 2, GTCore::IO::SeekOrigin::Current);

                                    uint32_t totalBoneWeights;
                                    GTCore::IO::Read(file, &totalBoneWeights, 4);
                                    GTCore::IO::Seek(file, totalBoneWeights * 8, GTCore::IO::SeekOrigin::Current);
                                }
                            }
                        }


                        // Now we need to load the key frame animation data.
                        for (uint32_t iFrame = 0; iFrame < counts.keyFrameCount; ++iFrame)
                        {
                            float time;
                            GTCore::IO::Read(file, &time, 4);

                            // With the key frame added, we now need to iterate over each channel in the key frame.
                            uint32_t channelCount;
                            GTCore::IO::Read(file, &channelCount, 4);

                            for (uint32_t iChannel = 0; iChannel < channelCount; ++iChannel)
                            {
                                // Bone Index
                                GTCore::IO::Seek(file, 4, GTCore::IO::SeekOrigin::Current);

                                // Transformation.
                                GTCore::IO::Seek(file, 4 * 3, GTCore::IO::SeekOrigin::Current);    // Position
                                GTCore::IO::Seek(file, 4 * 4, GTCore::IO::SeekOrigin::Current);    // Rotation
                                GTCore::IO::Seek(file, 4 * 3, GTCore::IO::SeekOrigin::Current);    // Scale
                            }
                        }


                        // Here is the engine-specific metadata.
                        ModelLibrary::LoadGTMODELMetadata(file, definition);
                            

                        // Can't forget to close the file.
                        GTCore::IO::Close(file);


                        break;
                    }

                default:
                    {
                        GTCore::IO::Close(file);

                        GTEngine::PostError("Can not load .gtmodel file: Unknown version.");
                        return nullptr;
                    }
                }
            }
            else
            {
                GTCore::IO::Close(file);

                GTEngine::PostError("Can not load .gtmodel file: Invalid header ID.");
                return nullptr;
            }
        }

        return true;
    }

    bool ModelLibrary::LoadDefaultMetadata(ModelDefinition &definition)
    {
        // The first thing is the materials.
        if (definition.meshMaterials.count == 0)
        {
            for (size_t i = 0; i < definition.meshGeometries.count; ++i)
            {
                definition.meshMaterials.PushBack(nullptr);
            }
        }

        for (size_t iMaterial = 0; iMaterial < definition.meshMaterials.count; ++iMaterial)
        {
            if (definition.meshMaterials[iMaterial] == nullptr)
            {
                definition.meshMaterials[iMaterial] = MaterialLibrary::Create("engine/materials/simple-diffuse.material");
            }
        }

        return true;
    }


    ModelDefinition* ModelLibrary::FindDefinition(const char* name)
    {
        // If we don't find the definition from the original file and the extension is 'gtmodel', we'll strip the extension
        // and try again.

        auto iDefinition = LoadedDefinitions.Find(name);
        if (iDefinition == nullptr)
        {
            if (GTCore::Path::ExtensionEqual(name, "gtmodel"))
            {
                GTCore::String gtmodelName = GTCore::IO::RemoveExtension(name);
                iDefinition = LoadedDefinitions.Find(gtmodelName.c_str());
            }
            else
            {
                GTCore::String baseName(name);
                baseName += ".gtmodel";

                iDefinition = LoadedDefinitions.Find(baseName.c_str());
            }
        }

        if (iDefinition != nullptr)
        {
            return iDefinition->value;
        }

        return nullptr;
    }
}





// These are private functions implementations for ModelLibrary.
namespace GTEngine
{
    Model* ModelLibrary_CreateFromPrimitive(const char* name, VertexArray* va)
    {
        ModelDefinition* definition = nullptr;

        // We first need to retrieve our model info.
        auto iModelInfo = LoadedDefinitions.Find(name);
        if (iModelInfo == nullptr)
        {
            definition = new ModelDefinition;
            definition->meshGeometries.PushBack(va);
            definition->meshMaterials.PushBack(MaterialLibrary::Create("engine/materials/simple-diffuse.material"));
            definition->meshSkinningVertexAttributes.PushBack(nullptr);

            LoadedDefinitions.Add(name, definition);
            LoadedModels.Add(definition, new GTCore::Vector<Model*>);
        }
        else
        {
            definition = iModelInfo->value;
        }


        // Now that we have the model information we can create a model from it.
        if (definition != nullptr)
        {
            return ModelLibrary::CreateFromDefinition(*definition);
        }

        return nullptr;
    }
}
