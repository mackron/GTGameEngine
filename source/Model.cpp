
#include <GTEngine/Model.hpp>
#include <GTEngine/VertexArrayFactory.hpp>
#include <GTEngine/CPUVertexShader_SimpleTransform.hpp>

namespace GTEngine
{
    Model::Model()
        : meshes(), materials()
    {
    }

    Model::~Model()
    {
        // We need to delete each mesh.
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            delete this->meshes[i];
        }
    }

    void Model::AttachMesh(VertexArray* va, Material* material)
    {
        auto newMesh = new Mesh(va);

        this->meshes.PushBack(newMesh);
        this->materials.PushBack(material);
    }

    void Model::ApplyTransformation(const glm::mat4 &transform)
    {
        // We're going to use a CPU vertex shader here.
        CPUVertexShader_SimpleTransform shader(transform);
        
        // We need to execute the shader on all meshes.
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            auto mesh = this->meshes[i];
            assert(mesh != nullptr);

            auto &format      = mesh->va->GetFormat();
            auto  vertexCount = mesh->va->GetVertexCount();
            auto  vertexData  = mesh->va->MapVertexData();

            shader.Execute(vertexData, vertexCount, format, vertexData);

            mesh->va->UnmapVertexData();
        }


        this->GenerateTangentsAndBitangents();
    }

    void Model::GenerateTangentsAndBitangents()
    {
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            this->meshes[i]->GenerateTangentsAndBitangents();
        }
    }
}


