
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/VertexArrayFactory.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>

#include <GTCore/Path.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Globals.
namespace GTEngine
{
    /// Structure containing information about a loaded model.
    struct LoadedModelInfo
    {
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
        }

        /// The list of vertex arrays containing the geometric data of each mesh.
        GTCore::Vector<VertexArray*> geometries;

        /// The default materials. There will always be an equal number of materials as there are meshes. Each material in this list
        /// has a one-to-one correspondance with a mesh in <meshes>. For example, the material at index 0 is the material to use with
        /// the mesh at index 0.
        GTCore::Vector<Material*> defaultMaterials;

        /// The list of skeletons. There will be one skeleton pointer for each mesh. If the mesh does not have a skeleton, the pointer
        /// will be set to null.
        //GTCore::Vector<Skeleton*> defaultSkeletons;
    };

    /// The list of loaded models.
    GTCore::Dictionary<LoadedModelInfo*> LoadedModels;
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
    const aiNode* FindNodeByName(const aiScene &scene, const aiNode &node, const aiString &name, unsigned int &depth)
    {
        if (node.mName == name)
        {
            return &node;
        }

        // Now we need to increment the depth.
        ++depth;

        // We need to keep track of this so we can restore it for each child iteration.
        auto childrenDepth = depth;


        // We'll get here if the node was not found. We need to check the children.
        for (unsigned int iChild = 0; iChild < node.mNumChildren; ++iChild)
        {
            auto child = node.mChildren[iChild];
            assert(child != nullptr);

            auto result = FindNodeByName(scene, *child, name, depth);
            if (result != nullptr)
            {
                // We'll get here if the child found the node. In this case, we just return that node.
                return result;
            }

            // With a new child iteration, the depth will need to be set back.
            depth = childrenDepth;
        }

        // If we get here, not even the children could find it.
        return nullptr;
    }

    const aiNode* FindNodeByName(const aiScene &scene, const aiString &name, unsigned int &depth)
    {
        assert(scene.mRootNode != nullptr);

        return FindNodeByName(scene, *scene.mRootNode, name, depth);
    }

    void CopyNodesWithMeshes(const aiScene &scene, const aiNode &node, const aiMatrix4x4 &accumulatedTransform, Model &model)
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

                auto va = new VertexArray(VertexArrayUsage_Static, format);       // TODO: Check if the mesh is animated and determine whether or not the mesh should be dynamic or static.
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

                model.AttachMesh(va, nullptr);
            }

            // Here we need to build the meshes skeleton, if it has one. What we do here is store bone information in a list. One of the attributes
            // of this data is the depth of the bone relative to the root node. The bone with the lowest depth (closest to the root) will be the
            // root bone.
            struct BoneData
            {
                BoneData(unsigned int depth, const aiNode* node, const aiBone* bone)
                    : depth(depth), node(node), bone(bone)
                {
                }

                unsigned int  depth;
                const aiNode* node;
                const aiBone* bone;
            };

            // This will hold a list of every bone in the model.
            GTCore::Vector<BoneData> boneList(mesh->mNumBones);

            // This will point to the bone data with the lowest depth. By the time the bone iteration is complete, this will point to the root node.
            BoneData* rootBoneData = nullptr;


            for (unsigned int iBone = 0; iBone < mesh->mNumBones; ++iBone)
            {
                //GTEngine::Log("Bone Name: %s", mesh->mBones[iBone]->mName.C_Str());

                auto bone = mesh->mBones[iBone];
                assert(bone != nullptr);

                // The name of the bone refers to a scene node. We need to find that node.
                auto boneDepth = 0U;
                auto boneNode  = FindNodeByName(scene, bone->mName, boneDepth);
                if (boneNode != nullptr)
                {
                    // We need to push the item onto the list before checking if it's the new root. The reason is because we'll retrieve a reference
                    // to the new item from the list to use as the object.
                    boneList.PushBack(BoneData(boneDepth, &node, bone));

                    // Now we need to check if the depth is lower than the existing root bone.
                    auto &iBoneData = boneList.GetBack();

                    if (rootBoneData != nullptr)
                    {
                        if (iBoneData.depth < rootBoneData->depth)
                        {
                            rootBoneData = &iBoneData;
                        }
                    }
                    else
                    {
                        rootBoneData = &iBoneData;
                    }
                }
            }

            // By this stage we have the list of bones and the root bone. Now we need to build the skeleton.
            if (rootBoneData != nullptr)
            {
                GTEngine::Log("Root Bone: %s", rootBoneData->bone->mName.C_Str());


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
        /*

        // If the file is already loaded, we don't want to reload. Instead we create a new instance of the model using the existing information.
        auto modelInfo = LoadedModels.Find(fileName);
        if (modelInfo == nullptr)
        {

        }
        
        // Now that we have information about the model, we can create a new Model object and return it.
        if (modelInfo != nullptr)
        {
        }

        */






        Assimp::Importer importer;

        auto scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenSmoothNormals);
        if (scene != nullptr)
        {
            // We need to recursively read each scene node and attach the meshes where possible.
            auto root = scene->mRootNode;
            if (root != nullptr)
            {
                auto model = new Model;

                // We're going to use a different transformation depending on whether or not we are importing a blender file. This is a bit of a hacky, so this may
                // be moved layer on.
                aiMatrix4x4 transform;

                if (GTCore::Path::ExtensionEqual(fileName, "blend"))
                {
                    transform.RotationX(glm::radians(-90.0f), transform);
                }

                CopyNodesWithMeshes(*scene, *root, transform, *model);

                // We need to generate tangents and bitangents.
                model->GenerateTangentsAndBitangents();


                // Here we will grab the animations.
                for (unsigned int iAnimation = 0; iAnimation < scene->mNumAnimations; ++iAnimation)
                {
                    auto animation = scene->mAnimations[iAnimation];
                    assert(animation != nullptr);

                    //animation->

                    GTEngine::Log("Animation: %s", animation->mName.C_Str());
                }


                return model;
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