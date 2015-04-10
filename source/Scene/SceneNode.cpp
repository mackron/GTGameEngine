// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNode.hpp>
#include <GTGameEngine/Scene/SceneNodeComponent.hpp>
#include <cassert>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

namespace GT
{
    SceneNode::SceneNode()
        : SceneNode(0)
    {
    }

    SceneNode::SceneNode(uint64_t id)
        : m_id(id),
          m_name(),
          m_components(),
          m_componentCount(0),
          m_flags(0),
          m_padding0(0),
          m_position(0.0f, 0.0f, 0.0f, 0.0f),
          m_rotation(0.0f, 0.0f, 0.0f, 1.0f),
          m_scale(1.0f, 1.0f, 1.0f, 0.0f)
    {
        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<uint64_t*>(m_name)[i] = 0;
        }

        for (int i = 0; i < GT_MAX_SCENE_NODE_COMPONENTS; ++i)
        {
            m_components[0] = nullptr;
        }
    }


    uint64_t SceneNode::GetID() const
    {
        return m_id;
    }


    const char* SceneNode::GetName() const
    {
        return m_name;
    }

    void SceneNode::SetName(const char* newName)
    {
        // Pad with zero.
        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<uint64_t*>(m_name)[i] = 0;
        }

        // Copy without going past 31 characters.
        for (int i = 0; i < 31 && newName[i] != '\0'; ++i)
        {
            m_name[i] = newName[i];
        }
    }


    unsigned int SceneNode::GetComponentCount() const
    {
        return m_componentCount;
    }

    SceneNodeComponent* SceneNode::GetComponentByIndex(unsigned int index)
    {
        if (index < m_componentCount)
        {
            return m_components[index];
        }
        else
        {
            return nullptr;
        }
    }

    SceneNodeComponent* SceneNode::GetComponentByTypeID(SceneNodeComponentTypeID typeID)
    {
        for (uint16_t i = 0; i < m_componentCount; ++i)
        {
            auto pComponent = m_components[i];
            assert(pComponent != nullptr);
            {
                if (pComponent->GetTypeID() == typeID)
                {
                    return pComponent;
                }
            }
        }

        return nullptr;
    }


    bool SceneNode::AttachComponent(SceneNodeComponent &component)
    {
        if (m_componentCount < GT_MAX_SCENE_NODE_COMPONENTS)
        {
            // Check a component of the same type is not already attached.
            if (this->GetComponentByTypeID(component.GetTypeID()) == nullptr)
            {
                m_components[m_componentCount] = &component;
                m_componentCount += 1;

                return true;
            }
            else
            {
                // A component of the same type is already attached.
                return false;
            }
        }
        else
        {
            // No available slots. Max 8.
            return false;
        }
    }

    void SceneNode::DetachComponent(SceneNodeComponent &component)
    {
        for (uint16_t i = 0; i < m_componentCount; ++i)
        {
            auto pComponent = m_components[i];
            if (pComponent == &component)
            {
                this->DetachComponentByIndex(i);
                break;
            }
        }
    }

    void SceneNode::DetachComponentByTypeID(SceneNodeComponentTypeID typeID)
    {
        for (uint16_t i = 0; i < m_componentCount; ++i)
        {
            auto pComponent = m_components[i];
            assert(pComponent != nullptr);
            {
                if (pComponent->GetTypeID() == typeID)
                {
                    this->DetachComponentByIndex(i);
                    break;
                }
            }
        }
    }

    void SceneNode::DetachComponentByIndex(unsigned int index)
    {
        if (index < m_componentCount)
        {
            m_componentCount -= 1;


            // Move everything down.
            for (uint16_t iComponent = static_cast<uint16_t>(index); iComponent < m_componentCount - 1; ++iComponent)
            {
                m_components[iComponent] = m_components[iComponent + 1];
            }

            // Make sure the last item is null.
            m_components[m_componentCount] = nullptr;
        }
    }


    vec4 SceneNode::GetPosition() const
    {
        return m_position;
    }

    quat SceneNode::GetRotation() const
    {
        return m_rotation;
    }

    vec4 SceneNode::GetScale() const
    {
        return m_scale;
    }


    void SceneNode::SetPosition(vec4 position)
    {
        m_position = position;
        m_flags |= _PositionChanged;
    }

    void SceneNode::SetRotation(quat rotation)
    {
        m_rotation = rotation;
        m_flags |= _RotationChanged;
    }

    void SceneNode::SetScale(vec4 scale)
    {
        m_scale = scale;
        m_flags |= _ScaleChanged;
    }





    ///////////////////////////////////
    // Internal Use Only

    uint16_t SceneNode::_GetFlags() const
    {
        return m_flags;
    }

    void SceneNode::_SetFlags(uint16_t flags)
    {
        m_flags = flags;
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
