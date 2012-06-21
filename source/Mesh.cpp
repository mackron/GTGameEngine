
#include <GTEngine/Mesh.hpp>
#include <GTEngine/CPUVertexShader_Skinning.hpp>
#include <GTEngine/Rendering.hpp>

namespace GTEngine
{
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


    void Mesh::AttachBoneWeights(const Bone &localBone, size_t weightCount, const VertexWeightPair* weightBuffer)
    {
        if (weightCount > 0 && weightBuffer != nullptr)
        {
            assert(this->geometry != nullptr);  // <-- the meshes geometry must be set beforehand.

            // We first need to ensure we have a skinning data structure.
            if (this->skinningData == nullptr)
            {
                this->skinningData = new MeshSkinningData(*this->geometry);
            }

            // At this point we can assert that we have skinning data and we can attach the bone weights to each vertex via the skinning vertex attributes.
            for (size_t i = 0; i < weightCount; ++i)
            {
                this->skinningData->skinningVertexAttributes[weightBuffer[i].vertexID].AddBoneWeightPair(localBone, weightBuffer[i].weight);
            }
        }
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
                shader.SetSkinningVertexAttributes(this->skinningData->skinningVertexAttributes);

                shader.Execute(srcVertices, this->geometry->GetVertexCount(), this->geometry->GetFormat(), dstVertices);
            }
            else
            {
                memcpy(dstVertices, srcVertices, this->geometry->GetFormat().GetSize() * this->geometry->GetVertexCount());
            }


            this->geometry->UnmapVertexData();
            dstVertexArray->UnmapVertexData();

            this->hasAnimated = true;
        }
    }
}
