
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/ModelLibrary.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(ModelComponent, "Model");

    ModelComponent::ModelComponent(SceneNode &node)
        : Component(node), model(nullptr), isOwner(false)
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
    }

    Model* ModelComponent::GetModel()
    {
        return this->model;
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


