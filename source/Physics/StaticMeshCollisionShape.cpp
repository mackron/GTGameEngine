// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Physics/StaticMeshCollisionShape.hpp>

namespace GT
{
    StaticMeshCollisionShape::StaticMeshCollisionShape()
        : btBvhTriangleMeshShape(new btTriangleIndexVertexArray, true, false)
    {
    }

    StaticMeshCollisionShape::StaticMeshCollisionShape(const Model &model)
        : btBvhTriangleMeshShape(new btTriangleIndexVertexArray, true, false)
    {
        this->AddFromModel(model);
    }

    StaticMeshCollisionShape::StaticMeshCollisionShape(btTriangleIndexVertexArray* meshInterface, bool buildBvh)
        : btBvhTriangleMeshShape(meshInterface, true, buildBvh)
    {
    }


    StaticMeshCollisionShape::~StaticMeshCollisionShape()
    {
        delete this->getMeshInterface();
    }



    void StaticMeshCollisionShape::Reset()
    {
        // We're going to do something evil here. We're going to destruct and then re-construct the entire shape. This is bad, but I can't see any other way to do this...
        this->~StaticMeshCollisionShape();
        new (this) StaticMeshCollisionShape();
    }


    void StaticMeshCollisionShape::AddFromModel(const Model &model)
    {
        auto vertexArray = reinterpret_cast<btTriangleIndexVertexArray*>(this->getMeshInterface());
        assert(vertexArray != nullptr);
        {
            vertexArray->getIndexedMeshArray().clear();


            bool buildBvh = false;

            // We need to add a btIndexedMesh for each mesh in the model.
            for (size_t i = 0; i < model.meshes.count; ++i)
            {
                if (model.meshes[i]->GetDrawMode() == DrawMode_Triangles)
                {
                    auto mesh = model.meshes[i]->GetGeometry();

                    btIndexedMesh meshBt;
                    meshBt.m_numTriangles        = mesh->GetIndexCount() / 3;
                    meshBt.m_triangleIndexBase   = reinterpret_cast<const unsigned char*>(mesh->GetIndexDataPtr());
                    meshBt.m_triangleIndexStride = sizeof(unsigned int) * 3;
                    meshBt.m_numVertices         = mesh->GetVertexCount();
                    meshBt.m_vertexBase          = reinterpret_cast<const unsigned char*>(mesh->GetVertexDataPtr());
                    meshBt.m_vertexStride        = mesh->GetFormat().GetSizeInBytes();
                    meshBt.m_indexType           = PHY_INTEGER;
                    meshBt.m_vertexType          = PHY_FLOAT;
                    vertexArray->addIndexedMesh(meshBt);

                    buildBvh = true;
                }
            }

            // This is some more evilness. I don't know how to apply a new mesh interface to the shape, so I'm just going to destruct and then reconstruct.
            this->btBvhTriangleMeshShape::~btBvhTriangleMeshShape();
            new (this) StaticMeshCollisionShape(vertexArray, buildBvh);
        }
    }
}
