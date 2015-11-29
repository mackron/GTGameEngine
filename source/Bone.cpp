// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Bone.hpp>

namespace GT
{
    Bone::Bone()
        : parent(nullptr), children(),
          name(), m_position(), m_rotation(), m_scale(1.0f, 1.0f, 1.0f),
          offsetMatrix(), skinningMatrix()
    {
    }

    Bone::Bone(const Bone &other)
        : parent(nullptr), children(),
          name(other.name), m_position(other.m_position), m_rotation(other.m_rotation), m_scale(other.m_scale),
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
        return m_position;
    }

    const glm::quat & Bone::GetRotation() const
    {
        return m_rotation;
    }

    const glm::vec3 & Bone::GetScale() const
    {
        return m_scale;
    }


    void Bone::SetPosition(const glm::vec3 &newPosition)
    {
        m_position = newPosition;
    }

    void Bone::SetRotation(const glm::quat &newRotation)
    {
        m_rotation = newRotation;
    }

    void Bone::SetScale(const glm::vec3 &newScale)
    {
        m_scale = newScale;
    }


    glm::vec3 Bone::GetAbsolutePosition() const
    {
        if (this->parent != nullptr)
        {
            glm::quat rotation = this->parent->GetAbsoluteRotation();
            glm::vec3 scale    = this->parent->GetAbsoluteScale();

            return this->parent->GetAbsolutePosition() + (rotation * (scale * m_position));
        }

        return m_position;
    }

    glm::quat Bone::GetAbsoluteRotation() const
    {
        if (this->parent != nullptr)
        {
            return this->parent->GetAbsoluteRotation() * m_rotation;
        }

        return m_rotation;
    }

    glm::vec3 Bone::GetAbsoluteScale() const
    {
        if (this->parent != nullptr)
        {
            return this->parent->GetAbsoluteScale() * m_scale;
        }

        return m_scale;
    }

    
    void Bone::GetAbsoluteTransformComponents(glm::vec3 &positionOut, glm::quat &rotationOut, glm::vec3 &scaleOut) const
    {
        if (this->parent != nullptr)
        {
            this->parent->GetAbsoluteTransformComponents(positionOut, rotationOut, scaleOut);

            positionOut = positionOut + (rotationOut * (scaleOut * m_position));
            rotationOut = rotationOut * m_rotation;
            scaleOut    = scaleOut    * m_scale;
        }
        else
        {
            positionOut = m_position;
            rotationOut = m_rotation;
            scaleOut    = m_scale;
        }
    }


    glm::mat4 Bone::GetTransform() const
    {
        glm::mat4 result;
        Math::CalculateTransformMatrix(m_position, m_rotation, m_scale, result);

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


    void Bone::SetAbsoluteTransform(const glm::vec3 &absolutePosition, const glm::quat &absoluteRotation, const glm::vec3 &absoluteScale)
    {
        if (this->parent != nullptr)
        {
            glm::vec3 Pp;
            glm::quat Po;
            glm::vec3 Ps;
            this->parent->GetAbsoluteTransformComponents(Pp, Po, Ps);

            m_position = ((absolutePosition - Pp) * Po) / Ps;
            m_rotation = glm::inverse(Po) * absoluteRotation;
            m_scale    = absoluteScale / Ps;
        }
        else
        {
            m_position = absolutePosition;
            m_rotation = absoluteRotation;
            m_scale    = absoluteScale;
        }
    }

    void Bone::SetAbsoluteTransform(const glm::vec3 &absolutePosition, const glm::quat &absoluteRotation)
    {
        if (this->parent != nullptr)
        {
            glm::vec3 Pp;
            glm::quat Po;
            glm::vec3 Ps;
            this->parent->GetAbsoluteTransformComponents(Pp, Po, Ps);

            m_position = ((absolutePosition - Pp) * Po) / Ps;
            m_rotation = glm::inverse(Po) * absoluteRotation;
        }
        else
        {
            m_position = absolutePosition;
            m_rotation = absoluteRotation;
        }
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


    void Bone::SetOffsetMatrix(const glm::mat4 &offsetMatrixIn)
    {
        this->offsetMatrix = offsetMatrixIn;
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

    void Bone::Serialize(Serializer &serializer) const
    {
        serializer.WriteString(this->name);
        serializer.Write(m_position);
        serializer.Write(m_rotation);
        serializer.Write(m_scale);
        serializer.Write(this->offsetMatrix);
        serializer.Write(this->skinningMatrix);
    }

    void Bone::Deserialize(Deserializer &deserializer)
    {
        deserializer.ReadString(this->name);
        deserializer.Read(m_position);
        deserializer.Read(m_rotation);
        deserializer.Read(m_scale);
        deserializer.Read(this->offsetMatrix);
        deserializer.Read(this->skinningMatrix);
    }
}
