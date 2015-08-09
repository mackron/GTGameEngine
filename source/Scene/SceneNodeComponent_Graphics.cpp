// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeComponent_Graphics.hpp>
#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    //SceneNodeComponent_Graphics::SceneNodeComponent_Graphics(GraphicsResourceManager* pGraphicsResourceManager)
    SceneNodeComponent_Graphics::SceneNodeComponent_Graphics(GraphicsWorld* pGraphicsWorld)
        : SceneNodeComponent(SceneNodeComponentType_Graphics),
          m_pGraphicsWorld(pGraphicsWorld),
          m_hMeshObject(0)
    {
    }

    SceneNodeComponent_Graphics::~SceneNodeComponent_Graphics()
    {
    }


    bool SceneNodeComponent_Graphics::SetModel(const char* modelPath, EngineContext &engineContext)
    {
        auto &assetLibrary = engineContext.GetAssetLibrary();
        
        auto pModelAsset = assetLibrary.Load(modelPath);
        if (pModelAsset != nullptr)
        {
            
        }

        return false;
    }


    void SceneNodeComponent_Graphics::SetMeshObject(HGraphicsObject hMeshObject)
    {
        m_hMeshObject = hMeshObject;
    }

    HGraphicsObject SceneNodeComponent_Graphics::GetMeshObject() const
    {
        return m_hMeshObject;
    }
}
