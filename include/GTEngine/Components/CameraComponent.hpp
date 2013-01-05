
#ifndef __GTEngine_CameraComponent_hpp_
#define __GTEngine_CameraComponent_hpp_

#include "../Component.hpp"
#include "../Math.hpp"

namespace GTEngine
{
    /**
    *   \brief  Class representing a camera component.
    *
    *   A node that is used as a camera must have a camera component attached.
    */
    GTENGINE_DECL_COMPONENT_START(CameraComponent)
    public:

        /// Constructor.
        CameraComponent(SceneNode &node);

        /**
        *   \brief  Sets the camera's projection to a perspective projection.
        */
        void Set3DProjection(float fov, float aspect, float zNear, float zFar);

        /**
        *   \brief  Sets the camera's projection to an orthographic projection.
        */
        void Set2DProjection(float left, float right, float bottom, float top, float zNear, float zFar);


        /**
        *   \brief  Calculates a view matrix based on the position and orientation of the node.
        */
        glm::mat4 GetViewMatrix() const;

        /**
        *   \brief  Retrieves the projection matrix.
        */
        const glm::mat4 & GetProjectionMatrix() const;



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTCore::Serializer &serializer);

        /// Component::Deserialize()
        void Deserialize(GTCore::Deserializer &deserializer);


    public:

        /// The projection matrix to use with the camera.
        glm::mat4 projection;

        struct _perspective
        {
            float fov;
            float aspect;
        };

        struct _ortho
        {
            float left;
            float right;
            float bottom;
            float top;
        };

        /// The camera's projection-specific attributes.
        union
        {
            _perspective perspective;
            _ortho       ortho;
        };

        float zNear;    /// The near clipping plane.
        float zFar;     /// The far clipping plane.

    GTENGINE_DECL_COMPONENT_END()
}


#endif
