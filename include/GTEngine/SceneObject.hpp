// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneObject_hpp_
#define __GTEngine_SceneObject_hpp_

#include "SceneObjectTypes.hpp"
#include "Math.hpp"
#include "Serialization.hpp"

namespace GTEngine
{
    /// The base class for anything that will be placed in the scene.
    ///
    /// The aim of this class is to be as light weight as possible as to allow for many-instantiation objects like particles. Any type of object
    /// that needs to be represented in the scene will be derived from this class, most notably the SceneNode class.
    ///
    /// This class has no notion of a hierarchy. For this, use a SceneNode class. Note, however, that SceneNode is a heavy class.
    ///
    /// Since most, if not all, scene objects will have the notion of a position/rotation/scale, we include those attributes here. The setter
    /// methods for the transformations are virtual so that derived classes can handle movement properly.
    class SceneObject
    {
    public:

        /// Constructor.
        ///
        /// @param type [in] The type of this scene node, for identification.
        SceneObject(SceneObjectType type);

        /// Destructor.
        virtual ~SceneObject();


        /// Retrieves the type of the scene object.
        ///
        /// @return The type of the scene object.
        SceneObjectType GetType() const { return this->type; }


        /// Retrieves the position of the object.
        const glm::vec3 & GetPosition() const { return this->position; }

        /// Sets the position of the object.
        ///
        /// @param newPosition [in] The new position of the object.
        virtual void SetPosition(const glm::vec3 &newPosition) { this->position = newPosition; }
        void SetPosition(float x, float y, float z)            { this->SetPosition(glm::vec3(x, y, z)); }


        /// Retrieves the orientation of the object.
        const glm::quat & GetOrientation() const { return this->orientation; }

        /// Sets the orientation of the object.
        ///
        /// @param newOrientation [in] The new orientation of the object, as a quaternion.
        virtual void SetOrientation(const glm::quat &newOrientation) { this->orientation = newOrientation; }


        /// Retrieves the scale of the object.
        const glm::vec3 & GetScale() const { return this->scale; }

        /// Sets the scale of the object.
        ///
        /// @param newScale [in] The new scale of the object.
        virtual void SetScale(const glm::vec3 &newScale) { this->scale = newScale; }
        void SetScale(float x, float y, float z)         { this->SetScale(glm::vec3(x, y, z)); }


        /// Translates the object.
        ///
        /// @param offset [in] The amount to translate on each axis.
        ///
        /// @remarks
        ///     This does a local transformation where the orientation of the object is taken into acount.
        void Translate(const glm::vec3 &offset);

        /// Rotates the node.
        ///
        /// @param angleDegrees [in] The amount of degrees to rotate the object.
        /// @param axis         [in] The axis to rotate around.
        void Rotate(float angleDegrees, const glm::vec3 &axis);

        /// Scales the node.
        ///
        /// @param scale [in] The amount to scale the object.
        void Scale(const glm::vec3 &scale);


        /// Moves the object in the direction of its forward vector.
        ///
        /// @param  distance [in] The distance to move.
        void MoveForward(float distance) { this->Translate(glm::vec3(0.0f, 0.0f, -distance)); }

        /// Moves the object in the direction of its right vector.
        ///
        /// @param  distance [in] The distance to move.
        void MoveRight(float distance) { this->Translate(glm::vec3(distance, 0.0f, 0.0f)); }

        /// Moves the object in the direction of its up vector.
        ///
        /// @param  distance [in] The distance to move.
        void MoveUp(float distance) { this->Translate(glm::vec3(0.0f, distance, 0.0f)); }


        /// Rotates the object around it's local x axis.
        ///
        /// @param  angleDegress [in] The angle in degrees to rotate.
        void RotateX(float angleDegrees) { this->Rotate(angleDegrees, glm::vec3(1.0f, 0.0f, 0.0f)); }

        /// Rotates the object around it's local y axis.
        ///
        /// @param  angleDegress [in] The angle in degrees to rotate.
        void RotateY(float angleDegrees) { this->Rotate(angleDegrees, glm::vec3(0.0f, 1.0f, 0.0f)); }

        /// Rotates the object around it's local z axis.
        ///
        /// @param  angleDegress [in] The angle in degrees to rotate.
        void RotateZ(float angleDegrees) { this->Rotate(angleDegrees, glm::vec3(0.0f, 0.0f, 1.0f)); }



        /// Performs a linear interpolation of the object's position.
        ///
        /// @param dest [in] The destination position.
        /// @param a    [in] The factor controlling how far to move towards <dest>. A value of 1.0 will move the object to <dest>, whereas a value of 0.0 will not move it at all.
        void InterpolatePosition(const glm::vec3 &dest, float a);

        /// Performs a spherical-linear interpolation of the object's rotation.
        ///
        /// @param dest [in] The destination orientation.
        /// @param a    [in] The factor controlling how far to rotate towards <dest>.
        void InterpolateOrientation(const glm::quat &dest, float a);

        /// Performs a linear interpolation of the object's scale.
        ///
        /// @param dest [in] The destination scale.
        /// @param a    [in] The factor controlling how far to scale towards <dest>.
        void InterpolateScale(const glm::vec3 &scale, float a);


        /// Retrieves a normalised vector pointing in the forward direction of the object.
        glm::vec3 GetForwardVector() const;

        /// Retrieves a normalised vector pointing in the right direction of the object.
        glm::vec3 GetRightVector() const;

        /// Retrieves a normalised vector pointing in the up direction of the object.
        glm::vec3 GetUpVector() const;


        /// Rotates the object about the world axis.
        ///
        /// @param angle [in] The angle of rotation.
        /// @param axis  [in] The axis of rotation.
        void RotateAroundWorldAxis(float angle, const glm::vec3 &axis);

        /// Rotates the object about the world axis around the given pivot point.
        ///
        /// @param angle [in] The angle of rotation.
        /// @param axis  [in] The axis of rotation.
        /// @param pivot [in] The pivot point to rotate around.
        void RotateAtPivotAroundWorldAxis(float angle, const glm::vec3 &axis, const glm::vec3 &pivot);


        /// Calculates a transformation matrix for this object.
        ///
        /// @param dest [in] A reference to the matrix that will receive the transformation.
        ///
        /// @remarks
        ///     This is not a simple accessor, but rather a calculate of the transform from the position, orientation and scale.
        void GetTransform(glm::mat4 &dest);



        //////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the scene object.
        ///
        /// @param serializer [in] The serializer to write the data to.
        virtual void Serialize(GTCore::Serializer &serializer) const;

        /// Deserializes the scene object.
        ///
        /// @param deserializer [in] The deserializer to read the data from.
        virtual void Deserialize(GTCore::Deserializer &deserializer);



    protected:

        /// The type of this scene object. We use this for identification for things like upcasting.
        SceneObjectType type;

        /// The position of the object.
        glm::vec3 position;

        /// The orientation of the object.
        glm::quat orientation;

        /// The scale of the object.
        glm::vec3 scale;
    };
}

#endif
