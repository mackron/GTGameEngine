
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/VertexArrayFactory.hpp>
#include <GTEngine/ColladaLoader.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/Errors.hpp>

#include <GTCore/Path.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace GTEngine
{
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

    Model* ModelLibrary::LoadFromCollada(const char* filename)
    {
        Model* model = nullptr;

        // We use the collada loader to load the file. This will construct a scene node containing all of the nodes in
        // the collada file. All we care about is the first model component.
        auto colladaNode = GTEngine::ColladaLoader::Load(filename);
        if (colladaNode != nullptr)
        {
            // We just find the first node with a model component and strip it from the collada scene node, which we'll be discarding. We
            // may need to tweak this is the player mode gets more complex than a single model.
            auto modelNode = colladaNode->FindFirstChildWithComponent<GTEngine::ModelComponent>(true);
            if (modelNode != nullptr)
            {
                // Now that we have the model node, we will steal the model from the model component.
                model = modelNode->GetComponent<GTEngine::ModelComponent>()->GetModel();
                model->ApplyTransformation(modelNode->GetTransformMatrix());
            }
        }

        delete colladaNode;
        return model;
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