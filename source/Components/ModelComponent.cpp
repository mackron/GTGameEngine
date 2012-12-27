
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(ModelComponent, "Model");

    ModelComponent::ModelComponent(SceneNode &node)
        : Component(node), model(nullptr),
          flags(CullBackFaces | CastShadow | Visible),
          wireframeColour(1.0f, 0.75f, 0.5f), drawWireframe(false)
    {
    }

    ModelComponent::~ModelComponent()
    {
        if (this->flags & Owner)
        {
            ModelLibrary::Delete(this->model);
        }
    }

    void ModelComponent::SetModel(Model* model, bool takeOwnership)
    {
        if (this->flags & Owner)
        {
            ModelLibrary::Delete(this->model);
        }
        
        this->model   = model;

        if (takeOwnership)
        {
            this->flags |= Owner;
        }
        else
        {
            this->flags &= ~Owner;
        }

        // This component has changed. We need to let the scene know about this so that it can change culling information and whatnot.
        auto scene = this->GetNode().GetScene();
        if (scene != nullptr)
        {
            scene->OnSceneNodeComponentChanged(this->GetNode(), *this);
        }
    }

    Model* ModelComponent::SetModel(const char* fileName)
    {
        this->SetModel(ModelLibrary::LoadFromFile(fileName), true);
        return this->model;
    }

    void ModelComponent::UnsetModel()
    {
        this->SetModel(static_cast<Model*>(nullptr));
    }


    void ModelComponent::SetFaceCulling(bool cullFront, bool cullBack)
    {
        if (cullFront)
        {
            this->flags |= CullFrontFaces;
        }
        else
        {
            this->flags &= ~CullFrontFaces;
        }

        if (cullBack)
        {
            this->flags |= CullBackFaces;
        }
        else
        {
            this->flags &= ~CullBackFaces;
        }
    }


    void ModelComponent::EnableShadowCasting()
    {
        this->flags |= CastShadow;
    }

    void ModelComponent::DisableShadowCasting()
    {
        this->flags &= ~CastShadow;
    }
}


