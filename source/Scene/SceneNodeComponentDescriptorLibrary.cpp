// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeComponentDescriptorLibrary.hpp>

namespace GT
{
    SceneNodeComponentDescriptorLibrary::SceneNodeComponentDescriptorLibrary()
        : m_descriptors()
    {
    }

    SceneNodeComponentDescriptorLibrary::~SceneNodeComponentDescriptorLibrary()
    {
    }


    void SceneNodeComponentDescriptorLibrary::RegisterDescriptor(const SceneNodeComponentDescriptor &descriptor)
    {
        m_descriptors.Add(descriptor.GetTypeID(), &descriptor);
    }

    const SceneNodeComponentDescriptor* SceneNodeComponentDescriptorLibrary::GetDescriptorByType(SceneNodeComponentTypeID type) const
    {
        auto iDescriptor = m_descriptors.Find(type);
        if (iDescriptor != nullptr)
        {
            return iDescriptor->value;
        }

        return nullptr;
    }


    SceneNodeComponent* SceneNodeComponentDescriptorLibrary::CreateComponent(SceneNodeComponentTypeID type)
    {
        auto pDescriptor = this->GetDescriptorByType(type);
        if (pDescriptor != nullptr)
        {
            return pDescriptor->CreateComponent();
        }

        return nullptr;
    }

    void SceneNodeComponentDescriptorLibrary::DeleteComponent(SceneNodeComponent* pComponent)
    {
        if (pComponent != nullptr)
        {
            auto pDescriptor = this->GetDescriptorByType(pComponent->GetTypeID());
            if (pDescriptor != nullptr)
            {
                return pDescriptor->DeleteComponent(pComponent);
            }
            else
            {
                // There's a logic error if we get here. It means that the library isn't aware of the component's type.
                assert(false);
            }
        }
    }
}
