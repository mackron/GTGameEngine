
#include <GTEngine/Mesh.hpp>
#include <GTEngine/CPUVertexShader_Skinning.hpp>
#include <GTEngine/Rendering.hpp>
#include <GTEngine/MaterialLibrary.hpp>

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
        // We'll write a version number just in case we want to change a few things, which is probable.
        serializer.Write(static_cast<uint32_t>(1));

        // We need to save the file name of the material.
        if (this->material && !this->material->GetDefinition().fileName.IsEmpty())
        {
            serializer.Write(true);
            serializer.Write(this->material->GetDefinition().fileName);

            // The material itself needs to be serialized.
            this->material->Serialize(serializer);


            // Geometry, if applicable.
            if (serializeGeometry && this->geometry != nullptr)
            {
                serializer.Write(true);
                this->geometry->Serialize(serializer);
            }
            else
            {
                serializer.Write(false);
            }
        }
        else
        {
            serializer.Write(false);
        }
    }

    void Mesh::Deserialize(GTCore::Deserializer &deserializer)
    {
        uint32_t version;
        deserializer.Read(version);


        bool hasMaterial;
        deserializer.Read(hasMaterial);

        if (hasMaterial)
        {
            GTCore::String fileName;
            deserializer.Read(fileName);

            this->SetMaterial(fileName.c_str());

            // The new material needs to be deserialized now.
            this->material->Deserialize(deserializer);


            // Now the geometry.
            bool deserializeGeometry;
            deserializer.Read(deserializeGeometry);
            
            if (deserializeGeometry)
            {
                if (this->deleteGeometry)
                {
                    GarbageCollector::MarkForCollection(this->geometry);
                }

                this->SetGeometry(new VertexArray(deserializer));
                this->deleteGeometry = true;
            }
        }
    }
}
