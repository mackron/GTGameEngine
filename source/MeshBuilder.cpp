// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/MeshBuilder.hpp>

namespace GTEngine
{
    MeshBuilder::MeshBuilder(size_t vertexSizeInFloats)
        : vertexSizeInFloats(vertexSizeInFloats), vertexBuffer(), indexBuffer()
    {
    }

    MeshBuilder::~MeshBuilder()
    {
    }

    void MeshBuilder::EmitVertex(const float* vertexData)
    {
        unsigned int index;
        if (!this->Find(vertexData, index))
        {
            index = static_cast<unsigned int>(this->vertexBuffer.count) / vertexSizeInFloats;

            // Here is where we append the input vertex data to the mesh builder's vertex data buffer.
            for (auto i = 0U; i < vertexSizeInFloats; ++i)
            {
                this->vertexBuffer.PushBack(vertexData[i]);
            }
        }
        
        // Now we just add the index to the end.
        this->indexBuffer.PushBack(index);
    }

    bool MeshBuilder::Find(const float* vertexData, unsigned int &indexOut)
    {
        size_t vertexCount = this->vertexBuffer.count / vertexSizeInFloats;

        for (size_t i = 0; i < vertexCount; ++i)
        {
            if (this->Equal(this->vertexBuffer.buffer + (i * vertexSizeInFloats), vertexData))
            {
                indexOut = static_cast<unsigned int>(i);
                return true;
            }
        }

        return false;
    }

    bool MeshBuilder::Equal(const float* vertexData0, const float* vertexData1)
    {
        // TODO: Look into SSE instrinsics for an optimization for appropriately aligned formats. P3T2N3 should benefit from this...
        for (size_t i = 0; i < vertexSizeInFloats; ++i)
        {
            if (vertexData0[i] != vertexData1[i])
            {
                return false;
            }
        }

        // If we make it here, it means we are equal.
        return true;
    }

    void MeshBuilder::Clear()
    {
        this->vertexBuffer.Clear();
        this->indexBuffer.Clear();
    }
}


namespace GTEngine
{
    /// MeshBuilderP3T2N3
    MeshBuilderP3T2N3::MeshBuilderP3T2N3()
        : MeshBuilder(8)
    {
    }

    MeshBuilderP3T2N3::~MeshBuilderP3T2N3()
    {
    }

    void MeshBuilderP3T2N3::EmitVertex(const glm::vec3 &position, const glm::vec2 &texCoord, const glm::vec3 &normal)
    {
        float data[8] =
        {
            position.x, position.y, position.z,
            texCoord.x, texCoord.y,
            normal.x,   normal.y,   normal.z
        };

        MeshBuilder::EmitVertex(data);
    }



    /// MeshBuilderP3
    MeshBuilderP3::MeshBuilderP3()
        : MeshBuilder(3)
    {
    }

    MeshBuilderP3::~MeshBuilderP3()
    {
    }

    void MeshBuilderP3::EmitVertex(const glm::vec3 &position)
    {
        MeshBuilder::EmitVertex(&position[0]);
    }



    /// WireframeSphereMeshBuilder
    WireframeSphereMeshBuilder::WireframeSphereMeshBuilder(unsigned int ringSegmentsCountIn)
        : ringSegmentsCount(ringSegmentsCountIn), xyRing(), xzRing(), yzRing(), cameraRing()
    {
        if (this->ringSegmentsCount < 3)
        {
            this->ringSegmentsCount == 3;
        }
    }

    WireframeSphereMeshBuilder::~WireframeSphereMeshBuilder()
    {
    }


    void WireframeSphereMeshBuilder::Build(const glm::mat4 &cameraView)
    {
        glm::mat4 xyTransform = glm::mat4();
        glm::mat4 xzTransform = glm::rotate( 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 yzTransform = glm::rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

        this->BuildRing(cameraView, xyTransform, true,  this->xyRing);
        this->BuildRing(cameraView, xzTransform, true,  this->xzRing);
        this->BuildRing(cameraView, yzTransform, true,  this->yzRing);
        this->BuildRing(cameraView, cameraView,  false, this->cameraRing);
    }

    void WireframeSphereMeshBuilder::BuildRing(const glm::mat4 &cameraView, const glm::mat4 &ringTransform, bool cullBackFacingSegments, MeshBuilderP3 &ringOut)
    {
        // All we care about is the orientation.
        glm::mat3 cameraView3    = glm::mat3(cameraView);
        glm::mat3 ringTransform3 = glm::mat3(ringTransform);

        float circleRadius       = 1.0f;
        float circleSegmentAngle = glm::radians(360.0f / static_cast<float>(this->ringSegmentsCount));

        GTCore::Vector<glm::vec3> vertices(this->ringSegmentsCount);


        // Important that we clear.
        ringOut.Clear();

        // We create the vertices, but don't want to output them just yet. We need to ignore back-facing segments, which we'll do in the second step.
        for (unsigned int iSegment = 0; iSegment < this->ringSegmentsCount; ++iSegment)
        {
            glm::vec3 position;
            position.x = std::cos(circleSegmentAngle * iSegment) * circleRadius;
            position.y = std::sin(circleSegmentAngle * iSegment) * circleRadius;
            position.z = 0.0f;
            position = ringTransform3 * position;

            vertices.PushBack(position);
        }


        // Now we need to cull any back-facing segments, if applicable.
        for (size_t iVertex = 0; iVertex < vertices.count; ++iVertex)
        {
            size_t index0 = iVertex;
            size_t index1 = (iVertex + 1) % vertices.count;

            if (cullBackFacingSegments)
            {
                glm::vec3 vertex0(ringTransform3 * vertices[index0]);
                glm::vec3 vertex1(ringTransform3 * vertices[index1]);

                if (vertex0.z <= 0.0f || vertex1.z <= 0.0f)
                {
                    ringOut.EmitVertex(vertices[index0]);
                    ringOut.EmitVertex(vertices[index1]);
                }
            }
            else
            {
                ringOut.EmitVertex(vertices[index0]);
                ringOut.EmitVertex(vertices[index1]);
            }
        }
    }
}
