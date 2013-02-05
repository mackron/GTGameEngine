// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/Logging.hpp>

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


        this->OnChanged();
    }

    void ModelComponent::SetModel(Model &model, bool takeOwnership)
    {
        this->SetModel(&model, takeOwnership);
    }

    Model* ModelComponent::SetModel(const char* fileName, const char* relativeTo)
    {
        this->SetModel(ModelLibrary::LoadFromFile(fileName, relativeTo), true);
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


        this->OnChanged();
    }


    void ModelComponent::EnableShadowCasting()
    {
        this->flags |= CastShadow;
        this->OnChanged();
    }

    void ModelComponent::DisableShadowCasting()
    {
        this->flags &= ~CastShadow;
        this->OnChanged();
    }



    void ModelComponent::Serialize(GTCore::Serializer &serializer) const
    {
        // The model component is simple. We have only a single chunk here. The content on this chunk will depend on the state of the
        // model. If we have a model that was loaded from a file, we need to save the file name. If it was not loaded from a file (a
        // procedural model), we obviously don't want to save the file name.
        //
        // As usual, the header of this chunk needs an exact data size, so we'll need to use an intermediary serializer.

        GTCore::BasicSerializer intermediarySerializer;

        // We'll save the flags first.
        intermediarySerializer.Write(static_cast<uint32_t>(this->flags));
        
        // Now we save the model. It's possible that there isn't any model set at the moment, so we need to write a boolean indicating
        // whether or not we are saving one.
        if (this->model != nullptr)
        {
            intermediarySerializer.Write(true);
            intermediarySerializer.Write(this->model->GetDefinition().fileName);

            this->model->Serialize(intermediarySerializer);
        }
        else
        {
            intermediarySerializer.Write(false);
        }


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_ModelComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void ModelComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);

        assert(header.id == Serialization::ChunkID_ModelComponent_Main);
        {
            switch (header.version)
            {
            case 1:
                {
                    // Flags are first.
                    deserializer.Read(static_cast<uint32_t &>(this->flags));

                    // Next is a boolean indicating whether or not a model is defined here.
                    bool hasModel;
                    deserializer.Read(hasModel);

                    // We will only have additional data at this point if we have actually have a model defined.
                    if (hasModel)
                    {
                        GTCore::String modelPath;
                        deserializer.Read(modelPath);

                        if (!modelPath.IsEmpty())
                        {
                            this->SetModel(modelPath.c_str());
                        }
                        else
                        {
                            this->SetModel(new Model, true);
                        }

                        this->model->Deserialize(deserializer);
                    }


                    break;
                }

            default:
                {
                    GTEngine::Log("Error deserializing ModelComponent. Main chunk has an unsupported version (%d).", header.version);
                    break;
                }
            }
        }


        this->OnChanged();
    }
}


