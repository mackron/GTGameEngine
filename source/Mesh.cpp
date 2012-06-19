
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
        if (this->skinningData != nullptr)
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


    /*
    // TOOD: Scaling works, but we need to check that we can reuse the same vertex array.
    btGImpactMeshShape* Mesh::BuildCollisionShape(const glm::vec3 &scale)
    {
        delete this->collisionVA;

        auto &vaFormat    = this->geometry->GetFormat();
        auto  indexCount  = this->geometry->GetIndexCount();
        auto  vertexCount = this->geometry->GetVertexCount();
        auto  indexData   = reinterpret_cast<int *>(this->geometry->MapIndexData());
        auto  vertexData  = this->geometry->MapVertexData();

        auto  positionOffset = vaFormat.GetAttributeOffset(VertexAttribs::Position);

        this->collisionVA = new btTriangleIndexVertexArray
        (
            static_cast<int>(indexCount / 3), indexData, 3 * sizeof(unsigned int),
            static_cast<int>(vertexCount), vertexData + positionOffset, static_cast<int>(vaFormat.GetSizeInBytes())
        );

        auto newCollisionShape = new btGImpactMeshShape(this->collisionVA);
        newCollisionShape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
        newCollisionShape->updateBound();        // <-- must call this, otherwise the AABB will be incorrect.

        this->geometry->UnmapIndexData();
        this->geometry->UnmapVertexData();

        return newCollisionShape;
    }
    */


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
        }
    }
}
