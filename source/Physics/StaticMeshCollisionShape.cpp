
#include <GTEngine/Physics/StaticMeshCollisionShape.hpp>

namespace GTEngine
{
    StaticMeshCollisionShape::StaticMeshCollisionShape()
        : btBvhTriangleMeshShape(new btTriangleIndexVertexArray, false, false)
    {
    }

    StaticMeshCollisionShape::StaticMeshCollisionShape(const Model &model)
        : btBvhTriangleMeshShape(new btTriangleIndexVertexArray, false, false)
    {
        this->AddFromModel(model);
    }

    StaticMeshCollisionShape::~StaticMeshCollisionShape()
    {
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
            // We need to add a btIndexedMesh for each mesh in the model.
            for (size_t i = 0; i < model.meshes.count; ++i)
            {
                auto mesh = model.meshes[i]->GetGeometry();

                btIndexedMesh meshBt;
                meshBt.m_numTriangles        = mesh->GetVertexCount() / 3;
                meshBt.m_triangleIndexBase   = reinterpret_cast<const unsigned char*>(mesh->GetIndexDataPtr());
                meshBt.m_triangleIndexStride = sizeof(unsigned int) * 3;
                meshBt.m_numVertices         = mesh->GetVertexCount();
                meshBt.m_vertexBase          = reinterpret_cast<const unsigned char*>(mesh->GetVertexDataPtr());
                meshBt.m_vertexStride        = mesh->GetFormat().GetSizeInBytes();
                meshBt.m_indexType           = PHY_INTEGER;
                meshBt.m_vertexType          = PHY_FLOAT;
                vertexArray->addIndexedMesh(meshBt);
            }

            // This is some more evilness. I don't know how to apply a new mesh interface to the shape, so I'm just going to destruct and then reconstruct.
            btBvhTriangleMeshShape::~btBvhTriangleMeshShape();
            new (this) btBvhTriangleMeshShape(vertexArray, false, model.meshes.count > 0);
        }
    }
}