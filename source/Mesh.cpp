
#include <GTEngine/Mesh.hpp>

namespace GTEngine
{
    bool Mesh::GenerateTangentsAndBitangents()
    {
        auto &format      = this->geometry->GetFormat();
            
        auto positionStride  = format.GetAttributeOffset(VertexAttribs::Position);
        auto normalStride    = format.GetAttributeOffset(VertexAttribs::Normal);
        auto texCoordStride  = format.GetAttributeOffset(VertexAttribs::TexCoord);
        auto tangentStride   = format.GetAttributeOffset(VertexAttribs::Tangent);
        auto bitangentStride = format.GetAttributeOffset(VertexAttribs::Bitangent);

        if (positionStride != -1 && normalStride != -1 && texCoordStride != -1 && tangentStride != -1 && bitangentStride != -1)
        {
            auto vertexSize  = format.GetSize();
            auto indexCount  = this->geometry->GetIndexCount();
            auto indexData   = this->geometry->MapIndexData();
            auto vertexData  = this->geometry->MapVertexData();

            // We need to loop over triangle, which is every 3 indices.
            for (size_t iVertex = 0; iVertex < indexCount; iVertex += 3)
            {
                auto vertex0Src = vertexData + (indexData[iVertex + 0] * vertexSize);
                auto vertex1Src = vertexData + (indexData[iVertex + 1] * vertexSize);
                auto vertex2Src = vertexData + (indexData[iVertex + 2] * vertexSize);

                auto p0Src = vertex0Src + positionStride;
                auto p1Src = vertex1Src + positionStride;
                auto p2Src = vertex2Src + positionStride;

                auto n0Src = vertex0Src + normalStride;
                auto n1Src = vertex1Src + normalStride;
                auto n2Src = vertex2Src + normalStride;

                auto t0Src = vertex0Src + texCoordStride;
                auto t1Src = vertex1Src + texCoordStride;
                auto t2Src = vertex2Src + texCoordStride;

                glm::vec3 p0(p0Src[0], p0Src[1], p0Src[2]);
                glm::vec3 p1(p1Src[0], p1Src[1], p1Src[2]);
                glm::vec3 p2(p2Src[0], p2Src[1], p2Src[2]);

                glm::vec3 n0(n0Src[0], n0Src[1], n0Src[2]);
                glm::vec3 n1(n1Src[0], n1Src[1], n1Src[2]);
                glm::vec3 n2(n2Src[0], n2Src[1], n2Src[2]);

                glm::vec2 t0(t0Src[0], t0Src[1]);
                glm::vec2 t1(t1Src[0], t1Src[1]);
                glm::vec2 t2(t2Src[0], t2Src[1]);


                glm::vec3 p10 = p1 - p0;
                glm::vec3 p20 = p2 - p0;
                glm::vec2 t10 = t1 - t0;
                glm::vec2 t20 = t2 - t0;

                float r = 1.0F / (t10.x * t20.y - t20.x * t10.y);

                glm::vec3 tangent  = glm::normalize((p10 * t20.y) - (p20 * t10.y)) * r;
                glm::vec3 tangent0 = glm::orthonormalize(tangent, n0);
                glm::vec3 tangent1 = glm::orthonormalize(tangent, n1);
                glm::vec3 tangent2 = glm::orthonormalize(tangent, n2);

                glm::vec3 bitangent0 = glm::normalize(glm::cross(n0, tangent0));
                glm::vec3 bitangent1 = glm::normalize(glm::cross(n1, tangent1));
                glm::vec3 bitangent2 = glm::normalize(glm::cross(n2, tangent2));

                if (glm::dot(glm::cross(n0, tangent0), bitangent0) < 0)
                {
                    bitangent0 *= -1.0f;
                }
                if (glm::dot(glm::cross(n1, tangent1), bitangent1) < 0)
                {
                    bitangent1 *= -1.0f;
                }
                if (glm::dot(glm::cross(n2, tangent2), bitangent2) < 0)
                {
                    bitangent2 *= -1.0f;
                }
                    
                auto tangent0Src = vertex0Src + tangentStride;
                auto tangent1Src = vertex1Src + tangentStride;
                auto tangent2Src = vertex2Src + tangentStride;
                tangent0Src[0] = tangent0.x; tangent0Src[1] = tangent0.y; tangent0Src[2] = tangent0.z;
                tangent1Src[0] = tangent1.x; tangent1Src[1] = tangent1.y; tangent1Src[2] = tangent1.z;
                tangent2Src[0] = tangent2.x; tangent2Src[1] = tangent2.y; tangent2Src[2] = tangent2.z;

                auto bitangent0Src = vertex0Src + bitangentStride;
                auto bitangent1Src = vertex1Src + bitangentStride;
                auto bitangent2Src = vertex2Src + bitangentStride;
                bitangent0Src[0] = bitangent0.x; bitangent0Src[1] = bitangent0.y; bitangent0Src[2] = bitangent0.z;
                bitangent1Src[0] = bitangent1.x; bitangent1Src[1] = bitangent1.y; bitangent1Src[2] = bitangent1.z;
                bitangent2Src[0] = bitangent2.x; bitangent2Src[1] = bitangent2.y; bitangent2Src[2] = bitangent2.z;
            }
                

            this->geometry->UnmapIndexData();
            this->geometry->UnmapVertexData();

            return true;
        }
        else
        {
            // We don't have all of the required attributes in the the vertex format.
            return false;
        }
    }



    // TOOD: Scaling works, but we need to check that we can reuse the same vertex array.
    btGImpactMeshShape* Mesh::BuildCollisionShape(const glm::vec3 &scale)
    {
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
}
