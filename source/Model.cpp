
#include <GTEngine/Model.hpp>
#include <GTEngine/VertexArrayFactory.hpp>

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
        glm::mat3 normalTransform = glm::inverse(glm::transpose(glm::mat3(transform)));

        // We need to do this for all meshes...
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            auto mesh = this->meshes[i];
            assert(mesh != nullptr);

            auto & format      = mesh->va->GetFormat();
            auto   indexCount  = mesh->va->GetIndexCount();
            auto   indexData   = mesh->va->MapIndexData();
            auto   vertexCount = mesh->va->GetVertexCount();
            auto   vertexData  = mesh->va->MapVertexData();

            auto   vertexSize             = format.GetSize();
            auto   positionStride         = format.GetAttributeOffset(VertexAttribs::Position);
            auto   normalStride           = format.GetAttributeOffset(VertexAttribs::Normal);
            auto   positionComponentCount = format.GetAttributeComponentCount(VertexAttribs::Position);

            // The buffer containing the new vertex information.
            auto newVertexData = reinterpret_cast<float *>(malloc(sizeof(float) * vertexCount * vertexSize));

            for (auto iIndex = 0U; iIndex < indexCount; ++iIndex)
            {
                auto index = indexData[iIndex];
                auto vertexSrc = vertexData + (index * vertexSize);
                auto vertexDst = newVertexData + (index * vertexSize);

                // Positions and normals need to be transformed. Tangents and binormals will also need to be done in the future.
                glm::vec4 position(0.0f, 0.0f, 0.0f, 1.0f);
                glm::vec3 normal;

                // 1) Position.
                if (positionStride >= 0)
                {
                    auto vertexPositionSrc = vertexSrc + positionStride;
                    auto vertexPositionDst = vertexDst + positionStride;
                    
                    position.x = *(vertexPositionSrc + 0);
                    if (positionComponentCount > 1) { position.y = *(vertexPositionSrc + 1); }
                    if (positionComponentCount > 2) { position.z = *(vertexPositionSrc + 2); }
                    if (positionComponentCount > 3) { position.w = *(vertexPositionSrc + 3); }

                    position = transform * position;

                    *(vertexPositionDst + 0) = position.x;
                    if (positionComponentCount > 1) { *(vertexPositionDst + 1) = position.y; }
                    if (positionComponentCount > 2) { *(vertexPositionDst + 2) = position.z; }
                    if (positionComponentCount > 3) { *(vertexPositionDst + 3) = position.w; }
                }

                // 2) Normal.
                if (normalStride >= 0)
                {
                    auto vertexNormalSrc = vertexSrc + normalStride;
                    auto vertexNormalDst = vertexDst + normalStride;

                    normal.x = *(vertexNormalSrc + 0);
                    normal.y = *(vertexNormalSrc + 1);
                    normal.z = *(vertexNormalSrc + 2);

                    normal = glm::normalize(normalTransform * normal);

                    *(vertexNormalDst + 0) = normal.x;
                    *(vertexNormalDst + 1) = normal.y;
                    *(vertexNormalDst + 2) = normal.z;
                }
            }

            // Now we need to copy over the new data.
            for (auto iIndex = 0U; iIndex < indexCount; ++iIndex)
            {
                auto index = indexData[iIndex];
                auto vertexSrc = newVertexData + (index * vertexSize);
                auto vertexDst = vertexData    + (index * vertexSize);

                // 1) Position.
                if (positionStride >= 0)
                {
                    auto vertexPositionSrc = vertexSrc + positionStride;
                    auto vertexPositionDst = vertexDst + positionStride;
                    
                    *(vertexPositionDst + 0) = *(vertexPositionSrc + 0);
                    if (positionComponentCount > 1) { *(vertexPositionDst + 1) = *(vertexPositionSrc + 1); }
                    if (positionComponentCount > 2) { *(vertexPositionDst + 2) = *(vertexPositionSrc + 2); }
                    if (positionComponentCount > 3) { *(vertexPositionDst + 3) = *(vertexPositionSrc + 3); }
                }

                // 2) Normal.
                if (normalStride >= 0)
                {
                    auto vertexNormalSrc = vertexSrc + normalStride;
                    auto vertexNormalDst = vertexDst + normalStride;

                    *(vertexNormalDst + 0) = *(vertexNormalSrc + 0);
                    *(vertexNormalDst + 1) = *(vertexNormalSrc + 1);
                    *(vertexNormalDst + 2) = *(vertexNormalSrc + 2);
                }
            }

            free(newVertexData);

            mesh->va->UnmapIndexData();
            mesh->va->UnmapVertexData();
        }
    }
}


