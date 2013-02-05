// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneObject.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    SceneObject::SceneObject(SceneObjectType typeIn)
        : type(typeIn),
          position(), orientation(), scale(1.0f, 1.0f, 1.0f)
    {
    }

    SceneObject::~SceneObject()
    {
    }


    void SceneObject::Translate(const glm::vec3 &offset)
    {
        this->SetPosition(this->position + (this->orientation * offset));
    }

    void SceneObject::Rotate(float angle, const glm::vec3 &axis)
    {
        this->SetOrientation(this->orientation * glm::angleAxis(angle, axis));
    }

    void SceneObject::Scale(const glm::vec3 &scale)
    {
        this->SetScale(this->scale * scale);
    }


    void SceneObject::InterpolatePosition(const glm::vec3 &dest, float a)
    {
        this->SetPosition(glm::mix(this->position, dest, a));
    }

    void SceneObject::InterpolateOrientation(const glm::quat &dest, float a)
    {
        this->SetOrientation(glm::slerp(this->orientation, dest, a));
    }

    void SceneObject::InterpolateScale(const glm::vec3 &dest, float a)
    {
        this->SetScale(glm::mix(this->scale, dest, a));
    }


    glm::vec3 SceneObject::GetForwardVector() const
    {
        return this->orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 SceneObject::GetRightVector() const
    {
        return this->orientation * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::vec3 SceneObject::GetUpVector() const
    {
        return this->orientation * glm::vec3(0.0f, 1.0f, 0.0f);
    }



    void SceneObject::RotateAroundWorldAxis(float angle, const glm::vec3 &axis)
    {
        this->SetOrientation(glm::angleAxis(angle, axis) * this->GetOrientation());
    }

    void SceneObject::RotateAtPivotAroundWorldAxis(float angle, const glm::vec3 &axis, const glm::vec3 &pivot)
    {
        glm::quat rotation       = glm::angleAxis(angle, axis);
        glm::quat newOrientation = rotation * this->GetOrientation();
        glm::vec3 newPosition    = rotation * (this->GetPosition() - pivot) + pivot;

        this->SetOrientation(newOrientation);
        this->SetPosition(newPosition);
    }


    void SceneObject::GetTransform(glm::mat4 &transform)
    {
        Math::CalculateTransformMatrix(this->position, this->orientation, this->scale, transform);
    }



    //////////////////////////////////////////////////
    // Serialization/Deserialization.

    void SceneObject::Serialize(GTCore::Serializer &serializer) const
    {
        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_SceneObject;
        header.version     = 1;
        header.sizeInBytes =
            sizeof(glm::vec3) +     // <-- Position
            sizeof(glm::quat) +     // <-- Orientation
            sizeof(glm::vec3);      // <-- Scale

        serializer.Write(header);
        {
            serializer.Write(this->position);
            serializer.Write(this->orientation);
            serializer.Write(this->scale);
        }
    }

    void SceneObject::Deserialize(GTCore::Deserializer &deserializer)
    {
        glm::vec3 newPosition;
        glm::quat newOrientation;
        glm::vec3 newScale;


        Serialization::ChunkHeader header;
        deserializer.Read(header);
        {
            switch (header.version)
            {
            case 1:
                {
                    deserializer.Read(newPosition);
                    deserializer.Read(newOrientation);
                    deserializer.Read(newScale);

                    break;
                }

            default:
                {
                    GTEngine::Log("Error deserializing SceneObject. Unsupported version (%d).", header.version);
                    break;
                }
            }
            
        }


        this->SetPosition(newPosition);
        this->SetOrientation(newOrientation);
        this->SetScale(newScale);
    }
}
