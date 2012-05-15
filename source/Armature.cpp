
#include <GTEngine/Armature.hpp>

namespace GTEngine
{
    Armature::Armature()
        : rootBones()
    {
    }

    Armature::~Armature()
    {
    }

    void Armature::AddRootBone(Bone &bone)
    {
        // Don't add it if it already exists...
        if (!this->rootBones.Exists(&bone))
        {
            this->rootBones.PushBack(&bone);
        }
    }

    void Armature::RemoveRootBone(Bone &bone)
    {
        this->rootBones.RemoveFirst(&bone);
    }
}
