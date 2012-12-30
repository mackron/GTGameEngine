
#include <GTEngine/Mesh.hpp>
#include <GTEngine/CPUVertexShader_Skinning.hpp>
#include <GTEngine/Rendering.hpp>
#include <GTEngine/MaterialLibrary.hpp>

namespace GTEngine
{
    Mesh::Mesh(DrawMode drawModeIn)
        : geometry(nullptr), material(nullptr),
          skinningData(nullptr),
          hasAnimated(false), deleteMaterial(false),
          drawMode(drawModeIn)
    {
    }

    Mesh::~Mesh()
    {
        delete this->skinningData;

        if (this->deleteMaterial)
        {
            GTEngine::MaterialLibrary::Delete(this->material);
        }
    }



    void Mesh::SetMaterial(Material* newMaterial)
    {
        if (this->deleteMaterial)
        {
            GTEngine::MaterialLibrary::Delete(this->material);
        }

        this->material       = newMaterial;
        this->deleteMaterial = false;
    }

    bool Mesh::SetMaterial(const char* materialFileName)
    {
        if (this->deleteMaterial)
        {
            GTEngine::MaterialLibrary::Delete(this->material);
        }

        auto newMaterial = GTEngine::MaterialLibrary::Create(materialFileName);
        if (newMaterial != nullptr)
        {
            this->material       = newMaterial;
            this->deleteMaterial = true;

            return true;
        }

        return false;
    }


    const VertexArray* Mesh::GetSkinnedGeometry() const
    {
        return const_cast<Mesh*>(this)->GetSkinnedGeometry();
    }

    VertexArray* Mesh::GetSkinnedGeometry()
    {
        if (this->skinningData != nullptr && this->hasAnimated)
        {
            return this->skinningData->skinnedGeometry[Renderer::BackIndex];
        }
        else
        {
            return this->geometry;
        }
    }


    void Mesh::SetSkinningData(const Bone* const* bones, const SkinningVertexAttribute* skinningVertexAttributes)
    {
        // Delete any previous data just in case. We should never actually have any, but we'll do it anyway just to be sure.
        delete this->skinningData;
        this->skinningData = new MeshSkinningData(bones, skinningVertexAttributes, *this->geometry);
    }


    bool Mesh::GenerateTangentsAndBitangents()
    {
        return this->geometry->GenerateTangentsAndBitangents();
    }



    void Mesh::ApplySkinning()
    {
        if (this->geometry != nullptr)
        {
            auto dstVertexArray = this->GetSkinnedGeometry();

            auto srcVertices = this->geometry->MapVertexData();
            auto dstVertices = dstVertexArray->MapVertexData();

            if (this->skinningData != nullptr)
            {
                CPUVertexShader_Skinning shader;
                shader.SetBoneBuffer(this->skinningData->bones);
                shader.SetSkinningVertexAttributes(this->skinningData->skinningVertexAttributes);

                shader.Execute(srcVertices, this->geometry->GetVertexCount(), this->geometry->GetFormat(), dstVertices);
            }


            this->geometry->UnmapVertexData();
            dstVertexArray->UnmapVertexData();

            this->hasAnimated = true;
        }
    }
}
