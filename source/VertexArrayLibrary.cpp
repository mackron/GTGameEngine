// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/MeshBuilder.hpp>
#include <GTEngine/Core/Math.hpp>


namespace GTEngine
{
    static VertexArray* FullscreenQuad     = nullptr;
    static VertexArray* FullscreenTriangle = nullptr;

    VertexArray* VertexArrayLibrary::GetFullscreenQuadVA()
    {
        if (FullscreenQuad == nullptr)
        {
            FullscreenQuad = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P2);
            
            float vertices[] =
            {
                -1.0f, -1.0f,
                 1.0f, -1.0f,
                 1.0f,  1.0f,
                -1.0f,  1.0f
            };

            unsigned int indices[] =
            {
                0, 1, 2,
                2, 3, 0,
            };
            
            FullscreenQuad->SetData(vertices, 4, indices, 6);
        }

        return FullscreenQuad;
    }

    VertexArray* VertexArrayLibrary::GetFullscreenTriangleVA()
    {
        if (FullscreenTriangle == nullptr)
        {
            FullscreenTriangle = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P2T2);

            float triangleVertices[] =
            {
                -3.0f, -1.0f,
                -1.0f,  0.0f,

                 1.0f, -1.0f,
                 1.0f,  0.0f,

                 1.0f,  3.0f,
                 1.0f,  2.0f,
            };

            unsigned int triangleIndices[] =
            {
                0, 1, 2
            };

            FullscreenTriangle->SetData(triangleVertices, 3, triangleIndices, 3);
        }

        return FullscreenTriangle;
    }
}


namespace GTEngine
{
    ////////////////////////////////////////////////////////////////
    // Startup/Shutdown.

    bool VertexArrayLibrary::Startup()
    {
        return true;
    }

    void VertexArrayLibrary::Shutdown()
    {
        Renderer::DeleteVertexArray(FullscreenQuad);
        Renderer::DeleteVertexArray(FullscreenTriangle);

        FullscreenQuad     = nullptr;
        FullscreenTriangle = nullptr;
    }


    ////////////////////////////////////////////////////////////////
    // Primitive Shapes

