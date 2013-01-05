
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(ModelComponent, "Model");

    ModelComponent::ModelComponent(SceneNode &node)
        : Component(node), model(nullptr),
          flags(CullBackFaces | CastShadow | Visible)
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

    void ModelComponent::SetModel(Model &model, bool takeOwnership)
    {
        this->SetModel(&model, takeOwnership);
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



    void ModelComponent::Serialize(GTCore::Serializer &serializer) const
    {
        serializer.Write(static_cast<uint32_t>(this->flags));

        // If the model is procedural (not loaded from a file) we will just ignore it. We'll need to store a flag specifying whether or
        // not the file name is stored.
        if (this->model != nullptr && !this->model->GetDefinition().fileName.IsEmpty())
        {
            serializer.Write(true);
            serializer.Write(this->model->GetDefinition().fileName);

            // Now we serialize the model itself.
            this->model->Serialize(serializer);
        }
        else
        {
            serializer.Write(false);
        }
    }

    void ModelComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        deserializer.Read(static_cast<uint32_t &>(this->flags));

        bool hasModel;
        deserializer.Read(hasModel);

        if (hasModel)
        {
            GTCore::String fileName;
            deserializer.Read(fileName);

            this->SetModel(fileName.c_str());

            // With the model loaded, we just deserialize it.
            this->model->Deserialize(deserializer);
        }
    }
}


