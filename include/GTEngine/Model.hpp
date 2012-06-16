
#ifndef __GTEngine_Model_hpp_
#define __GTEngine_Model_hpp_

#include "Mesh.hpp"
#include "Animation/Animation.hpp"
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    /// Class representing a model.
    ///
    /// A model contains an arbitrary number of meshes, materials and animation data. 
    class Model
    {
    public:

        /// Constructor.
        Model();

        /// Destructor.
        ~Model();

        /// Attaches a mesh to the model.
        ///
        /// @return A pointer to the new mesh.
        ///
        /// @remarks
        ///     It's important that <bones> refers to a list of BoneWithWeights pointers since the vertex/weight information will be needed to set the skinning vertex attributes.
        Mesh* AttachMesh(VertexArray* mesh, Material* material);
        Mesh* AttachMesh(VertexArray* mesh, Material* material, const GTCore::Vector<BoneWeights*> &bones);
        

        /// Creates copies and attaches a list of bones.
        ///
        /// @remarks
        ///     If a bone of the same name already exists, this method will do nothing.
        void CopyAndAttachBones(const GTCore::Dictionary<Bone*> &bones);


        /// Copies the given animation.
        void CopyAnimation(Animation &sourceAnimation, GTCore::Map<AnimationChannel*, Bone*> &sourceAnimationChannelBones);


        /// Applies a transformation to the model's geometric data.
        void ApplyTransformation(const glm::mat4 &transform);

        /// Generates tangents and bitangents for every mesh.
        void GenerateTangentsAndBitangents();


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

        /// Priave method for attaching a bone to the given mesh.
        ///
        /// @param mesh [in] A reference to the mesh whose having the bone information added.
        /// @param bone [in] A reference to the VertexWeights object containing the weights to use for each applicable vertex.
        void AddBoneWeightsToMesh(Mesh &mesh, const BoneWeights &bone);



    public:

        /// The list of meshes making up the model.
        GTCore::Vector<Mesh*> meshes;

        /// The list of bones in the model.
        GTCore::Dictionary<Bone*> bones;


        /// The model's animation object.
        Animation animation;

        /// The bones associated with each channel in the animation.
        GTCore::Map<AnimationChannel*, Bone*> animationChannelBones;

        /// The cache of animation keys.
        GTCore::Vector<TransformAnimationKey*> animationKeyCache;

        /// The playback speed of animations.
        double animationPlaybackSpeed;
    };    
}

#endif
