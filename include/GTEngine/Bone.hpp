// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Bone_hpp_
#define __GTEngine_Bone_hpp_

#include "Math.hpp"
#include <GTEngine/Core/Vector.hpp>
#include <GTEngine/Core/String.hpp>
#include <GTEngine/Core/Serializer.hpp>
#include <GTEngine/Core/Deserializer.hpp>

namespace GT
{
    /// Structure reprsenting a vertex weight.
    struct VertexWeightPair
    {
        /// Default constructor. Note the lack of initialisation!
        VertexWeightPair()
            : vertexID(), weight()
        {
        }

        VertexWeightPair(unsigned int vertexIDIn, float weightIn)
            : vertexID(vertexIDIn), weight(weightIn)
        {
        }

        unsigned int vertexID;
        float        weight;
    };
}

namespace GT
{
    /// Structure containing weights for a given bone.
    struct BoneWeights
    {
        BoneWeights(const char* name)
            : name(name), weights()
        {
        }

        /// The name of the bone these weights are referring to.
        String name;

        /// The lsit of vertex/weight pairs.
        Vector<VertexWeightPair> weights;
    };
}



namespace GT
{
    /// Class representing a bone in a skeleton.
    class Bone
    {
    public:

        /// Constructor.
        Bone();

        /// Copy constructor.
        ///
        /// @remarks
        ///     This does NOT attach the new bone to a parent, nor does it attach any children. Thus, the new bone will need to be attached to a parent manually.
        Bone(const Bone &other);

        /// Destructor.
        virtual ~Bone();


        /// Attaches a child.
        void AttachChild(Bone &child);

        /// Detaches a child.
        void DetachChild(Bone &child);


        /// Retrieves a pointer to the parent bone, or nullptr if the bone does not have a parent.
        Bone* GetParent() { return this->parent; }

        /// Retrieves a direct reference to the internal vector containing the pointers to the child bones.
              Vector<Bone*> & GetChildren()       { return this->children; }
        const Vector<Bone*> & GetChildren() const { return this->children; }


        /// Sets the name of the bone.
        void SetName(const char* newName) { this->name = newName; }

        /// Retrieves the name of the bone.
        const char* GetName() const { return this->name.c_str(); }



        /// Retrieves the relative position of the bone.
        const glm::vec3 & GetPosition() const;

        /// Retrieves the relative rotation of the bone.
        const glm::quat & GetRotation() const;

        /// Retrieves the relative scale of the bone.
        const glm::vec3 & GetScale() const;


        /// Sets the relative position of the bone.
        void SetPosition(const glm::vec3 &position);

        /// Sets the relative rotation of the bone.
        void SetRotation(const glm::quat &rotation);

        /// Sets the relative scale of the bone.
        void SetScale(const glm::vec3 &scale);


        /// Retrieves the absolute position of the bone.
        glm::vec3 GetAbsolutePosition() const;

        /// Retrieves the absolute orientation of the bone.
        glm::quat GetAbsoluteRotation() const;

        /// Retrieves the absolute scale of the bone.
        glm::vec3 GetAbsoluteScale() const;


        /// Optimized method for retrieving the absolute transformation components (position, rotation, scale).
        void GetAbsoluteTransformComponents(glm::vec3 &position, glm::quat &rotation, glm::vec3 &scale) const;


        /// Retrieves the local transformation matrix.
        glm::mat4 GetTransform() const;

        /// Retrieves the absolute transformation of the bone.
        glm::mat4 GetAbsoluteTransform() const;
        glm::mat4 GetAbsoluteTransform(glm::vec3 &absolutePositionOut, glm::quat &absoluteRotationOut, glm::vec3 &absoluteScaleOut) const;


        /// Sets the absolute transformation of the bone based on the given components.
        ///
        /// @param absolutePosition [in] The new absolute position of the bone.
        /// @param absoluteRotation [in] The new absolute rotation of the bone.
        /// @param absoluteScale    [in] The new absolute scale of the bone.
        ///
        /// @remarks
        ///     The relative position of child bones will not be modified. Because of this the absolute position of the children will change after calling this method.
        void SetAbsoluteTransform(const glm::vec3 &absolutePosition, const glm::quat &absoluteRotation, const glm::vec3 &absoluteScale);
        void SetAbsoluteTransform(const glm::vec3 &absolutePosition, const glm::quat &absoluteRotation);


        /// Updates the skinning transformation matrix.
        ///
        /// @param
        ///     The matrix generated from this method is used as the return value for GetSkinningTransform().
        void UpdateSkinningTransform();
        void UpdateSkinningTransform(glm::vec3 &absolutePositionOut, glm::quat &absoluteRotationOut, glm::vec3 &absoluteScaleOut);

        /// Retrieves the transformation to use when applying vertex blending.
        ///
        /// @param
        ///     You must call UpdateSkinningTransform() before calling this method. This will return the matrix that was generated by the last call to UpdateSkinningTransform(). We use
        ///     this design so that the CPU vertex shader doing the transformations does not need to perform an expensive recursive chain for each vertex and can instead use a direct accessor.
        const glm::mat4 & GetSkinningTransform() const;



        /// Sets the offset matrix of the bone.
        void SetOffsetMatrix(const glm::mat4 &transform);

        /// Retrieves the offset matrix of the bone.
        const glm::mat4 & GetOffsetMatrix() const;


        /// Retrieves the top level bone. If this bone is already the top level bone, <this> will be returned.
        Bone* GetTopLevelBone();



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the bone, not including hierarchy information (parent, children).
        ///
        /// @param serializer [in] A reference to the serializer to write to.
        void Serialize(Serializer &serializer) const;

        /// Deserializes the bone, not including hierarchy information (parent, children).
        ///
        /// @param deserializer [in] A reference to the deserializer to read from.
        void Deserialize(Deserializer &deserializer);




    protected:

        /// The parent bone. If this is null then the bone is the root bone in the skeleton.
        Bone* parent;

        /// The children.
        Vector<Bone*> children;


        /// The name of the bone.
        String name;


        /// The relative position of the bone.
        glm::vec3 m_position;

        /// The relative rotation of the bone.
        glm::quat m_rotation;

        /// The relative scale of the bone.
        glm::vec3 m_scale;


        /// The offset matrix for this bone.
        glm::mat4 offsetMatrix;

        /// The skinning matrix to apply to vertices. This is computed in UpdateSkinningMatrix().
        glm::mat4 skinningMatrix;
        
        
    private:    // No copying.
        Bone & operator=(const Bone &);
    };
}

#endif
