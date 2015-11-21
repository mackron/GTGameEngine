// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(ModelComponent, "Model")

    const uint32_t ModelComponent::ChangeFlag_Flags    = (1 << 0);
    const uint32_t ModelComponent::ChangeFlag_Model    = (1 << 1);
    const uint32_t ModelComponent::ChangeFlag_Material = (1 << 2);

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

    void ModelComponent::SetModel(Model* newModel, bool takeOwnership)
    {
        // Don't do anything if the models are the same.
        if (this->model != nullptr && newModel != nullptr && this->model->GetDefinition().GetAbsolutePath() == newModel->GetDefinition().GetAbsolutePath())
        {
            return;
        }



        if (this->flags & Owner)
        {
            ModelLibrary::Delete(this->model);
        }

        this->model = newModel;

        if (takeOwnership)
        {
            this->flags |= Owner;
        }
        else
        {
            this->flags &= ~Owner;
        }


        this->OnChanged(ChangeFlag_Flags | ChangeFlag_Model);
    }

    void ModelComponent::SetModel(Model &modelIn, bool takeOwnership)
    {
        this->SetModel(&modelIn, takeOwnership);
    }

    Model* ModelComponent::SetModel(const char* fileName, const char* relativeTo)
    {
        this->SetModel(ModelLibrary::Create(fileName, relativeTo), true);
        return this->model;
    }

    void ModelComponent::UnsetModel()
    {
        this->SetModel(static_cast<Model*>(nullptr));
    }


    bool ModelComponent::SetMaterial(size_t materialIndex, const char* materialRelativePath)
    {
        if (this->model != nullptr)
        {
            auto oldMaterial = this->model->meshes[materialIndex]->GetMaterial();
            if (oldMaterial != nullptr && oldMaterial->GetDefinition().relativePath == materialRelativePath)
            {
                return true;
            }

            if (this->model->meshes[materialIndex]->SetMaterial(materialRelativePath))
            {
                this->OnChanged(ChangeFlag_Material);
                return true;
            }
        }

        return false;
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


        this->OnChanged(ChangeFlag_Flags);
    }


    void ModelComponent::EnableShadowCasting()
    {
        this->flags |= CastShadow;
        this->OnChanged(ChangeFlag_Flags);
    }

    void ModelComponent::DisableShadowCasting()
    {
        this->flags &= ~CastShadow;
        this->OnChanged(ChangeFlag_Flags);
    }



    void ModelComponent::Serialize(GTLib::Serializer &serializer) const
    {
        // The model component is simple. We have only a single chunk here. The content on this chunk will depend on the state of the
        // model. If we have a model that was loaded from a file, we need to save the file name. If it was not loaded from a file (a
        // procedural model), we obviously don't want to save the file name.
        //
        // As usual, the header of this chunk needs an exact data size, so we'll need to use an intermediary serializer.

        GTLib::BasicSerializer intermediarySerializer;

        // We'll save the flags first.
        intermediarySerializer.Write(static_cast<uint32_t>(this->flags));

        // Now we save the model. It's possible that there isn't any model set at the moment, so we need to write a boolean indicating
        // whether or not we are saving one.
        if (this->model != nullptr)
        {
            intermediarySerializer.Write(true);
            intermediarySerializer.WriteString(this->model->GetDefinition().GetRelativePath());

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

    void ModelComponent::Deserialize(GTLib::Deserializer &deserializer)
    {
        uint32_t whatChanged = 0;
        this->LockOnChanged();


        Serialization::ChunkHeader header;
        deserializer.Read(header);

        assert(header.id == Serialization::ChunkID_ModelComponent_Main);
        {
            size_t deserializerStart = deserializer.Tell();
            
            switch (header.version)
            {
            case 1:
                {
                    // Flags are first.
                    uint32_t newFlags;
                    deserializer.Read(newFlags);

                    if (newFlags != this->flags)
                    {
                        this->flags = newFlags;
                        whatChanged |= ChangeFlag_Flags;
                    }


                    // Next is a boolean indicating whether or not a model is defined here.
                    bool hasModel;
                    deserializer.Read(hasModel);

                    // We will only have additional data at this point if we have actually have a model defined.
                    if (hasModel)
                    {
                        auto oldModel = this->model;


                        GTLib::String modelPath;
                        deserializer.ReadString(modelPath);

                        if (!modelPath.IsEmpty())
                        {
                            this->SetModel(modelPath.c_str());
                            
                            // If we failed to set the model (most likely due to the file not existing) we need to skip this chunk and return.
                            if (this->model == nullptr)
                            {
                                const size_t bytesReadSoFar = deserializer.Tell() - deserializerStart;
                                
                                deserializer.Seek(header.sizeInBytes - bytesReadSoFar);
                                return;
                            }
                        }
                        else
                        {
                            this->SetModel(new Model, true);
                        }


                        assert(this->model != nullptr);
                        {
                            this->model->Deserialize(deserializer);
                        }


                        if (this->model != oldModel)
                        {
                            whatChanged |= ChangeFlag_Model;
                        }
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


        this->UnlockOnChanged();
        if (whatChanged != 0)
        {
            this->OnChanged(whatChanged);
        }
    }
}


