// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Mesh.hpp>
#include <GTEngine/CPUVertexShader_Skinning.hpp>
#include <GTEngine/Rendering.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    Mesh::Mesh(DrawMode drawModeIn)
        : geometry(nullptr), material(nullptr),
          skinningData(nullptr),
          hasAnimated(false), deleteGeometry(false), deleteMaterial(false),
          drawMode(drawModeIn)
    {
    }

    Mesh::~Mesh()
    {
        delete this->skinningData;

        if (this->deleteGeometry)
        {
            GarbageCollector::MarkForCollection(this->geometry);
        }

        if (this->deleteMaterial)
        {
            MaterialLibrary::Delete(this->material);
        }
    }


    void Mesh::SetGeometry(VertexArray* newGeometry)
    {
        if (this->deleteGeometry)
        {
            GarbageCollector::MarkForCollection(this->geometry);
        }

        this->geometry       = newGeometry;
        this->deleteGeometry = false;
    }


    void Mesh::SetMaterial(Material* newMaterial)
    {
        if (this->deleteMaterial)
        {
            MaterialLibrary::Delete(this->material);
        }

        this->material       = newMaterial;
        this->deleteMaterial = false;
    }

    bool Mesh::SetMaterial(const char* materialFileName)
    {
        if (this->deleteMaterial)
        {
            MaterialLibrary::Delete(this->material);
        }

        this->deleteMaterial = true;
        this->material       = MaterialLibrary::Create(materialFileName);
        
        return this->material != nullptr;
    }


    const VertexArray* Mesh::GetSkinnedGeometry() const
    {
        return const_cast<Mesh*>(this)->GetSkinnedGeometry();
    }

    VertexArray* Mesh::GetSkinnedGeometry()
    {
        if (this->skinningData != nullptr && this->hasAnimated)
        {
            return this->skinningData->skinnedGeometry[Renderer::BackIndex];
        }
        else
        {
            return this->geometry;
        }
    }


    void Mesh::SetSkinningData(const Bone* const* bones, const SkinningVertexAttribute* skinningVertexAttributes)
    {
        // Delete any previous data just in case. We should never actually have any, but we'll do it anyway just to be sure.
        delete this->skinningData;
        this->skinningData = new MeshSkinningData(bones, skinningVertexAttributes, *this->geometry);
    }


    bool Mesh::GenerateTangentsAndBitangents()
    {
        return this->geometry->GenerateTangentsAndBitangents();
    }



    void Mesh::ApplySkinning()
    {
        if (this->geometry != nullptr)
        {
            auto dstVertexArray = this->GetSkinnedGeometry();

            auto srcVertices = this->geometry->MapVertexData();
            auto dstVertices = dstVertexArray->MapVertexData();

            if (this->skinningData != nullptr)
            {
                CPUVertexShader_Skinning shader;
                shader.SetBoneBuffer(this->skinningData->bones);
                shader.SetSkinningVertexAttributes(this->skinningData->skinningVertexAttributes);

                shader.Execute(srcVertices, this->geometry->GetVertexCount(), this->geometry->GetFormat(), dstVertices);
            }


            this->geometry->UnmapVertexData();
            dstVertexArray->UnmapVertexData();

            this->hasAnimated = true;
        }
    }


    void Mesh::Serialize(GTCore::Serializer &serializer, bool serializeGeometry) const
    {
        // We'll write the material chunk first.
        if (this->material != nullptr && !this->material->GetDefinition().fileName.IsEmpty())
        {
            GTCore::BasicSerializer materialSerializer;
            
            materialSerializer.Write(this->material->GetDefinition().fileName);
            this->material->Serialize(materialSerializer);



            Serialization::ChunkHeader header;
            header.id          = Serialization::ChunkID_Mesh_Material;
            header.version     = 1;
            header.sizeInBytes = materialSerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(materialSerializer.GetBuffer(), header.sizeInBytes);
        }

        // Now the geometry.
        if (this->geometry != nullptr && serializeGeometry)
        {
            GTCore::BasicSerializer geometrySerializer;

            this->geometry->Serialize(geometrySerializer);



            Serialization::ChunkHeader header;
            header.id          = Serialization::ChunkID_Mesh_Geometry;
            header.version     = 1;
            header.sizeInBytes = geometrySerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(geometrySerializer.GetBuffer(), header.sizeInBytes);
        }


        // We want a null chunk at the end so we can do an iteration-based deserializer.
        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_Null;
        header.version     = 1;
        header.sizeInBytes = 0;
        serializer.Write(header);
    }

    void Mesh::Deserialize(GTCore::Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;

        do
        {
            deserializer.Read(header);

            switch (header.id)
            {
            case Serialization::ChunkID_Mesh_Material:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            GTCore::String materialPath;
                            deserializer.Read(materialPath);

                            this->SetMaterial(materialPath.c_str());
                            this->material->Deserialize(deserializer);

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing Mesh. Material chunk is an unsupported version (%d).", header.version);
                            break;
                        }
                    }

                    break;
                }

            case Serialization::ChunkID_Mesh_Geometry:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            if (this->deleteGeometry)
                            {
                                GarbageCollector::MarkForCollection(this->geometry);
                            }


                            auto newVA = new VertexArray(VertexArrayUsage_Static, VertexFormat());
                            newVA->Deserialize(deserializer);

                            this->SetGeometry(newVA);
                            this->deleteGeometry = true;

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing Mesh. Geometry chunk is an unsupported version (%d).", header.version);
                            break;
                        }
                    }

                    break;
                }

            default:
                {
                    // We're not aware of the chunk, so we'll skip it.
                    deserializer.Seek(header.sizeInBytes);

                    break;
                }
            }

        } while (header.id != Serialization::ChunkID_Null);
    }
}
