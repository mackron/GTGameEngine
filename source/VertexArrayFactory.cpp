
#include <GTEngine/VertexArrayFactory.hpp>

namespace GTEngine
{
    VertexArray * VertexArrayFactory::CreateWireframeFromAABB(const btAABB &aabb)
    {
        VertexArray * va = new VertexArray(VertexArrayUsage_Static, VertexFormat::P3);

        float vertices[] =
        {
            aabb.m_min.x(), aabb.m_min.y(), aabb.m_max.z(),     // left/bottom/front
            aabb.m_max.x(), aabb.m_min.y(), aabb.m_max.z(),     // right/bottom/front
            aabb.m_max.x(), aabb.m_max.y(), aabb.m_max.z(),     // right/top/front
            aabb.m_min.x(), aabb.m_max.y(), aabb.m_max.z(),     // left/top/front
            aabb.m_min.x(), aabb.m_min.y(), aabb.m_min.z(),     // left/bottom/back
            aabb.m_max.x(), aabb.m_min.y(), aabb.m_min.z(),     // right/bottom/back
            aabb.m_max.x(), aabb.m_max.y(), aabb.m_min.z(),     // right/top/back
            aabb.m_min.x(), aabb.m_max.y(), aabb.m_min.z()      // left/top/back
        };

        unsigned int indices[] =
        {
            // Front side.
            0, 1,
            1, 2,
            2, 3,
            3, 0,

            // Back side.
            4, 5,
            5, 6,
            6, 7,
            7, 4,

            // Connectors.
            0, 4,
            1, 5,
            2, 6,
            3, 7
        };

        va->SetData(vertices, 8, indices, 24);

        return va;
    }

    VertexArray * VertexArrayFactory::CreatePlaneXZ(float width, float height, VertexFormat &format)
    {
        VertexArray * va = new VertexArray(VertexArrayUsage_Static, format);

        auto vertexSize     = format.GetSize();
        auto positionOffset = format.GetAttributeOffset(VertexAttribs::Position);
        auto texCoordOffset = format.GetAttributeOffset(VertexAttribs::TexCoord);
        auto normalOffset   = format.GetAttributeOffset(VertexAttribs::Normal);

        auto vertices = reinterpret_cast<float *>(malloc(sizeof(float) * vertexSize * 4));

        auto vertex0 = vertices + (0 * vertexSize);     // left/front
        auto vertex1 = vertices + (1 * vertexSize);     // right/front
        auto vertex2 = vertices + (2 * vertexSize);     // right/back
        auto vertex3 = vertices + (3 * vertexSize);     // left/back

        auto halfWidth  = width  * 0.5f;
        auto halfHeight = height * 0.5f;

        // 1) Positions.
        if (positionOffset != -1)
        {
            vertex0[positionOffset + 0] = -halfWidth; vertex0[positionOffset + 1] = 0.0f; vertex0[positionOffset + 2] = +halfHeight;
            vertex1[positionOffset + 0] = +halfWidth; vertex1[positionOffset + 1] = 0.0f; vertex1[positionOffset + 2] = +halfHeight;
            vertex2[positionOffset + 0] = +halfWidth; vertex2[positionOffset + 1] = 0.0f; vertex2[positionOffset + 2] = -halfHeight;
            vertex3[positionOffset + 0] = -halfWidth; vertex3[positionOffset + 1] = 0.0f; vertex3[positionOffset + 2] = -halfHeight;
        }

        // 2) TexCoords.
        if (texCoordOffset != -1)
        {
            vertex0[texCoordOffset + 0] = 0.0f; vertex0[texCoordOffset + 1] = 0.0f;
            vertex1[texCoordOffset + 0] = 1.0f; vertex1[texCoordOffset + 1] = 0.0f;
            vertex2[texCoordOffset + 0] = 1.0f; vertex2[texCoordOffset + 1] = 1.0f;
            vertex3[texCoordOffset + 0] = 0.0f; vertex3[texCoordOffset + 1] = 1.0f;
        }

        // 3) Normals.
        if (normalOffset != -1)
        {
            vertex0[normalOffset + 0] = 0.0f; vertex0[normalOffset + 1] = 1.0f; vertex0[normalOffset + 2] = 0.0f;
            vertex1[normalOffset + 0] = 0.0f; vertex1[normalOffset + 1] = 1.0f; vertex1[normalOffset + 2] = 0.0f;
            vertex2[normalOffset + 0] = 0.0f; vertex2[normalOffset + 1] = 1.0f; vertex2[normalOffset + 2] = 0.0f;
            vertex3[normalOffset + 0] = 0.0f; vertex3[normalOffset + 1] = 1.0f; vertex3[normalOffset + 2] = 0.0f;
        }

        unsigned int indices[] =
        {
            0, 1, 2,
            2, 3, 0
        };

        va->SetData(vertices, 4, indices, 6);

        free(vertices);
        return va;
    }

    void VertexArrayFactory::DeleteVertexArray(VertexArray * vaToDelete)
    {
        delete vaToDelete;
    }
}