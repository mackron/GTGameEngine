
#include <GTEngine/Bone.hpp>

namespace GTEngine
{
    Bone::Bone()
        : parent(nullptr), children(),
          name(), transform(), offsetMatrix(), weights()
    {
    }

    Bone::Bone(const Bone &other)
        : parent(nullptr), children(),
          name(other.name), transform(other.transform), offsetMatrix(other.offsetMatrix), weights(other.weights)
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

    void Bone::SetTransform(const glm::mat4 &transform)
    {
        this->transform = transform;
    }

    void Bone::SetOffsetMatrix(const glm::mat4 &offsetMatrix)
    {
        this->transform = offsetMatrix;
    }

    void Bone::AddWeight(unsigned int vertexID, float weight)
    {
        this->weights.PushBack(VertexWeight(vertexID, weight));
    }


    Bone* Bone::GetTopLevelBone()
    {
        if (this->parent != nullptr)
        {
            return this->parent->GetTopLevelBone();
        }

        return this;
    }
}
