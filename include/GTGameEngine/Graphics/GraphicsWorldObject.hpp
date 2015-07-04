// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsWorldObject_hpp_
#define __GT_GraphicsWorldObject_hpp_

#include "../Math.hpp"

namespace GT
{
    typedef size_t HGraphicsWorldMeshDescriptor;        //< A handle representing a mesh descriptor.



    /// Enumerator for the different types of objects contained in a graphics world.
    enum class GraphicsWorldObjectType
    {
        Unknown = 0,
        Mesh,
        AmbientLight,
        PointLight,
        DirectionalLight,
    };



    /// Base class for graphics world objects.
    class GraphicsWorldObject
    {
    public:

        /// Constructor.
        GraphicsWorldObject();

        /// Destructor.
        virtual ~GraphicsWorldObject();


        /// Retrieves the object type.
        virtual GraphicsWorldObjectType GetType() const;


        /// Sets the position of the object.
        ///
        /// @param position [in] The new position.
        ///
        /// @remarks
        ///     This is just a simple setter.
        void SetPosition(const vec4 &position);

        /// Retrieves the position of the object.
        ///
        /// @return The position of the object.
        vec4 GetPosition() const;


        /// Sets the rotation of the object.
        ///
        /// @param rotation [in] The new rotation.
        ///
        /// @remarks
        ///     This is just a simple setter.
        void SetRotation(const quat &rotation);

        /// Retrieves the rotation of the object.
        ///
        /// @return The rotation of the object.
        quat GetRotation() const;


        /// Sets the scale of the object.
        ///
        /// @param scale [in] The new scale.
        ///
        /// @remarks
        ///     This is just a simple setter.
        void SetScale(const vec4 &scale);

        /// Retrieves the scale of the object.
        ///
        /// @return The scale of the object.
        vec4 GetScale() const;


        /// A helper function for calculating a matrix representation of the object's transform.
        ///
        /// @return The matrix representation of the object's transform.
        mat4 CalculateTransformMatrix() const;


    private:

        /// The position of the object in the world.
        vec4 m_position;

        /// The rotation of the object in the world.
        quat m_rotation;

        /// The scale of the object.
        vec4 m_scale;
    };



    /// Mesh graphics world object.
    class GraphicsWorldMesh : public GraphicsWorldObject
    {
    public:

        /// Constructor.
        GraphicsWorldMesh(HGraphicsWorldMeshDescriptor hDescriptor);

        /// Destructor.
        ~GraphicsWorldMesh();


        /// GraphicsWorldObject::GetType()
        GraphicsWorldObjectType GetType() const;


        /// Retrieves a handle to the descriptor.
        HGraphicsWorldMeshDescriptor GetDescriptor() const;



    private:

        /// A handle to the mesh descriptor.
        HGraphicsWorldMeshDescriptor m_hDescriptor;
    };
}

#endif
