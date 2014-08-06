// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

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
            Renderer::DeleteVertexArray(this->geometry);
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
            Renderer::DeleteVertexArray(this->geometry);
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
            return this->skinningData->skinnedGeometry;
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
        glm::vec3 devnull0;
        glm::vec3 devnull1;
        this->ApplySkinning(devnull0, devnull1);
    }

    void Mesh::ApplySkinning(glm::vec3 &aabbMinOut, glm::vec3 &aabbMaxOut)
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

                // After executing, we need the AABBs.
                shader.GetAABB(aabbMinOut, aabbMaxOut);
            }


            this->geometry->UnmapVertexData();
            dstVertexArray->UnmapVertexData();

            this->hasAnimated = true;
        }
    }

    bool Mesh::IsAnimated() const
    {
        return this->skinningData != nullptr;
    }



    void Mesh::Serialize(GTLib::Serializer &serializer, bool serializeGeometry) const
    {
        // We'll write the material chunk first.
        if (this->material != nullptr && !this->material->GetDefinition().relativePath.IsEmpty())
        {
            GTLib::BasicSerializer materialSerializer;
            
            materialSerializer.WriteString(this->material->GetDefinition().relativePath);
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
            GTLib::BasicSerializer geometrySerializer;

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

    void Mesh::Deserialize(GTLib::Deserializer &deserializer)
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
                            GTLib::String materialPath;
                            deserializer.ReadString(materialPath);

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
                                Renderer::DeleteVertexArray(this->geometry);
                            }


                            auto newVA = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat());
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
