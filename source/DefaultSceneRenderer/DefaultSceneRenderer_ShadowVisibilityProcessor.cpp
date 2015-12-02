// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/DefaultSceneRenderer/DefaultSceneRenderer_ShadowVisibilityProcessor.hpp>

namespace GT
{
    DefaultSceneRenderer_ShadowVisibilityProcessor::DefaultSceneRenderer_ShadowVisibilityProcessor()
        : meshes()
    {
    }

    DefaultSceneRenderer_ShadowVisibilityProcessor::~DefaultSceneRenderer_ShadowVisibilityProcessor()
    {
    }

    
    void DefaultSceneRenderer_ShadowVisibilityProcessor::ProcessModel(const SceneNode &sceneNode)
    {
        auto modelComponent = sceneNode.GetComponent<ModelComponent>();
        assert(modelComponent != nullptr);
        {
            if (modelComponent->IsShadowCastingEnabled())       // <-- Ignore the model if it is not casting shadows.
            {
                auto model = modelComponent->GetModel();
                if (model != nullptr)                           // <-- Is allowed to be null. Perhaps due to a bad path?
                {
                    for (size_t i = 0; i < model->meshes.count; ++i)
                    {
                        auto mesh = model->meshes[i];
                        assert(mesh != nullptr);
                        {
                            DefaultSceneRendererMesh object;
                            object.vertexArray = mesh->GetSkinnedGeometry();
                            object.drawMode    = mesh->GetDrawMode();
                            object.material    = mesh->GetMaterial();
                            object.transform   = sceneNode.GetWorldTransform();
                            this->meshes.PushBack(object);
                        }
                    }
                }
            }
        }
    }
}