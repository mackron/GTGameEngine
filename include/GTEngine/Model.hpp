// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Model_hpp_
#define __GTEngine_Model_hpp_

#include "ModelDefinition.hpp"
#include "Mesh.hpp"
#include "Animation/Animation.hpp"
#include "Serialization.hpp"
#include <GTCore/Vector.hpp>


namespace GTEngine
{
    /// Class representing a model.
    ///
    /// A model contains an arbitrary number of meshes, materials and animation data.
    class Model
    {
    public:

        /// Default constructor.
        ///
        /// @remarks
        ///     This constructor will allow something to create a model without needing to supply a definition. This is useful for doing procedural models or whatnot.
        Model();

        /// Constructor.
        ///
        /// @param definition [in] A reference to the model definition to create the model from.
        Model(const ModelDefinition &definition);

        /// Destructor.
        ~Model();


        /// Retrieves the definition of this model.
        ///
        /// @return
        ///     A constant reference to the definition of this model.
        const ModelDefinition & GetDefinition() const { return this->definition; }


        /// Attaches a mesh to the model.
        ///
        /// @return A pointer to the new mesh.
        ///
        /// @remarks
        ///     It's important that <bones> refers to a list of BoneWithWeights pointers since the vertex/weight information will be needed to set the skinning vertex attributes.
        Mesh* AttachMesh(VertexArray* mesh, const char* materialFileName, DrawMode drawMode = DrawMode_Triangles);
        Mesh* AttachMesh(VertexArray* mesh, const char* materialFileName, const SkinningVertexAttribute* skinningVertexAttributes);


        /// Creates copies and attaches a list of bones.
        ///
        /// @remarks
        ///     If a bone of the same name already exists, this method will do nothing.
        void CopyAndAttachBones(const GTCore::Dictionary<Bone*> &bones);


        /// Copies the given animation.
        void CopyAnimation(const Animation &sourceAnimation, const GTCore::Map<Bone*, AnimationChannel*> &sourceAnimationChannelBones);


        /// Applies a transformation to the model's geometric data.
        void ApplyTransformation(const glm::mat4 &transform);

        /// Generates tangents and bitangents for every mesh.
        void GenerateTangentsAndBitangents();


        /// Retrieves the AABB of the base geometry.
        ///
        /// @param aabbMin [out] A reference to the variable that will receive the min corner.
        /// @param aabbMax [out] A reference to the variable that will receive that max corner.
        void GetBaseAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const;


        /// Retrieves the internal list of convex hulls of this model.
        const GTCore::Vector<ConvexHull*> & GetConvexHulls() const { return this->definition.GetConvexHulls(); }



        /// Called when the definition of the model has been modified.
        ///
        /// @remarks
        ///     When this is called, the model will have it's information updated based on the definition.
        void OnDefinitionChanged();



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the state of the model.
        ///
        /// @param serializer [in] A reference to the serializer to write to.
        ///
        /// @remarks
        ///     This is only used for saving the state of the model, and not for saving an actual .gtmodel file. This does not save vertex information and whatnot.
        ///     Instead it just saves the state that makes this instantiation different from other instantiations.
        void Serialize(GTCore::Serializer &serializer) const;

        /// Deserializes the state of the model.
        ///
        /// @param deserializer [in] A reference to the deserializer to read from.
        ///
        /// @remarks
        ///     The only retrieves state information. It assumes that the data being read has the same number of meshes as is currently loaded in the Model object.
        ///     @par
        ///     The way deserialization should work is that the model should already be loaded and instantiated before restoring the state. This should NOT be used
        ///     as a generic loading function for a mesh.
        void Deserialize(GTCore::Deserializer &deserializer);



    // Animation.
    public:

        /// Plays the given animation sequence.
        void PlayAnimation(const AnimationSequence &sequence);

        /// Stops the current animation.
        void StopAnimation();

        /// Pauses the current animation.
        void PauseAnimation();

        /// Resumes the animation if it was paused.
        void ResumeAnimation();

        /// Steps the current animation.
        void StepAnimation(double time);

        /// Determines whether or not the model is animating. Basically, this is used in determining whether or not the model should have vertex blending applied.
        bool IsAnimating() const;

        /// Sets the playback speed.
        void SetAnimationPlaybackSpeed(double speed);



    private:

        /// Finds a bone by name.
        ///
        /// @param name     [in] The name of the bone to retrieve.
        /// @param indexOut [in] A pointer to the variable that will receive the index.
        Bone* GetBoneByName(const char* name, size_t* indexOut = nullptr);

        /// Clears meshes, bones, etc.
        void Clear();



    public:

        /// The model definition.
        const ModelDefinition &definition;


        /// The list of meshes making up the model.
        GTCore::Vector<Mesh*> meshes;

        /// The list of bones in the model.
        GTCore::Vector<Bone*> bones;





        /// The base AABB of the model.
        mutable glm::vec3 aabbMin;
        mutable glm::vec3 aabbMax;

        /// Keeps track of whether or not the AABB is valid.
        mutable bool isAABBValid;


        /// The model's animation object.
        Animation animation;

        /// The bones associated with each channel in the animation.
        GTCore::Map<Bone*, AnimationChannel*> animationChannelBones;

        /// The cache of animation keys.
        GTCore::Vector<TransformAnimationKey*> animationKeyCache;

        /// The playback speed of animations.
        double animationPlaybackSpeed;


    private:    // No copying.
        Model(const Model &);
        Model & operator=(const Model &);
    };
}

#endif
