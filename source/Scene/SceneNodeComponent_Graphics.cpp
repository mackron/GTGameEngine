// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeComponent_Graphics.hpp>
#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    SceneNodeComponent_Graphics::SceneNodeComponent_Graphics()
        : SceneNodeComponent(SceneNodeComponentType_Graphics),
          m_pGraphicsResourceManager(nullptr),
          m_pModelResource(nullptr),
          m_pModelObject(nullptr)
    {
    }

    SceneNodeComponent_Graphics::~SceneNodeComponent_Graphics()
    {
        this->UnsetModel();
    }


    bool SceneNodeComponent_Graphics::SetModel(const char* modelPath, GraphicsAssetResourceManager* pGraphicsResourceManager)
    {
        this->UnsetModel();

        if (modelPath != nullptr && pGraphicsResourceManager != nullptr)
        {
            m_pModelResource = pGraphicsResourceManager->LoadModel(modelPath);
            if (m_pModelResource != nullptr)
            {
                m_pGraphicsResourceManager = pGraphicsResourceManager;

                // All good. We don't create the graphics objects for each mesh until AddModelToGraphicsWorld() is called.
                return true;
            }
        }
        
        return false;
    }

    void SceneNodeComponent_Graphics::UnsetModel()
    {
        if (m_pModelResource != nullptr && m_pGraphicsResourceManager != nullptr)
        {
            // Make sure the model is removed from the graphics world first.
            this->RemoveModelFromGraphicsWorld();

            // Now we can unload the resources.
            m_pGraphicsResourceManager->Unload(m_pModelResource);
            m_pModelResource           = nullptr;
            m_pGraphicsResourceManager = nullptr;
        }
    }


    void SceneNodeComponent_Graphics::AddModelToGraphicsWorld(const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        if (m_pModelObject == nullptr && m_pModelResource != nullptr)
        {
            m_pModelObject = new GraphicsModelObject;
            m_pModelObject->Initialize(m_pModelResource, &m_pGraphicsResourceManager->GetGraphicsWorld(), position, rotation, scale);
        }
    }

    void SceneNodeComponent_Graphics::RemoveModelFromGraphicsWorld()
    {
        if (m_pModelObject != nullptr)
        {
            m_pModelObject->Uninitialize();

            delete m_pModelObject;
            m_pModelObject = nullptr;
        }
    }

    void SceneNodeComponent_Graphics::SetModelTransform(const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        if (m_pModelObject != nullptr)
        {
            m_pModelObject->SetTransform(position, rotation, scale);
        }
    }
}