    VertexArray* VertexArrayLibrary::CreateBox(float halfWidth, float halfHeight, float halfDepth)
    {
        // A box will have 24 unique vertices. We first calculate the positions of each corner and then build the texture coordinates and normals.
        glm::vec3 positions[8];

        positions[0].x = -halfWidth;
        positions[0].y = +halfHeight;
        positions[0].z = +halfDepth;

        positions[1].x = -halfWidth;
        positions[1].y = -halfHeight;
        positions[1].z = +halfDepth;

        positions[2].x = +halfWidth;
        positions[2].y = -halfHeight;
        positions[2].z = +halfDepth;

        positions[3].x = +halfWidth;
        positions[3].y = +halfHeight;
        positions[3].z = +halfDepth;

        positions[4].x = -halfWidth;
        positions[4].y = +halfHeight;
        positions[4].z = -halfDepth;

        positions[5].x = -halfWidth;
        positions[5].y = -halfHeight;
        positions[5].z = -halfDepth;

        positions[6].x = +halfWidth;
        positions[6].y = -halfHeight;
        positions[6].z = -halfDepth;

        positions[7].x = +halfWidth;
        positions[7].y = +halfHeight;
        positions[7].z = -halfDepth;


        // The different texture coordinates.
        glm::vec2 texCoordLeftTop(    0.0f, 0.0f);
        glm::vec2 texCoordLeftBottom( 0.0f, 1.0f);
        glm::vec2 texCoordRightBottom(1.0f, 1.0f);
        glm::vec2 texCoordRightTop(   1.0f, 0.0f);

        // The different normals.
        glm::vec3 normalFront(  0.0f,  0.0f,  1.0f);
        glm::vec3 normalBack(   0.0f,  0.0f, -1.0f);
        glm::vec3 normalTop(    0.0f,  1.0f,  0.0f);
        glm::vec3 normalBottom( 0.0f, -1.0f,  0.0f);
        glm::vec3 normalRight(  1.0f,  0.0f,  0.0f);
        glm::vec3 normalLeft(  -1.0f,  0.0f,  0.0f);


        // With the various properties retrieved we can now generate the geometric data.
        MeshBuilderP3T2N3 builder;

        // Front face.
        builder.EmitVertex(positions[0], texCoordLeftTop,     normalFront);     // Left/Top
        builder.EmitVertex(positions[1], texCoordLeftBottom,  normalFront);     // Left/Bottom
        builder.EmitVertex(positions[2], texCoordRightBottom, normalFront);     // Right/Bottom
        builder.EmitVertex(positions[2], texCoordRightBottom, normalFront);     // Right/Bottom
        builder.EmitVertex(positions[3], texCoordRightTop,    normalFront);     // Right/Top
        builder.EmitVertex(positions[0], texCoordLeftTop,     normalFront);     // Left/Top
        
        // Right face.
        builder.EmitVertex(positions[3], texCoordLeftTop,     normalRight);     // Left/Top
        builder.EmitVertex(positions[2], texCoordLeftBottom,  normalRight);     // Left/Bottom
        builder.EmitVertex(positions[6], texCoordRightBottom, normalRight);     // Right/Bottom
        builder.EmitVertex(positions[6], texCoordRightBottom, normalRight);     // Right/Bottom
        builder.EmitVertex(positions[7], texCoordRightTop,    normalRight);     // Right/Top
        builder.EmitVertex(positions[3], texCoordLeftTop,     normalRight);     // Left/Top

        // Back face.
        builder.EmitVertex(positions[7], texCoordLeftTop,     normalBack);      // Left/Top
        builder.EmitVertex(positions[6], texCoordLeftBottom,  normalBack);      // Left/Bottom
        builder.EmitVertex(positions[5], texCoordRightBottom, normalBack);      // Right/Bottom
        builder.EmitVertex(positions[5], texCoordRightBottom, normalBack);      // Right/Bottom
        builder.EmitVertex(positions[4], texCoordRightTop,    normalBack);      // Right/Top
        builder.EmitVertex(positions[7], texCoordLeftTop,     normalBack);      // Left/Top

        // Left face.
        builder.EmitVertex(positions[4], texCoordLeftTop,     normalLeft);      // Left/Top
        builder.EmitVertex(positions[5], texCoordLeftBottom,  normalLeft);      // Left/Bottom
        builder.EmitVertex(positions[1], texCoordRightBottom, normalLeft);      // Right/Bottom
        builder.EmitVertex(positions[1], texCoordRightBottom, normalLeft);      // Right/Bottom
        builder.EmitVertex(positions[0], texCoordRightTop,    normalLeft);      // Right/Top
        builder.EmitVertex(positions[4], texCoordLeftTop,     normalLeft);      // Left/Top

        // Top face.
        builder.EmitVertex(positions[4], texCoordLeftTop,     normalTop);       // Left/Top
        builder.EmitVertex(positions[0], texCoordLeftBottom,  normalTop);       // Left/Bottom
        builder.EmitVertex(positions[3], texCoordRightBottom, normalTop);       // Right/Bottom
        builder.EmitVertex(positions[3], texCoordRightBottom, normalTop);       // Right/Bottom
        builder.EmitVertex(positions[7], texCoordRightTop,    normalTop);       // Right/Top
        builder.EmitVertex(positions[4], texCoordLeftTop,     normalTop);       // Left/Top

        // Bottom face.
        builder.EmitVertex(positions[1], texCoordLeftTop,     normalBottom);    // Left/Top
        builder.EmitVertex(positions[5], texCoordLeftBottom,  normalBottom);    // Left/Bottom
        builder.EmitVertex(positions[6], texCoordRightBottom, normalBottom);    // Right/Bottom
        builder.EmitVertex(positions[6], texCoordRightBottom, normalBottom);    // Right/Bottom
        builder.EmitVertex(positions[2], texCoordRightTop,    normalBottom);    // Right/Top
        builder.EmitVertex(positions[1], texCoordLeftTop,     normalBottom);    // Left/Top
        

        // The builder will contain all of the geometric data so now we can create a vertex array from it.
        return VertexArrayLibrary::CreateFromBuilder(builder);
    }


    ////////////////////////////////////////////////////////////////
    // Wireframe Primitives

    VertexArray* VertexArrayLibrary::CreateWireframeFromAABB(const btAABB &aabb)
    {
        auto va = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);

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



    ////////////////////////////////////////////////////////////////
    // Convex Hulls

    VertexArray* VertexArrayLibrary::CreateFromConvexHull(const ConvexHull &convexHull)
    {
        // We use a mesh builder for this.
        MeshBuilderP3T2N3 builder;

        auto vertices   = convexHull.GetVertices();
        auto indices    = convexHull.GetIndices();
        auto indexCount = convexHull.GetIndexCount();

        // We need to use flat shading here, so we go triangle-by-triangle.
        for (size_t iTriangle = 0; iTriangle < indexCount / 3; ++iTriangle)
        {
            auto index0 = indices[iTriangle * 3 + 0];
            auto index1 = indices[iTriangle * 3 + 1];
            auto index2 = indices[iTriangle * 3 + 2];

            auto vertex0 = vertices + (index0 * 3);
            auto vertex1 = vertices + (index1 * 3);
            auto vertex2 = vertices + (index2 * 3);

            glm::vec3 v0(vertex0[0], vertex0[1], vertex0[2]);
            glm::vec3 v1(vertex1[0], vertex1[1], vertex1[2]);
            glm::vec3 v2(vertex2[0], vertex2[1], vertex2[2]);

            glm::vec3 u = v1 - v0;
            glm::vec3 v = v2 - v0;

            glm::vec3 normal((u.y * v.z) - (u.z * v.y),
                             (u.z * v.x) - (u.x * v.z),
                             (u.x * v.y) - (u.y * v.x));

            glm::vec2 texCoord(0.0f, 0.0f);

            builder.EmitVertex(v0, texCoord, normal);
            builder.EmitVertex(v1, texCoord, normal);
            builder.EmitVertex(v2, texCoord, normal);
        }

        return VertexArrayLibrary::CreateFromBuilder(builder);
    }



