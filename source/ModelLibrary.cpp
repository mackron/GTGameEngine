
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/VertexArrayFactory.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>

#include <GTCore/Path.hpp>
#include <GTCore/Timing.hpp>

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

    /// The list of loaded models. These will be deleted when the model library is shutdown.
    static GTCore::List<Model*> LoadedModels;


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
        // We need to unload all models.
        while (LoadedModels.root != nullptr)
        {
            delete LoadedModels.root->value;
            LoadedModels.RemoveRoot();
        }

        // We unload the definitions after the models.
        for (size_t i = 0; i < LoadedDefinitions.count; ++i)
        {
            delete LoadedDefinitions.buffer[i]->value;
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


    void CopyNodesWithMeshes(const aiScene &scene, const aiNode &node, const aiMatrix4x4 &accumulatedTransform, ModelDefinition &definition)
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

                definition.meshBones.PushBack(localBones);
            }
            else
            {
                definition.meshBones.PushBack(nullptr);
            }
        }

        // Now we need to iterate over the children and copy them.
        for (unsigned int iChild = 0; iChild < node.mNumChildren; ++iChild)
        {
            auto child = node.mChildren[iChild];
            assert(child != nullptr);

            CopyNodesWithMeshes(scene, *child, transform, definition);
        }


        // With the meshes created, we can now generate the tangents+bitangents.
        definition.GenerateTangentsAndBitangents();
    }

    /// This function creates a new model definition from the given assimp scene.
    ///
    /// @remarks
    ///     The returned structure is allocated with 'new'.
    ModelDefinition* ModelLibrary_LoadFromAssimpScene(const aiScene &scene)
    {
        // We need to recursively read each scene node and attach the meshes where possible.
        auto root = scene.mRootNode;
        if (root != nullptr)
        {
            auto definition = new ModelDefinition;

            // This is where we take the assimp meshes and create the GTEngine meshes.
            aiMatrix4x4 transform;
            CopyNodesWithMeshes(scene, *root, transform, *definition);


            // Here is where we load up the animations. Assimp has multiple animations, but GTEngine uses only a single animation. To
            // resolve, we simply copy over each animation into the main animation and create a named segment for that animation.
            double segmentStartTime = 0.0;
            
            for (auto iAnimation = 0U; iAnimation < scene.mNumAnimations; ++iAnimation)
            {
                auto animation = scene.mAnimations[iAnimation];
                assert(animation != nullptr);

                // The starting keyframe will be equal to the number of keyframes in the animation at this point.
                size_t startKeyFrame = definition->animation.GetKeyFrameCount();

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

                    auto iBone = definition->bones.Find(channel->mNodeName.C_Str());
                    if (iBone == nullptr)
                    {
                        auto newNode = FindNodeByName(scene, channel->mNodeName);
                        assert(newNode != nullptr);

                        bone = AddBone(*newNode, *definition);
                    }
                    else
                    {
                        bone = iBone->value;
                    }

                    assert(bone != nullptr);

                    // TODO: Check that this assertion is valid. If not, we need to combine them all into a single list.
                    assert(channel->mNumPositionKeys == channel->mNumRotationKeys && channel->mNumPositionKeys == channel->mNumScalingKeys);



                    // Now we create the channel.
                    auto &newChannel = definition->animation.CreateChannel();
                    definition->MapBoneToAnimationChannel(newChannel, *bone);

                    // Here is where we add the key frames. Since we are looping over the channels, each key frame will probably be creating twice. This is OK because
                    // Animation will make sure there are no duplicate key frames.
                    for (unsigned int iKey = 0; iKey < channel->mNumPositionKeys; ++iKey)
                    {
                        auto &positionKey = channel->mPositionKeys[iKey];
                        auto &rotationKey = channel->mRotationKeys[iKey];
                        auto &scaleKey    = channel->mScalingKeys[iKey];

                        size_t keyFrameIndex = definition->animation.AppendKeyFrame(segmentStartTime + positionKey.mTime);

                        auto key = definition->CreateAnimationKey(AssimpToGLM(positionKey.mValue), AssimpToGLM(rotationKey.mValue), AssimpToGLM(scaleKey.mValue));
                        newChannel.SetKey(keyFrameIndex, key);
                    }
                }
                


                // At this point we can now create the named segment.
                definition->animation.AddNamedSegment(animation->mName.C_Str(), startKeyFrame, definition->animation.GetKeyFrameCount());

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

            return definition;
        }

        return nullptr;
    }




    Model* ModelLibrary::LoadFromFile(const char* fileName)
    {
        // We first need to check if we're loading a .gtmodel. If so, we branch off and do that separate.
        if (GTCore::Path::ExtensionEqual(fileName, "gtmodel"))
        {
            return ModelLibrary::LoadFromGTMODEL(fileName);
        }
        else
        {
            ModelDefinition* definition = nullptr;

            // If the file is already loaded, we don't want to reload. Instead we create a new instance of the model using the existing information.
            auto iDefinition = LoadedDefinitions.Find(fileName);
            if (iDefinition == nullptr)
            {
                Assimp::Importer importer;
                importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, AssimpRemovedComponentsFlags);

                auto scene = importer.ReadFile(fileName, AssimpReadFileFlags);
                if (scene != nullptr)
                {
                    definition = ModelLibrary_LoadFromAssimpScene(*scene);
                    if (definition != nullptr)
                    {
                        // The definition was loaded from a file, so we need to mark it as so.
                        definition->fileName = fileName;

                        // When we get here we will have a LoadedModelInfo object which needs to be added to the global list.
                        LoadedDefinitions.Add(fileName, definition);
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
                definition = iDefinition->value;
            }

        
            // Now that we have information about the model, we can create a new Model object and return it.
            if (definition != nullptr)
            {
                return ModelLibrary::CreateFromDefinition(*definition);
            }


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
                definition = ModelLibrary_LoadFromAssimpScene(*scene);
                if (definition != nullptr)
                {
                    // When we get here we will have a LoadedModelInfo object which needs to be added to the global list.
                    LoadedDefinitions.Add(nffFileName.c_str(), definition);
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
            definition = iDefinition->value;
        }

        
        // Now that we have information about the model, we can create a new Model object and return it.
        if (definition != nullptr)
        {
            return ModelLibrary::CreateFromDefinition(*definition);
        }


        return nullptr;
    }

    Model* ModelLibrary::LoadFromGTMODEL(const char* fileNameIn)
    {
        ModelDefinition* definition = nullptr;

        auto iDefinition = LoadedDefinitions.Find(fileNameIn);
        if (iDefinition == nullptr)
        {
            auto file = GTCore::IO::Open(fileNameIn, GTCore::IO::OpenMode::Read);
            if (file != nullptr)
            {
                // We need to check the first 4 bytes to ensure they read "gtem".
                char id[4];
                GTCore::IO::Read(file, id, 4);

                if (id[0] == 'g' && id[1] == 't' && id[2] == 'e' && id[3] == 'm')
                {
                    uint32_t version;
                    GTCore::IO::Read(file, &version, 4);

                    // We load differently depending on the version. Unknown version means return null.
                    switch (version)
                    {
                    case 1:
                        {
                            definition = new ModelDefinition(fileNameIn);

                            // Bones.
                            //
                            // Bones are first because they are referenced by everything else. We store a bone count
                            //uint32_t boneCount;



                            // Meshes.
                            //
                            // Meshes reference bones, so we do these afterwards.
                            uint32_t meshCount;
                            GTCore::IO::Read(file, &meshCount, 4);

                            for (uint32_t i = 0; i < meshCount; ++i)
                            {
                                // Geometry.
                                {
                                    auto va = new VertexArray(VertexArrayUsage_Static, VertexFormat::P3T2N3T3B3);

                                    // Vertices.
                                    uint32_t vertexCount;
                                    GTCore::IO::Read(file, &vertexCount, 4);

                                    va->SetVertexData(nullptr, static_cast<size_t>(vertexCount));
                                    auto vertexData = va->MapVertexData();
                                    {
                                        GTCore::IO::Read(file, vertexData, vertexCount * VertexFormat::P3T2N3T3B3.GetSize() * sizeof(float));
                                    }
                                    va->UnmapVertexData();


                                    // Indices.
                                    uint32_t indexCount;
                                    GTCore::IO::Read(file, &indexCount, 4);
                                
                                    va->SetIndexData(nullptr, static_cast<size_t>(indexCount));
                                    auto indexData = va->MapIndexData();
                                    {
                                        GTCore::IO::Read(file, indexData, indexCount * sizeof(unsigned int));
                                    }
                                    va->UnmapIndexData();

                                
                                    definition->meshGeometries.PushBack(va);
                                }

                                
                                // Materials
                                {
                                    uint32_t length;
                                    GTCore::IO::Read(file, &length, 4);

                                    if (length > 0)
                                    {
                                        auto materialFileName = static_cast<char*>(malloc(length + 1));                         // <-- +1 for null terminator. Not included in 'length'.
                                        GTCore::IO::Read(file, materialFileName, length); materialFileName[length] = '\0';

                                        definition->meshMaterials.PushBack(MaterialLibrary::Create(materialFileName));

                                        free(materialFileName);
                                    }
                                    else
                                    {
                                        definition->meshMaterials.PushBack(MaterialLibrary::Create("engine/materials/simple-diffuse.material"));
                                    }
                                }


                                // Bones
                                {
                                    uint32_t boneCount;
                                    GTCore::IO::Read(file, &boneCount, 4);

                                    if (boneCount > 0)
                                    {
                                        auto boneWeightList = new GTCore::Vector<BoneWeights*>(boneCount);
                                        for (uint32_t iBone = 0; iBone < boneCount; ++iBone)
                                        {
                                            // First is the name.
                                            uint32_t nameLength;
                                            GTCore::IO::Read(file, &nameLength, 4);

                                            auto name = static_cast<char*>(malloc(nameLength + 1));                 // <-- +1 for null terminator.
                                            GTCore::IO::Read(file, name, nameLength); name[nameLength] = '\0';

                                            auto boneWeights = new BoneWeights(name);
                                            free(name);


                                            // Now we need to load the vertex/weight pairs. We do this in one go.
                                            uint32_t weightCount;
                                            GTCore::IO::Read(file, &weightCount, 4);

                                            boneWeights->weights.Resize(weightCount);
                                            GTCore::IO::Read(file, boneWeights->weights.buffer, weightCount * sizeof(VertexWeightPair));


                                            boneWeightList->PushBack(boneWeights);
                                        }

                                        definition->meshBones.PushBack(boneWeightList);
                                    }
                                    else
                                    {
                                        definition->meshBones.PushBack(nullptr);
                                    }
                                }
                            }


                            // Here is where we read the bone hierarchy. These will not be written in any set order, so we need to do a second pass which will
                            // do the actual linking based on parent names. To do this, we need to map bone names with parent names. The key will be the bone
                            // pointer, and the value will be the parent name.
                            GTCore::Map<Bone*, GTCore::String> boneParentMap;

                            // First is the bone count.
                            uint32_t boneCount;
                            GTCore::IO::Read(file, &boneCount, 4);

                            // Each bone has a name, parent name, transformation and an offset matrix.
                            for (uint32_t iBone = 0; iBone < boneCount; ++iBone)
                            {
                                // Name.
                                uint32_t nameLength;
                                GTCore::IO::Read(file, &nameLength, 4);

                                auto name = static_cast<char*>(malloc(nameLength + 1));                 // <-- +1 for null terminator.
                                GTCore::IO::Read(file, name, nameLength); name[nameLength] = '\0';


                                // Parent name.
                                uint32_t parentNameLength;
                                GTCore::IO::Read(file, &parentNameLength, 4);

                                char* parentName = nullptr;
                                if (parentNameLength > 0)
                                {
                                    parentName = static_cast<char*>(malloc(parentNameLength + 1));                                  // <-- +1 for null terminator.
                                    GTCore::IO::Read(file, parentName, parentNameLength); parentName[parentNameLength] = '\0';
                                }



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
                                definition->bones.Add(name, bone);


                                // In order to do the parent linkage, we need to map a bone with a parent name.
                                boneParentMap.Add(bone, GTCore::String(parentName));


                                free(name);
                                free(parentName);
                            }

                            // This is the second pass for bone loading where we link the bones with their parents. We also create the channels here. There are two maps
                            // to concern ourselves with here. The first is the map in the model definition which maps a channel to a bone. The other is a local map where
                            // we map a bone name to a channel. This local map is used below for loading key frame data.
                            GTCore::Dictionary<AnimationChannel*> boneChannelMap;

                            for (size_t iBone = 0; iBone < boneParentMap.count; ++iBone)
                            {
                                auto bone       = boneParentMap.buffer[iBone]->key;
                                auto parentName = boneParentMap.buffer[iBone]->value;

                                assert(bone != nullptr);


                                auto parent = definition->bones.Find(parentName.c_str());
                                if (parent != nullptr)
                                {
                                    parent->value->AttachChild(*bone);
                                }


                                // We need to create a channel for this bone. We then need to map that channel to a bone.
                                auto &channel = definition->animation.CreateChannel();
                                definition->animationChannelBones.Add(&channel, bone);

                                boneChannelMap.Add(bone->GetName(), &channel);
                            }


                            // At this point all of the bones will be linked together. Now we need to load the key frame animation data.
                            uint32_t keyFrameCount;
                            GTCore::IO::Read(file, &keyFrameCount, 4);

                            for (uint32_t iFrame = 0; iFrame < keyFrameCount; ++iFrame)
                            {
                                float time;
                                GTCore::IO::Read(file, &time, 4);

                                size_t keyFrameIndex = definition->animation.AppendKeyFrame(static_cast<double>(time));

                                // With the key frame added, we now need to iterate over each channel in the key frame.
                                uint32_t channelCount;
                                GTCore::IO::Read(file, &channelCount, 4);

                                for (uint32_t iChannel = 0; iChannel < channelCount; ++iChannel)
                                {
                                    // The first attribute is the bone name.
                                    uint32_t boneNameLength;
                                    GTCore::IO::Read(file, &boneNameLength, 4);

                                    auto boneName = static_cast<char*>(malloc(boneNameLength + 1));                         // +1 for null terminator.
                                    GTCore::IO::Read(file, boneName, boneNameLength); boneName[boneNameLength] = '\0';

                                    auto channel = boneChannelMap.Find(boneName);
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
                                    definition->animationKeyCache.PushBack(key);


                                    free(boneName);
                                }
                            }


                            // We can't forget to add the definition to the global list.
                            definition->fileName = fileNameIn;
                            LoadedDefinitions.Add(fileNameIn, definition);

                            
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
            else
            {
                GTEngine::PostError("Can not open file: %s.", fileNameIn);
                return nullptr;
            }
        }
        else
        {
            definition = iDefinition->value;
        }


        if (definition != nullptr)
        {
            return ModelLibrary::CreateFromDefinition(*definition);
        }

        return nullptr;
    }


    bool ModelLibrary::WriteToFile(const Model &model, const char* fileNameIn)
    {
        auto &definition = model.GetDefinition();

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


            // First we save the base mesh data. We first need to save the mesh count.
            uint32_t meshCount = static_cast<uint32_t>(definition.meshGeometries.count);
            GTCore::IO::Write(file, &meshCount, 4);

            // Now we iterate over each mesh and save the data. The vertex data is always saved as P3T2N3T3B3 format.
            for (size_t i = 0; i < definition.meshGeometries.count; ++i)
            {
                // Geometry
                auto va = definition.meshGeometries[i];
                assert(va != nullptr);

                // First is the vertex data.
                uint32_t vertexCount = static_cast<uint32_t>(va->GetVertexCount());
                GTCore::IO::Write(file, &vertexCount, 4);
                GTCore::IO::Write(file, va->GetVertexDataPtr(), sizeof(float) * va->GetFormat().GetSize() * vertexCount);

                // Now the index data.
                uint32_t indexCount = static_cast<uint32_t>(va->GetIndexCount());
                GTCore::IO::Write(file, &indexCount, 4);
                GTCore::IO::Write(file, va->GetIndexDataPtr(), sizeof(unsigned int) * indexCount);


                // Material. Note the lack of null terminators. Materials are saved based on the input model, and not the definition. The reason is, is that the definition
                // may not have up-to-date material information, as opposed to the mesh.
                auto material = model.meshes[i]->GetMaterial();
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


                // Bones. Start with the count.
                uint32_t boneCount = static_cast<uint32_t>(definition.meshBones[i]->count);
                GTCore::IO::Write(file, &boneCount, 4);

                // Now we iterate over each bone and save the name of the bone and the vertex/weight pairs.
                for (uint32_t j = 0; j < boneCount; ++j)
                {
                    auto weights = definition.meshBones[i]->buffer[j];
                    assert(weights != nullptr);

                    uint32_t nameLength = static_cast<uint32_t>(weights->name.GetLength());
                    GTCore::IO::Write(file, &nameLength, 4);
                    GTCore::IO::Write(file, weights->name.c_str(), static_cast<size_t>(nameLength));

                    // Here we write the vertex/weight pairs for this bone on this mesh.
                    uint32_t weightCount = static_cast<uint32_t>(weights->weights.count);
                    GTCore::IO::Write(file, &weightCount, 4);
                    GTCore::IO::Write(file, weights->weights.buffer, static_cast<size_t>(weightCount) * sizeof(VertexWeightPair));
                }
            }


            // Now we save the bone hierarchy. This is a simple system where we simple write the name of the bone, and the name of the parent bone, for each bone.
            uint32_t boneCount = static_cast<uint32_t>(definition.bones.count);
            GTCore::IO::Write(file, &boneCount, 4);

            // Now we iterate over each bone. The transformation is relative to the parent.
            for (uint32_t i = 0; i < boneCount; ++i)
            {
                auto bone = definition.bones.buffer[i]->value;
                assert(bone != nullptr);

                // Name.
                uint32_t nameLength = GTCore::Strings::SizeInBytes(bone->GetName());
                GTCore::IO::Write(file, &nameLength, 4);
                GTCore::IO::Write(file, bone->GetName(), nameLength);

                // Parent Name.
                if (bone->GetParent() != nullptr)
                {
                    uint32_t parentNameLength = GTCore::Strings::SizeInBytes(bone->GetParent()->GetName());
                    GTCore::IO::Write(file, &parentNameLength, 4);
                    GTCore::IO::Write(file, bone->GetParent()->GetName(), parentNameLength);
                }
                else
                {
                    uint32_t tmp = 0;
                    GTCore::IO::Write(file, &tmp, 4);
                }


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


            // Now we need to write the animation keys. There a channel for every bone at every key frame. The data is the transformation of the bone.
            // We loop over each key, and then save the bone transformation at every channel on that key.
            uint32_t keyFrameCount = definition.animation.GetKeyFrameCount();
            GTCore::IO::Write(file, &keyFrameCount, 4);

            // Now we need to loop over each key frame and write all of the data for that frame.
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

                    auto channel = jChannel->key;
                    assert(channel != nullptr);

                    auto bone = jChannel->value;
                    assert(bone != nullptr);


                    // We need to save the name of the bone that this channel is referring to.
                    uint32_t boneNameLength = GTCore::Strings::SizeInBytes(bone->GetName());
                    GTCore::IO::Write(file, &boneNameLength, 4);
                    GTCore::IO::Write(file, bone->GetName(), boneNameLength);
                    
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



            // Can't forget to close the file.
            GTCore::IO::Close(file);
        }

        return false;
    }









    ////////////////////////////////////////////////////////////////////////
    // Create functions.

    Model* ModelLibrary::CreateFromDefinition(const ModelDefinition &definition)
    {
        auto model = new Model(definition);
        LoadedModels.Append(model);

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


    void ModelLibrary::Delete(Model* model)
    {
        LoadedModels.Remove(LoadedModels.Find(model));
        delete model;
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
            definition->meshMaterials.PushBack(nullptr);
            definition->meshBones.PushBack(nullptr);

            LoadedDefinitions.Add(name, definition);
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
