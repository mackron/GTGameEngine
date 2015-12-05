// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGE/Graphics/GraphicsWorldObject.hpp>

namespace GT
{
    /////////////////////////////////////////
    // GraphicsWorldObject

    GraphicsWorldObject::GraphicsWorldObject()
    {
    }

    GraphicsWorldObject::~GraphicsWorldObject()
    {
    }


    GraphicsWorldObjectType GraphicsWorldObject::GetType() const
    {
        return GraphicsWorldObjectType::Unknown;
    }


    void GraphicsWorldObject::SetPosition(const glm::vec4 &position)
    {
        m_position = position;
    }

    glm::vec4 GraphicsWorldObject::GetPosition() const
    {
        return m_position;
    }


    void GraphicsWorldObject::SetRotation(const glm::quat &rotation)
    {
        m_rotation = rotation;
    }

    glm::quat GraphicsWorldObject::GetRotation() const
    {
        return m_rotation;
    }


    void GraphicsWorldObject::SetScale(const glm::vec4 &scale)
    {
        m_scale = scale;
    }

    glm::vec4 GraphicsWorldObject::GetScale() const
    {
        return m_scale;
    }


    glm::mat4 GraphicsWorldObject::CalculateTransformMatrix() const
    {
        glm::mat4 result;

        result = glm::mat4_cast(m_rotation);

        result[3][0] = m_position.x;
        result[3][1] = m_position.y;
        result[3][2] = m_position.z;

        result[0] *= m_scale.x;
        result[1] *= m_scale.y;
        result[2] *= m_scale.z;

        return result;
    }




    /////////////////////////////////////////
    // GraphicsWorldMesh

    GraphicsWorldMesh::GraphicsWorldMesh(HGraphicsWorldMeshDescriptor hDescriptor)
        : GraphicsWorldObject(),
          m_hDescriptor(hDescriptor)
    {
    }

    GraphicsWorldMesh::~GraphicsWorldMesh()
    {
    }


    GraphicsWorldObjectType GraphicsWorldMesh::GetType() const
    {
        return GraphicsWorldObjectType::Mesh;
    }


    HGraphicsWorldMeshDescriptor GraphicsWorldMesh::GetDescriptor() const
    {
        return m_hDescriptor;
    }
}