    ////////////////////////////////////////////////////////////////
    // Mesh Builders

    VertexArray* VertexArrayLibrary::CreateFromBuilder(const MeshBuilderP3T2N3 &builder)
    {
        auto va = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3T2N3T3B3);
        va->SetIndexData(builder.GetIndexData(), builder.GetIndexCount());
        
        // Here we need to copy over the data. The data will be in a different format, so we need to do this vertex by vertex.
        auto sourceVertexData  = builder.GetVertexData();
        auto sourceVertexCount = builder.GetVertexCount();
        
        va->SetVertexData(nullptr, sourceVertexCount);

        auto destVertexData = va->MapVertexData();
        {
            for (size_t i = 0; i < sourceVertexCount; ++i)
            {
                auto sourceVertex = sourceVertexData + (i * 8);     // <-- '8' is the size of P3T2N3
                auto destVertex   = destVertexData   + (i * 14);    // <-- '14' is the size of P3T2N3T3B3

                memcpy(destVertex, sourceVertex, sizeof(float) * 8);
            }
        }
        va->UnmapVertexData();

        // Here is where we generate the tangents and bitangents for the vertex array.
        va->GenerateTangentsAndBitangents();

        return va;
    }



    ////////////////////////////////////////////////////////////////
    // Combining

    VertexArray* VertexArrayLibrary::CreateCombined(const VertexArray* const* vertexArrays, size_t count, const VertexFormat &format)
    {
        if (vertexArrays != nullptr && count > 0)
        {
            // We'll need to retrieve a few attributes.
            size_t vertexCount = 0;
            size_t indexCount  = 0;
            for (size_t i = 0; i < count; ++i)
            {
                vertexCount += vertexArrays[i]->GetVertexCount();
                indexCount  += vertexArrays[i]->GetIndexCount();
            }

            auto va = Renderer::CreateVertexArray(VertexArrayUsage_Static, format);
            va->SetData(nullptr, vertexCount, nullptr, indexCount);

            auto destVertexSize = format.GetSize();

            auto destVertexData = va->MapVertexData();
            auto destIndexData  = va->MapIndexData();
            {
                unsigned int indexOffset = 0;

                for (size_t iVA = 0; iVA < count; ++iVA)
                {
                    auto sourceVA         = vertexArrays[iVA];
                    auto sourceVertexSize = sourceVA->GetFormat().GetSize();

                    // Now we need to loop over every vertex in 'va' and copy over the relevant data to the destination.
                    for (size_t iVertex = 0; iVertex < sourceVA->GetVertexCount(); ++iVertex)
                    {
                        auto sourceVertexData = sourceVA->GetVertexDataPtr() + (iVertex * sourceVertexSize);

                        // Now we need to loop over all of the attributes in <format> and try and find the corresponding one in the input format. If we find it, we
                        // copy it over. Otherwise we leave it undefined.
                        size_t iAttributeOffset = 0;
                        for (size_t iAttribute = 0; format[iAttribute] != VertexAttribs::End; iAttribute += 2)
                        {
                            size_t sourceComponentCount;
                            size_t sourceOffset;
                            if (sourceVA->GetFormat().GetAttributeInfo(format[iAttribute], sourceComponentCount, sourceOffset))
                            {
                                // When we get here the component will have 
                                memcpy(
                                    destVertexData   + iAttributeOffset,
                                    sourceVertexData + sourceOffset,
                                    sizeof(float) * GTLib::Min(static_cast<size_t>(format[iAttribute + 1]), sourceComponentCount));
                            }

                            iAttributeOffset += format[iAttribute + 1];
                        }

                        // We need to move to the next destination vertex.
                        destVertexData += destVertexSize;
                    }


                    // Now we need to copy over the index data. We can't do this in one go because we need to offset the indices.
                    for (size_t iIndex = 0; iIndex < sourceVA->GetIndexCount(); ++iIndex)
                    {
                        *destIndexData++ = indexOffset + sourceVA->GetIndexDataPtr()[iIndex];
                    }
                    

                    // We can't forget to modify the index offset in preparation for the next vertex array.
                    indexOffset += sourceVA->GetVertexCount();
                }
            }
            va->UnmapVertexData();
            va->UnmapIndexData();


            return va;
        }

        return nullptr;
    }


    void VertexArrayLibrary::Delete(VertexArray* va)
    {
        Renderer::DeleteVertexArray(va);
    }
}
