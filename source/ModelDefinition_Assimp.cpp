// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

// This file is where anything requiring Assimp is located. It is forbidden to place any Assimp code anywhere other
// than inside this file. FORBIDDEN, I TELL YOU!

#include <GTEngine/ModelDefinition.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Errors.hpp>

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
#endif

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

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


    /////////////////////////////////////
    // Assimp -> GLM Conversions

    glm::mat4 mat4_cast(const aiMatrix4x4 &value)
    {
        // Assimp matrix = row-major. GLM matrix = column-major. Sigh.
        glm::mat4 result;
        result[0] = glm::vec4(value[0][0], value[1][0], value[2][0], value[3][0]);
        result[1] = glm::vec4(value[0][1], value[1][1], value[2][1], value[3][1]);
        result[2] = glm::vec4(value[0][2], value[1][2], value[2][2], value[3][2]);
        result[3] = glm::vec4(value[0][3], value[1][3], value[2][3], value[3][3]);

        return result;
    }

    glm::vec3 vec3_cast(const aiVector3D &value)
    {
        return glm::vec3(value.x, value.y, value.z);
    }

    glm::quat quat_cast(const aiQuaternion &value)
    {
        return glm::quat(value.w, value.x, value.y, value.z);
    }


    /////////////////////////////////////
    // Assimp Helpers

    void ConvertBone(const aiBone &inputBone, Bone &outputBone)
    {
        aiVector3D   scale(1.0f, 1.0f, 1.0f);
        aiQuaternion rotation;
        aiVector3D   position;
        inputBone.mOffsetMatrix.Decompose(scale, rotation, position);

        outputBone.SetOffsetMatrix(
            glm::translate(position.x, position.y, position.z) *
            glm::mat4_cast(quat_cast(rotation) * glm::angleAxis(90.0f, glm::vec3(1.0f, 0.0f, 0.0f))) *        // <-- TODO: Check if this rotation is a bug in Assimp or Blender.
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

        newBone->SetPosition(vec3_cast(position));
        newBone->SetRotation(quat_cast(rotation));
        newBone->SetScale(vec3_cast(scale));

        return newBone;
    }

    Bone* CreateBone(const aiNode &inputNode, const aiBone &inputBone)
    {
        auto newBone = CreateEmptyBone(inputNode);
        assert(newBone != nullptr);

        ConvertBone(inputBone, *newBone);

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

            ConvertBone(bone, *newBone);

            return newBone;
        }
    }



    void CopyNodesWithMeshes(const aiScene &scene, const aiNode &node, const aiMatrix4x4 &accumulatedTransform, ModelDefinition &definition, GTCore::Vector<GTCore::Vector<BoneWeights*>*> &meshBones)
    {
        // First we need to grab the transformation to apply to the mesh.
        aiMatrix4x4 transform = accumulatedTransform * node.mTransformation;
        aiMatrix3x3 normalTransform(transform);
        normalTransform.Transpose();
        normalTransform.Inverse();

        
        // If the scale has a negative component we need to reverse the polygon winding.
        aiVector3t<float>    scale;
        aiQuaterniont<float> rotation;
        aiVector3t<float>    position;
        node.mTransformation.Decompose(scale, rotation, position);

        bool useReverseWinding = scale.x < 0.0f || scale.y < 0.0f || scale.z < 0.0f;



        // Here we need to loop over each mesh and add it to the model. The mesh needs to be transformed based on the transformation.
        for (unsigned int iMesh = 0; iMesh < node.mNumMeshes; ++iMesh)
        {
            auto mesh = scene.mMeshes[node.mMeshes[iMesh]];
            assert(mesh != nullptr);

            auto positions = mesh->mVertices;
            auto normals   = mesh->mNormals;
            auto texCoords = mesh->mTextureCoords;

            auto format = VertexFormat::P3T2N3T3B3;
            auto va     = Renderer::CreateVertexArray(VertexArrayUsage_Static, format);

            // For now, only support triangle formats.
            if (mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE)
            {
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

                    if (!useReverseWinding)
                    {
                        indexDst[0] = face.mIndices[0];
                        indexDst[1] = face.mIndices[1];
                        indexDst[2] = face.mIndices[2];
                    }
                    else
                    {
                        indexDst[0] = face.mIndices[2];
                        indexDst[1] = face.mIndices[1];
                        indexDst[2] = face.mIndices[0];
                    }
                }

                va->UnmapVertexData();
                va->UnmapIndexData();
            }

            definition.meshGeometries.PushBack(va);


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



    bool ModelDefinition::LoadFromAssimpFile(const GTCore::String &absolutePathIn)
    {
        Assimp::Importer importer;
        importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, AssimpRemovedComponentsFlags);

        auto scene = importer.ReadFile(absolutePathIn.c_str(), AssimpReadFileFlags);
        if (scene != nullptr)
        {
            auto root = scene->mRootNode;
            if (root != nullptr)
            {
                GTCore::Vector<GTCore::Vector<BoneWeights*>*> meshBones;

                size_t oldMeshCount = this->meshGeometries.count;

                // At this point we're going to be re-creating the skinning and animation data. These need clearing.
                this->ClearMeshGeometries();
                this->ClearMeshSkinningVertexAttributes();
                this->ClearBones();
                this->ClearAnimations();

                // This is where we take the assimp meshes and create the GTEngine meshes.
                aiMatrix4x4 transform;
                CopyNodesWithMeshes(*scene, *root, transform, *this, meshBones);

                // We need to perform a post-process step of sorts on each mesh. Here we sort out the materials and skinning vertex attributes. It's important that
                // we do this after creating the local bones of the mesh so that we get the correct indices.
                //
                // If the number of meshes is different to the old one, we want to reset materials. Otherwise, we leave the materials alone.
                bool resetMaterials = oldMeshCount != this->meshGeometries.count;
                if (resetMaterials)
                {
                    this->ClearMaterials();
                }

                for (size_t i = 0; i < this->meshGeometries.count; ++i)
                {
                    // Material.
                    if (resetMaterials)
                    {
                        this->meshMaterials.PushBack(MaterialLibrary::Create("engine/materials/simple-diffuse.material"));
                    }


                    // Skinning Vertex Attributes.
                    auto localBones = meshBones[i];
                    if (localBones != nullptr && this->meshGeometries[i]->GetVertexCount() > 0)
                    {
                        auto skinningVertexAttributes = new SkinningVertexAttribute[this->meshGeometries[i]->GetVertexCount()];

                        for (size_t j = 0; j < localBones->count; ++j)
                        {
                            auto bone = localBones->buffer[j];
                            assert(bone != nullptr);

                            size_t boneIndex = this->bones.Find(bone->name.c_str())->index;

                            for (size_t k = 0; k < bone->weights.count; ++k)
                            {
                                skinningVertexAttributes[bone->weights[k].vertexID].AddBoneWeightPair(boneIndex, bone->weights[k].weight);
                            }
                        }

                        this->meshSkinningVertexAttributes.PushBack(skinningVertexAttributes);
                    }
                    else
                    {
                        this->meshSkinningVertexAttributes.PushBack(nullptr);
                    }
                }



                // Here is where we load up the animations. Assimp has multiple animations, but GTEngine uses only a single animation. To
                // resolve, we simply copy over each animation into the main animation and create a named segment for that animation.
                double segmentStartTime = 0.0;

                for (auto iAnimation = 0U; iAnimation < scene->mNumAnimations; ++iAnimation)
                {
                    auto animation = scene->mAnimations[iAnimation];
                    assert(animation != nullptr);

                    // The starting keyframe will be equal to the number of keyframes in the animation at this point.
                    size_t startKeyFrame = this->animation.GetKeyFrameCount();

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

                        auto iBone = this->bones.Find(channel->mNodeName.C_Str());
                        if (iBone == nullptr)
                        {
                            auto newNode = FindNodeByName(*scene, channel->mNodeName);
                            assert(newNode != nullptr);

                            bone = AddBone(*newNode, *this);
                        }
                        else
                        {
                            bone = iBone->value;
                        }

                        assert(bone != nullptr);

                        // TODO: Check that this assertion is valid. If not, we need to combine them all into a single list.
                        assert(channel->mNumPositionKeys == channel->mNumRotationKeys && channel->mNumPositionKeys == channel->mNumScalingKeys);



                        // Now we create the channel.
                        auto &newChannel = this->animation.CreateChannel();
                        this->MapAnimationChannelToBone(*bone, newChannel);

                        // Here is where we add the key frames. Since we are looping over the channels, each key frame will probably be creating twice. This is OK because
                        // Animation will make sure there are no duplicate key frames.
                        for (unsigned int iKey = 0; iKey < channel->mNumPositionKeys; ++iKey)
                        {
                            auto &positionKey = channel->mPositionKeys[iKey];
                            auto &rotationKey = channel->mRotationKeys[iKey];
                            auto &scaleKey    = channel->mScalingKeys[iKey];

                            size_t keyFrameIndex = this->animation.AppendKeyFrame(segmentStartTime + positionKey.mTime);

                            auto key = this->CreateAnimationKey(vec3_cast(positionKey.mValue), quat_cast(rotationKey.mValue), vec3_cast(scaleKey.mValue));
                            newChannel.SetKey(keyFrameIndex, key);
                        }
                    }



                    // At this point we can now create the named segment.
                    this->animation.AddNamedSegment(animation->mName.C_Str(), startKeyFrame, this->animation.GetKeyFrameCount());

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
            else
            {
                GTEngine::PostError("Error creating model info for %s: %s", absolutePath.c_str(), importer.GetErrorString());
                return false;
            }
        }
        else
        {
            GTEngine::PostError("Error importing %s: %s", absolutePath.c_str(), importer.GetErrorString());
            return false;
        }
    }
}