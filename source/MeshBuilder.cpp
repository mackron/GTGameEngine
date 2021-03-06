// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/MeshBuilder.hpp>
#include <GTGE/Physics.hpp>

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
#endif
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

namespace GT
{
    MeshBuilder::MeshBuilder(size_t vertexSizeInFloats)
        : vertexSizeInFloats(vertexSizeInFloats), vertexBuffer(), indexBuffer(), checkDuplicatesOnEmit(true)
    {
    }

    MeshBuilder::~MeshBuilder()
    {
    }

    void MeshBuilder::EmitVertex(const float* vertexData)
    {
        unsigned int index;
        if (!this->checkDuplicatesOnEmit || !this->Find(vertexData, index))
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

    void MeshBuilder::Merge(const MeshBuilder &other)
    {
        for (size_t iIndex = 0; iIndex < other.GetIndexCount(); ++iIndex)
        {
            unsigned int index  = other.GetIndexData()[iIndex];
            auto         vertex = other.GetVertexData() + (other.GetVertexSizeInFloats() * index);

            this->EmitVertex(vertex);
        }
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


namespace GT
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



    /// WireframeBoxMeshBuilder
    WireframeBoxMeshBuilder::WireframeBoxMeshBuilder()
    {
    }

    WireframeBoxMeshBuilder::~WireframeBoxMeshBuilder()
    {
    }

    void WireframeBoxMeshBuilder::Build(const glm::vec3 &halfExtents, const glm::mat4 &transform)
    {
        this->Clear();

        glm::vec3 vertex0 = glm::vec3(transform * glm::vec4(-halfExtents.x, -halfExtents.y,  halfExtents.z, 1.0f));
        glm::vec3 vertex1 = glm::vec3(transform * glm::vec4( halfExtents.x, -halfExtents.y,  halfExtents.z, 1.0f));
        glm::vec3 vertex2 = glm::vec3(transform * glm::vec4( halfExtents.x,  halfExtents.y,  halfExtents.z, 1.0f));
        glm::vec3 vertex3 = glm::vec3(transform * glm::vec4(-halfExtents.x,  halfExtents.y,  halfExtents.z, 1.0f));
        glm::vec3 vertex4 = glm::vec3(transform * glm::vec4(-halfExtents.x, -halfExtents.y, -halfExtents.z, 1.0f));
        glm::vec3 vertex5 = glm::vec3(transform * glm::vec4( halfExtents.x, -halfExtents.y, -halfExtents.z, 1.0f));
        glm::vec3 vertex6 = glm::vec3(transform * glm::vec4( halfExtents.x,  halfExtents.y, -halfExtents.z, 1.0f));
        glm::vec3 vertex7 = glm::vec3(transform * glm::vec4(-halfExtents.x,  halfExtents.y, -halfExtents.z, 1.0f));

        // Front size.
        this->EmitVertex(vertex0); this->EmitVertex(vertex1);
        this->EmitVertex(vertex1); this->EmitVertex(vertex2);
        this->EmitVertex(vertex2); this->EmitVertex(vertex3);
        this->EmitVertex(vertex3); this->EmitVertex(vertex0);

        // Back size.
        this->EmitVertex(vertex4); this->EmitVertex(vertex5);
        this->EmitVertex(vertex5); this->EmitVertex(vertex6);
        this->EmitVertex(vertex6); this->EmitVertex(vertex7);
        this->EmitVertex(vertex7); this->EmitVertex(vertex4);

        // Connectors size.
        this->EmitVertex(vertex0); this->EmitVertex(vertex4);
        this->EmitVertex(vertex1); this->EmitVertex(vertex5);
        this->EmitVertex(vertex2); this->EmitVertex(vertex6);
        this->EmitVertex(vertex3); this->EmitVertex(vertex7);
    }


    /// WireframeSphereMeshBuilder
    WireframeSphereMeshBuilder::WireframeSphereMeshBuilder(unsigned int ringSegmentsCountIn)
        : ringSegmentsCount(ringSegmentsCountIn), rings(5)
    {
        this->rings.Resize(5);

        if (this->ringSegmentsCount < 3)
        {
            this->ringSegmentsCount = 3;
        }
    }

    WireframeSphereMeshBuilder::~WireframeSphereMeshBuilder()
    {
    }

    void WireframeSphereMeshBuilder::Build(const glm::mat4 &transform)
    {
        // 5 rings. One on the X/Z plane, and the other 4 on the X/Y plane.
        this->BuildRing(transform, glm::mat4(),                                                    this->rings[0]);
        this->BuildRing(transform, glm::rotate(glm::radians( 90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), this->rings[1]);
        this->BuildRing(transform, glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), this->rings[2]);
        this->BuildRing(transform, glm::rotate(glm::radians( 45.0f), glm::vec3(0.0f, 1.0f, 0.0f)), this->rings[3]);
        this->BuildRing(transform, glm::rotate(glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f)), this->rings[4]);
    }

    void WireframeSphereMeshBuilder::BuildRing(const glm::mat4 &transform, const glm::mat4 &ringTransform, MeshBuilderP3 &ringOut)
    {
        float circleRadius       = 1.0f;
        float circleSegmentAngle = glm::radians(360.0f / static_cast<float>(this->ringSegmentsCount));

        Vector<glm::vec3> vertices(this->ringSegmentsCount);


        // Important that we clear.
        ringOut.Clear();

        // We create the vertices, but don't want to output them just yet. We need to ignore back-facing segments, which we'll do in the second step.
        for (unsigned int iSegment = 0; iSegment < this->ringSegmentsCount; ++iSegment)
        {
            glm::vec3 position;
            position.x = std::cos(circleSegmentAngle * iSegment) * circleRadius;
            position.y = std::sin(circleSegmentAngle * iSegment) * circleRadius;
            position.z = 0.0f;
            position = glm::vec3(transform * ringTransform * glm::vec4(position, 1.0f));

            vertices.PushBack(position);
        }


        // Now we need to cull any back-facing segments, if applicable.
        for (size_t iVertex = 0; iVertex < vertices.count; ++iVertex)
        {
            size_t index0 = iVertex;
            size_t index1 = (iVertex + 1) % vertices.count;

            ringOut.EmitVertex(vertices[index0]);
            ringOut.EmitVertex(vertices[index1]);
        }
    }



    /// WireframeCylinderMeshBuilder
    WireframeCylinderMeshBuilder::WireframeCylinderMeshBuilder(unsigned int ringSegmentsCountIn)
        : ringSegmentsCount(ringSegmentsCountIn)
    {
    }

    WireframeCylinderMeshBuilder::~WireframeCylinderMeshBuilder()
    {
    }

    void WireframeCylinderMeshBuilder::Build(float radius, float length, const glm::mat4 &transform)
    {
        this->Clear();

        // Rings.
        this->CreateRing(radius, transform * glm::translate(glm::vec3(0.0f, 0.0f,  length * 0.5f)));
        this->CreateRing(radius, transform * glm::translate(glm::vec3(0.0f, 0.0f, -length * 0.5f)));

        // Connection lines.
        int   connectionCount = 8;
        float connectionAngle = glm::radians(360.0f / static_cast<float>(connectionCount));

        for (int iLine = 0; iLine < connectionCount; ++iLine)
        {
            float x = std::cos(connectionAngle * iLine) * radius;
            float y = std::sin(connectionAngle * iLine) * radius;

            glm::vec3 vertex0 = glm::vec3(transform * glm::vec4(x, y,  length * 0.5f, 1.0f));
            glm::vec3 vertex1 = glm::vec3(transform * glm::vec4(x, y, -length * 0.5f, 1.0f));

            this->EmitVertex(vertex0);
            this->EmitVertex(vertex1);
        }
    }

    void WireframeCylinderMeshBuilder::CreateRing(float radius, const glm::mat4 &transform)
    {
        float circleSegmentAngle = glm::radians(360.0f / static_cast<float>(this->ringSegmentsCount));

        Vector<glm::vec3> vertices(this->ringSegmentsCount);
        for (unsigned int iSegment = 0; iSegment < this->ringSegmentsCount; ++iSegment)
        {
            glm::vec3 position;
            position.x = std::cos(circleSegmentAngle * iSegment) * radius;
            position.y = std::sin(circleSegmentAngle * iSegment) * radius;
            position.z = 0.0f;
            position = glm::vec3(transform * glm::vec4(position, 1.0f));

            vertices.PushBack(position);
        }

        for (size_t iVertex = 0; iVertex < vertices.count; ++iVertex)
        {
            size_t index0 = iVertex;
            size_t index1 = (iVertex + 1) % vertices.count;

            this->EmitVertex(vertices[index0]);
            this->EmitVertex(vertices[index1]);
        }
    }



    /// WireframeCapsuleMeshBuilder
    WireframeCapsuleMeshBuilder::WireframeCapsuleMeshBuilder(unsigned int ringSegmentsCountIn)
        : circleSegmentsCount(ringSegmentsCountIn)
    {
    }

    WireframeCapsuleMeshBuilder::~WireframeCapsuleMeshBuilder()
    {
    }

    void WireframeCapsuleMeshBuilder::Build(float radius, float length, const glm::mat4 &transform)
    {
        this->Clear();

        // Rings.
        this->CreateRing(radius, length, transform);
        this->CreateRing(radius, length, transform * glm::rotate(glm::radians(45.0f),  glm::vec3(0.0f, 0.0f, 1.0f)));
        this->CreateRing(radius, length, transform * glm::rotate(glm::radians(90.0f),  glm::vec3(0.0f, 0.0f, 1.0f)));
        this->CreateRing(radius, length, transform * glm::rotate(glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    }

    void WireframeCapsuleMeshBuilder::CreateRing(float radius, float length, const glm::mat4 &transform)
    {
        float circleSegmentAngle = glm::radians(180.0f / static_cast<float>(this->circleSegmentsCount - 1));

        Vector<glm::vec3> vertices(this->circleSegmentsCount);
        for (unsigned int iSegment = 0; iSegment < this->circleSegmentsCount; ++iSegment)
        {
            glm::vec3 position;
            position.x = std::cos(circleSegmentAngle * iSegment - glm::radians(180.0f)) * radius;
            position.y = 0.0f;
            position.z = std::sin(circleSegmentAngle * iSegment - glm::radians(180.0f)) * radius - length * 0.5f;
            position = glm::vec3(transform * glm::vec4(position, 1.0f));

            vertices.PushBack(position);
        }

        for (size_t iVertex = 0; iVertex < vertices.count - 1; ++iVertex)
        {
            size_t index0 = iVertex;
            size_t index1 = (iVertex + 1) % vertices.count;

            this->EmitVertex(vertices[index0]);
            this->EmitVertex(vertices[index1]);
        }


        vertices.Clear();
        for (unsigned int iSegment = 0; iSegment < this->circleSegmentsCount; ++iSegment)
        {
            glm::vec3 position;
            position.x = std::cos(circleSegmentAngle * iSegment) * radius;
            position.y = 0.0f;
            position.z = std::sin(circleSegmentAngle * iSegment) * radius + length * 0.5f;
            position = glm::vec3(transform * glm::vec4(position, 1.0f));

            vertices.PushBack(position);
        }

        for (size_t iVertex = 0; iVertex < vertices.count - 1; ++iVertex)
        {
            size_t index0 = iVertex;
            size_t index1 = (iVertex + 1) % vertices.count;

            this->EmitVertex(vertices[index0]);
            this->EmitVertex(vertices[index1]);
        }


        // Connectors.
        glm::vec3 negVertex0 = glm::vec3(transform * glm::vec4(-radius, 0.0f, -length * 0.5f, 1.0f));
        glm::vec3 negVertex1 = glm::vec3(transform * glm::vec4(-radius, 0.0f,  length * 0.5f, 1.0f));
        this->EmitVertex(negVertex0);
        this->EmitVertex(negVertex1);

        glm::vec3 posVertex0 = glm::vec3(transform * glm::vec4(radius, 0.0f, -length * 0.5f, 1.0f));
        glm::vec3 posVertex1 = glm::vec3(transform * glm::vec4(radius, 0.0f,  length * 0.5f, 1.0f));
        this->EmitVertex(posVertex0);
        this->EmitVertex(posVertex1);
    }



    /// CollisionShapeWireframeMeshBuilder
    WireframeCollisionShapeMeshBuilder::WireframeCollisionShapeMeshBuilder(unsigned int circleSegmentsCountIn)
        : circleSegmentsCount(circleSegmentsCountIn)
    {
        this->checkDuplicatesOnEmit = false;
    }

    WireframeCollisionShapeMeshBuilder::~WireframeCollisionShapeMeshBuilder()
    {
    }

    void WireframeCollisionShapeMeshBuilder::Build(const btCollisionShape &shape, const glm::mat4 &cameraView)
    {
        this->Clear();
        this->BuildAndMerge(shape, cameraView);
    }

    void WireframeCollisionShapeMeshBuilder::BuildAndMerge(const btCollisionShape &shape, const glm::mat4 &cameraView, const glm::mat4 &transform)
    {
        if (shape.getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
        {
            auto &compound = static_cast<const btCompoundShape &>(shape);

            // Create the child shapes.
            for (int iShape = 0; iShape < compound.getNumChildShapes(); ++iShape)
            {
                auto childShape = compound.getChildShape(iShape);
                assert(childShape != nullptr);
                {
                    this->BuildAndMerge(*childShape, cameraView, Math::mat4_cast(compound.getChildTransform(iShape)));
                }
            }
        }
        else if (shape.getShapeType() == BOX_SHAPE_PROXYTYPE)
        {
            WireframeBoxMeshBuilder mesh;
            mesh.Build(Math::vec3_cast(static_cast<const btBoxShape &>(shape).getHalfExtentsWithMargin()), transform);

            this->Merge(mesh);
        }
        else if (shape.getShapeType() == SPHERE_SHAPE_PROXYTYPE)
        {
            WireframeSphereMeshBuilder mesh(this->circleSegmentsCount);
            mesh.Build(transform * glm::scale(glm::vec3(static_cast<const btSphereShape &>(shape).getRadius())));

            for (size_t iRing = 0; iRing < mesh.GetRingCount(); ++iRing)
            {
                this->Merge(mesh.GetRing(iRing));
            }
        }
        else if (shape.getShapeType() == CUSTOM_CONVEX_SHAPE_TYPE)          // Ellipsoid. This should be given a proper type.
        {
            auto &ellipsoid = static_cast<const btEllipsoidShape &>(shape);

            WireframeSphereMeshBuilder mesh(this->circleSegmentsCount);
            mesh.Build(transform * glm::scale(Math::vec3_cast(ellipsoid.getImplicitShapeDimensions() + btVector3(ellipsoid.getMargin(), ellipsoid.getMargin(), ellipsoid.getMargin()))));

            for (size_t iRing = 0; iRing < mesh.GetRingCount(); ++iRing)
            {
                this->Merge(mesh.GetRing(iRing));
            }
        }
        else if (shape.getShapeType() == CAPSULE_SHAPE_PROXYTYPE)
        {
            auto &capsule = static_cast<const btCapsuleShape &>(shape);

            float length = capsule.getHalfHeight() * 2.0f;
            glm::mat4 orientation;

            if (capsule.getUpAxis() == 0)          // X
            {
                orientation = glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else if (capsule.getUpAxis() == 1)     // Y
            {
                orientation = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            }
            else if (capsule.getUpAxis() == 2)     // Z
            {
                orientation = glm::mat4();
            }
            else
            {
                length = 1.0f;      // <-- Warning silencer.
                assert(false);
            }

            WireframeCapsuleMeshBuilder mesh(this->circleSegmentsCount / 2);
            mesh.Build(capsule.getRadius(), length, transform * orientation);

            this->Merge(mesh);
        }
        else if (shape.getShapeType() == CYLINDER_SHAPE_PROXYTYPE)
        {
            auto &cylinder = static_cast<const btCylinderShape &>(shape);

            float length;
            glm::mat4 orientation;

            if (cylinder.getUpAxis() == 0)          // X
            {
                length      = cylinder.getHalfExtentsWithMargin().x() * 2.0f;
                orientation = glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else if (cylinder.getUpAxis() == 1)     // Y
            {
                length      = cylinder.getHalfExtentsWithMargin().y() * 2.0f;
                orientation = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            }
            else if (cylinder.getUpAxis() == 2)     // Z
            {
                length      = cylinder.getHalfExtentsWithMargin().z() * 2.0f;
                orientation = glm::mat4();
            }
            else
            {
                length = 1.0f;      // <-- Warning silencer.
                assert(false);
            }

            WireframeCylinderMeshBuilder mesh(this->circleSegmentsCount);
            mesh.Build(cylinder.getRadius(), length, transform * orientation);

            this->Merge(mesh);
        }
        else if (shape.getShapeType() == CONVEX_HULL_SHAPE_PROXYTYPE)
        {
            auto &convexHullShape = static_cast<const btConvexHullShape &>(shape);

            btShapeHull hull(&convexHullShape);
            hull.buildHull(convexHullShape.getMargin());

            if (hull.numIndices() > 0 && hull.numVertices() > 0)
            {
                auto indices  = hull.getIndexPointer();
                auto vertices = hull.getVertexPointer();

                for (int iIndex = 0; iIndex < hull.numIndices(); iIndex += 3)
                {
                    auto index0 = indices[iIndex + 0];
                    auto index1 = indices[iIndex + 1];
                    auto index2 = indices[iIndex + 2];

                    auto vertex0 = Math::vec3_cast(vertices[index0]);
                    auto vertex1 = Math::vec3_cast(vertices[index1]);
                    auto vertex2 = Math::vec3_cast(vertices[index2]);

                    this->EmitVertex(vertex0); this->EmitVertex(vertex1);
                    this->EmitVertex(vertex1); this->EmitVertex(vertex2);
                    this->EmitVertex(vertex2); this->EmitVertex(vertex0);
                }
            }
        }
    }



    // BoxCollisionShapeMeshBuilder
    BoxCollisionShapeMeshBuilder::BoxCollisionShapeMeshBuilder()
        : MeshBuilderP3()
    {
    }

    BoxCollisionShapeMeshBuilder::~BoxCollisionShapeMeshBuilder()
    {
    }

    void BoxCollisionShapeMeshBuilder::Build(const btBoxShape &shape, const glm::mat4 &transform)
    {
        const btVector3 halfExtents = shape.getImplicitShapeDimensions();

        glm::vec3 positions[8];
        positions[0] = glm::vec3(transform * glm::vec4(-halfExtents.x(),  halfExtents.y(),  halfExtents.z(), 1.0f));
        positions[1] = glm::vec3(transform * glm::vec4(-halfExtents.x(), -halfExtents.y(),  halfExtents.z(), 1.0f));
        positions[2] = glm::vec3(transform * glm::vec4( halfExtents.x(), -halfExtents.y(),  halfExtents.z(), 1.0f));
        positions[3] = glm::vec3(transform * glm::vec4( halfExtents.x(),  halfExtents.y(),  halfExtents.z(), 1.0f));
        positions[4] = glm::vec3(transform * glm::vec4(-halfExtents.x(),  halfExtents.y(), -halfExtents.z(), 1.0f));
        positions[5] = glm::vec3(transform * glm::vec4(-halfExtents.x(), -halfExtents.y(), -halfExtents.z(), 1.0f));
        positions[6] = glm::vec3(transform * glm::vec4( halfExtents.x(), -halfExtents.y(), -halfExtents.z(), 1.0f));
        positions[7] = glm::vec3(transform * glm::vec4( halfExtents.x(),  halfExtents.y(), -halfExtents.z(), 1.0f));


        // Front.
        this->EmitVertex(positions[0]); this->EmitVertex(positions[1]); this->EmitVertex(positions[2]);
        this->EmitVertex(positions[2]); this->EmitVertex(positions[3]); this->EmitVertex(positions[0]);

        // Right.
        this->EmitVertex(positions[3]); this->EmitVertex(positions[2]); this->EmitVertex(positions[6]);
        this->EmitVertex(positions[6]); this->EmitVertex(positions[7]); this->EmitVertex(positions[3]);

        // Back.
        this->EmitVertex(positions[7]); this->EmitVertex(positions[6]); this->EmitVertex(positions[5]);
        this->EmitVertex(positions[5]); this->EmitVertex(positions[4]); this->EmitVertex(positions[7]);

        // Left.
        this->EmitVertex(positions[4]); this->EmitVertex(positions[5]); this->EmitVertex(positions[1]);
        this->EmitVertex(positions[1]); this->EmitVertex(positions[0]); this->EmitVertex(positions[4]);

        // Top.
        this->EmitVertex(positions[4]); this->EmitVertex(positions[0]); this->EmitVertex(positions[3]);
        this->EmitVertex(positions[3]); this->EmitVertex(positions[7]); this->EmitVertex(positions[4]);

        // Bottom.
        this->EmitVertex(positions[1]); this->EmitVertex(positions[5]); this->EmitVertex(positions[6]);
        this->EmitVertex(positions[6]); this->EmitVertex(positions[2]); this->EmitVertex(positions[1]);
    }


    // ConvexHullCollisionShapeMeshBuilder
    ConvexHullCollisionShapeMeshBuilder::ConvexHullCollisionShapeMeshBuilder()
        : MeshBuilderP3()
    {
    }

    ConvexHullCollisionShapeMeshBuilder::~ConvexHullCollisionShapeMeshBuilder()
    {
    }

    void ConvexHullCollisionShapeMeshBuilder::Build(const btConvexHullShape &shape, const glm::mat4 &transform)
    {
        btShapeHull hull(&shape);
        hull.buildHull(shape.getMargin());

        if (hull.numIndices() > 0 && hull.numVertices() > 0)
        {
            auto indices  = hull.getIndexPointer();
            auto vertices = hull.getVertexPointer();

            for (int iIndex = 0; iIndex < hull.numIndices(); iIndex += 3)
            {
                auto index0 = indices[iIndex + 0];
                auto index1 = indices[iIndex + 1];
                auto index2 = indices[iIndex + 2];

                auto vertex0 = glm::vec3(transform * glm::vec4(Math::vec3_cast(vertices[index0]), 1.0f));
                auto vertex1 = glm::vec3(transform * glm::vec4(Math::vec3_cast(vertices[index1]), 1.0f));
                auto vertex2 = glm::vec3(transform * glm::vec4(Math::vec3_cast(vertices[index2]), 1.0f));

                this->EmitVertex(vertex2);
                this->EmitVertex(vertex1);
                this->EmitVertex(vertex0);
            }
        }
    }


    // CollisionShapeMeshBuilder
    CollisionShapeMeshBuilder::CollisionShapeMeshBuilder()
        : MeshBuilderP3()
    {
    }

    CollisionShapeMeshBuilder::~CollisionShapeMeshBuilder()
    {
    }

    void CollisionShapeMeshBuilder::Build(const btCollisionShape &shape, const glm::mat4 &transform)
    {
        this->BuildAndMerge(shape, transform);
    }

    void CollisionShapeMeshBuilder::BuildAndMerge(const btCollisionShape &shape, const glm::mat4 &transform)
    {
        if (shape.getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
        {
            auto &compound = static_cast<const btCompoundShape &>(shape);

            // Create the child shapes.
            for (int iShape = 0; iShape < compound.getNumChildShapes(); ++iShape)
            {
                auto childShape = compound.getChildShape(iShape);
                assert(childShape != nullptr);
                {
                    this->BuildAndMerge(*childShape, transform * Math::mat4_cast(compound.getChildTransform(iShape)));
                }
            }
        }
        else if (shape.getShapeType() == BOX_SHAPE_PROXYTYPE)
        {
            auto &boxShape = static_cast<const btBoxShape &>(shape);

            BoxCollisionShapeMeshBuilder builder;
            builder.Build(boxShape, transform);

            this->Merge(builder);
        }
        else if (shape.getShapeType() == SPHERE_SHAPE_PROXYTYPE)
        {
        }
        else if (shape.getShapeType() == CUSTOM_CONVEX_SHAPE_TYPE)          // Ellipsoid. This should be given a proper type.
        {
        }
        else if (shape.getShapeType() == CAPSULE_SHAPE_PROXYTYPE)
        {
        }
        else if (shape.getShapeType() == CYLINDER_SHAPE_PROXYTYPE)
        {
        }
        else if (shape.getShapeType() == CONVEX_HULL_SHAPE_PROXYTYPE)
        {
            auto &convexHullShape = static_cast<const btConvexHullShape &>(shape);

            ConvexHullCollisionShapeMeshBuilder builder;
            builder.Build(convexHullShape, transform);

            this->Merge(builder);
        }
    }
}
