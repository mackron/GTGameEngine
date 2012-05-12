
#ifndef __GTEngine_Bone_hpp_
#define __GTEngine_Bone_hpp_

#include <GTCore/Vector.hpp>
#include <GTCore/String.hpp>
#include "Math.hpp"

// Temp for testing.
#include "Logging.hpp"

namespace GTEngine
{
    /// Class representing a bone in a skeleton.
    class Bone
    {
    public:

        /// Structure reprsenting a vertex weight.
        struct VertexWeight
        {
            VertexWeight(unsigned int vertexIDIn, float weightIn)
                : vertexID(vertexIDIn), weight(weightIn)
            {
            }

            unsigned int vertexID;
            float        weight;
        };



    public:

        /// Constructor.
        Bone();

        /// Copy constructor.
        ///
        /// @remarks
        ///     This does NOT attach the new bone to a parent, nor does it attach any children. Thus, the new bone will need to be attached to a parent manually.
        Bone(const Bone &other);

        /// Destructor.
        ~Bone();


        /// Attaches a child.
        void AttachChild(Bone &child);

        /// Detaches a child.
        void DetachChild(Bone &child);


        /// Retrieves a pointer to the parent bone, or nullptr if the bone does not have a parent.
        Bone* GetParent() { return this->parent; }


        /// Sets the name of the bone.
        void SetName(const char* newName) { this->name = newName; }

        /// Retrieves the name of the bone.
        const char* GetName() const { return this->name.c_str(); }



        /// Sets the transform of the bone.
        void SetTransform(const glm::mat4 &transform);

        /// Sets the offset matrix of the bone.
        void SetOffsetMatrix(const glm::mat4 &transform);

        /// Sets the vertex weights.
        void AddWeight(unsigned int vertexID, float weight);


        /// Determines whether or not the bone is empty (has any weights).
        bool IsEmpty() const { return this->weights.count == 0; }


        /// Retrieves the top level bone. If this bone is already the top level bone, <this> will be returned.
        Bone* GetTopLevelBone();
        

        // For testing.
        void LogInfo()
        {
            GTEngine::Log("Bone: %s/%s", name.c_str(), (parent != nullptr) ? parent->GetName() : "none");

            for (size_t i = 0; i < this->children.count; ++i)
            {
                this->children[i]->LogInfo();
            }
        }


    private:

        /// The parent bone. If this is null then the bone is the root bone in the skeleton.
        Bone* parent;

        /// The children.
        GTCore::Vector<Bone*> children;


        /// The name of the bone.
        GTCore::String name;


        /// The bones transformation.
        glm::mat4 transform;

        /// The offset matrix for this bone.
        glm::mat4 offsetMatrix;


        /// The list of vertex weights.
        GTCore::Vector<VertexWeight> weights;
    };
}


#endif
