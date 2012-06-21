
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(ModelComponent, "Model");

    ModelComponent::ModelComponent(SceneNode &node)
        : Component(node), model(nullptr),
          cullFrontFaces(false), cullBackFaces(true),
          castShadow(false),
          isOwner(false)
    {
    }

    ModelComponent::~ModelComponent()
    {
        if (this->isOwner)
        {
            ModelLibrary::Delete(this->model);
        }
    }

    void ModelComponent::SetModel(Model* model, bool takeOwnership)
    {
        this->model   = model;
        this->isOwner = takeOwnership;

        // This component has changed. We need to let the scene know about this so that it can change culling information and whatnot.
        auto scene = this->GetNode().GetScene();
        if (scene != nullptr)
        {
            scene->OnSceneNodeComponentChanged(this->GetNode(), *this);
        }
    }

    Model* ModelComponent::GetModel()
    {
        return this->model;
    }

    void ModelComponent::SetFaceCulling(bool cullFront, bool cullBack)
    {
        this->cullFrontFaces = cullFront;
        this->cullBackFaces  = cullBack;
    }


    void ModelComponent::EnableShadowCasting()
    {
        this->castShadow = true;
    }

    void ModelComponent::DisableShadowCasting()
    {
        this->castShadow = false;
    }

    
    void ModelComponent::MakeOwner()
    {
        this->isOwner = true;
    }

    void ModelComponent::RemoveOwnership()
    {
        this->isOwner = false;
    }
}


