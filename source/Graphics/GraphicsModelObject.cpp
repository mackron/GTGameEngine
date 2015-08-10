// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsModelObject.hpp>
#include <GTGameEngine/Graphics/GraphicsAssetResourceManager.hpp>

namespace GT
{
    GraphicsModelObject::GraphicsModelObject()
        : m_pModelResource(nullptr), m_pGraphicsWorld(nullptr), m_meshes()
    {
    }

    GraphicsModelObject::~GraphicsModelObject()
    {
        this->Uninitialize();
    }


    bool GraphicsModelObject::Initialize(const GraphicsAssetResource_Model* pModelResource, GraphicsWorld* pGraphicsWorld, const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        if (pModelResource != nullptr && pGraphicsWorld != nullptr)
        {
            if (m_pModelResource == nullptr)
            {
                size_t meshCount = pModelResource->GetMeshCount();
                m_meshes.Reserve(meshCount);
                
                for (size_t iMesh = 0; iMesh < meshCount; ++iMesh)
                {
                    MeshObject newMesh;
                    newMesh.localPosition   = position;
                    newMesh.localRotation   = rotation;
                    newMesh.localScale      = scale;
                    newMesh.pMeshResource   = pModelResource->GetMeshByIndex(iMesh);
                    newMesh.hGraphicsObject = pGraphicsWorld->CreateMeshObject(newMesh.pMeshResource->GetGraphicsResource(), position, rotation, scale);
                    m_meshes.PushBack(newMesh);
                }

                m_pModelResource = pModelResource;
                m_pGraphicsWorld = pGraphicsWorld;

                return true;
            }
            else
            {
                // Already initialized.
                return true;
            }
        }
        else
        {
            // Invalid arguments.
            return false;
        }
    }

    void GraphicsModelObject::Uninitialize()
    {
        if (m_pModelResource != nullptr && m_pGraphicsWorld != nullptr)
        {
            for (size_t iMesh = 0; iMesh < m_meshes.GetCount(); ++iMesh)
            {
                auto &mesh = m_meshes[iMesh];
                m_pGraphicsWorld->DeleteObject(mesh.hGraphicsObject);
            }

            m_meshes.Clear();
        }
    }


    void GraphicsModelObject::SetTransform(const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        // TODO: At the moment this assumes that all meshes are root level, but later on we'll want to check it properly and ensure
        //       the local positions of actual root level meshes are changed. When a root level mesh changes position the absolute
        //       position of it's children need to be updated.
        
        for (size_t iMesh = 0; iMesh < m_meshes.GetCount(); ++iMesh)    // TODO: Only iterate over root level elements.
        {
            auto &mesh = m_meshes[iMesh];
            mesh.localPosition = position;
            mesh.localRotation = rotation;
            mesh.localScale    = scale;

            if (mesh.hGraphicsObject != 0 && m_pGraphicsWorld != nullptr)
            {
                m_pGraphicsWorld->SetObjectPosition(mesh.hGraphicsObject, mesh.localPosition);
                m_pGraphicsWorld->SetObjectRotation(mesh.hGraphicsObject, mesh.localRotation);
                m_pGraphicsWorld->SetObjectScale(mesh.hGraphicsObject, mesh.localScale);
            }

            // TODO: Update the absolute transforms of the meshes children.
        }
    }
}
