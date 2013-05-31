// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Bone.hpp>

namespace GTEngine
{
    Bone::Bone()
        : parent(nullptr), children(),
          name(), position(), rotation(), scale(1.0f, 1.0f, 1.0f),
          offsetMatrix(), skinningMatrix()
    {
    }

    Bone::Bone(const Bone &other)
        : parent(nullptr), children(),
          name(other.name), position(other.position), rotation(other.rotation), scale(other.scale),
          offsetMatrix(other.offsetMatrix), skinningMatrix(other.skinningMatrix)
    {
    }

    Bone::~Bone()
    {
    }

    void Bone::AttachChild(Bone &child)
    {
        if (child.parent != nullptr)
        {
            child.parent->DetachChild(child);
        }

        child.parent = this;

        this->children.PushBack(&child);
    }

    void Bone::DetachChild(Bone &child)
    {
        if (child.parent == this)
        {
            for (size_t i = 0; i < this->children.count; ++i)
            {
                if (this->children[i] == &child)
                {
                    child.parent = nullptr;
                    this->children.Remove(i);

                    break;
                }
            }
        }
    }


    const glm::vec3 & Bone::GetPosition() const
    {
        return this->position;
    }

    const glm::quat & Bone::GetRotation() const
    {
        return this->rotation;
    }

    const glm::vec3 & Bone::GetScale() const
    {
        return this->scale;
    }


    void Bone::SetPosition(const glm::vec3 &newPosition)
    {
        this->position = newPosition;
    }

    void Bone::SetRotation(const glm::quat &newRotation)
    {
        this->rotation = newRotation;
    }

    void Bone::SetScale(const glm::vec3 &newScale)
    {
        this->scale = newScale;
    }


    glm::vec3 Bone::GetAbsolutePosition() const
    {
        if (this->parent != nullptr)
        {
            glm::quat rotation = this->parent->GetAbsoluteRotation();
            glm::vec3 scale    = this->parent->GetAbsoluteScale();

            return this->parent->GetAbsolutePosition() + (rotation * (scale * this->position));
        }

        return this->position;
    }

    glm::quat Bone::GetAbsoluteRotation() const
    {
        if (this->parent != nullptr)
        {
            return this->parent->GetAbsoluteRotation() * this->rotation;
        }

        return this->rotation;
    }

    glm::vec3 Bone::GetAbsoluteScale() const
    {
        if (this->parent != nullptr)
        {
            return this->parent->GetAbsoluteScale() * this->scale;
        }

        return this->scale;
    }

    
    void Bone::GetAbsoluteTransformComponents(glm::vec3 &positionOut, glm::quat &rotationOut, glm::vec3 &scaleOut) const
    {
        if (this->parent != nullptr)
        {
            this->parent->GetAbsoluteTransformComponents(positionOut, rotationOut, scaleOut);

            positionOut = positionOut + (rotationOut * (scaleOut * this->position));
            rotationOut = rotationOut * this->rotation;
            scaleOut    = scaleOut    * this->scale;
        }
        else
        {
            positionOut = this->position;
            rotationOut = this->rotation;
            scaleOut    = this->scale;
        }
    }



    glm::mat4 Bone::GetTransform() const
    {
        glm::mat4 result;
        Math::CalculateTransformMatrix(this->position, this->rotation, this->scale, result);

        return result;
    }
    
    glm::mat4 Bone::GetAbsoluteTransform() const
    {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
        return this->GetAbsoluteTransform(position, rotation, scale);
    }

    glm::mat4 Bone::GetAbsoluteTransform(glm::vec3 &absolutePositionOut, glm::quat &absoluteRotationOut, glm::vec3 &absoluteScaleOut) const
    {
        this->GetAbsoluteTransformComponents(absolutePositionOut, absoluteRotationOut, absoluteScaleOut);

        glm::mat4 result;
        Math::CalculateTransformMatrix(absolutePositionOut, absoluteRotationOut, absoluteScaleOut, result);

        return result;
    }

    void Bone::UpdateSkinningTransform()
    {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
        this->UpdateSkinningTransform(position, rotation, scale);
    }

    void Bone::UpdateSkinningTransform(glm::vec3 &absolutePositionOut, glm::quat &absoluteRotationOut, glm::vec3 &absoluteScaleOut)
    {
        this->skinningMatrix = this->GetAbsoluteTransform(absolutePositionOut, absoluteRotationOut, absoluteScaleOut) * this->offsetMatrix;
    }

    const glm::mat4 & Bone::GetSkinningTransform() const
    {
        return this->skinningMatrix;
    }


    void Bone::SetOffsetMatrix(const glm::mat4 &offsetMatrix)
    {
        this->offsetMatrix = offsetMatrix;
    }

    const glm::mat4 & Bone::GetOffsetMatrix() const
    {
        return this->offsetMatrix;
    }


    


    Bone* Bone::GetTopLevelBone()
    {
        if (this->parent != nullptr)
        {
            return this->parent->GetTopLevelBone();
        }

        return this;
    }




    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void Bone::Serialize(GTCore::Serializer &serializer) const
    {
        serializer.WriteString(this->name);
        serializer.Write(this->position);
        serializer.Write(this->rotation);
        serializer.Write(this->scale);
        serializer.Write(this->offsetMatrix);
        serializer.Write(this->skinningMatrix);
    }

    void Bone::Deserialize(GTCore::Deserializer &deserializer)
    {
        deserializer.ReadString(this->name);
        deserializer.Read(this->position);
        deserializer.Read(this->rotation);
        deserializer.Read(this->scale);
        deserializer.Read(this->offsetMatrix);
        deserializer.Read(this->skinningMatrix);
    }
}
