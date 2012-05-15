
#ifndef __GTEngine_Model_hpp_
#define __GTEngine_Model_hpp_

#include "Mesh.hpp"
#include "ArmatureAnimation.hpp"
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
        ///     The new mesh will use <mesh> and <material> directory, but <armature> will be copied. The reason for this is that the input
        ///     armature will be pointing to different bones.
        ///     @par
        ///     For armatures to work correctly, the bones must have been set first with CopyAndAttachBones(). If this is not the case, the
        ///     armature will be incorrect.
        Mesh* AttachMesh(VertexArray* mesh, Material* material, const Armature* armature);

        /// Creates copies and attaches a list of bones.
        ///
        /// @remarks
        ///     If a bone of the same name already exists, this method will do nothing.
        void CopyAndAttachBones(const GTCore::Dictionary<BoneWithWeights*> &bones);

        /// Creates copies and adds a list of animations.
        void CopyAndAddAnimations(const GTCore::Dictionary<ArmatureAnimation*> &animations);


        /// Applies a transformation to the model's geometric data.
        void ApplyTransformation(const glm::mat4 &transform);

        /// Generates tangents and bitangents for every mesh.
        void GenerateTangentsAndBitangents();


        /// Determines whether or not the model is animating. Basically, this is used in determining whether or not the model should have vertex blending applied.
        bool IsAnimating() const;


    // Animation.
    public:

        /// Plays the given animation.
        ///
        /// @param animationName [in] The name of the animation to start playing.
        void PlayAnimation(const char* animationName, bool loop = true);

        /// Completely stops the current animation, returning the model to it's default pose.
        void StopAnimation();

        /// Pauses the current animation.
        void PauseAnimation();

        /// Resumes the animation if it was paused.
        void ResumeAnimation();

        /// Retrieves a pointer to the current ArmatureAnimation object.
        ArmatureAnimation* GetCurrentAnimation();

        /// Retrieves the name of the current animation.
        const char* GetCurrentAnimationName() const;

        /// Steps the current animation by the given time amount.
        ///
        /// @param deltaTimeInSeconds [in] The amount of seconds to step the animation by.
        void StepAnimation(double deltaTimeInSeconds);


    private:

        /// Private method for recursively attaching a bone to the given mesh.
        ///
        /// @param mesh [in] A reference to the mesh whose having the bone information added.
        /// @param bone [in] A reference to the bone containing the vertex/weight information for the mesh.
        ///
        /// @remarks
        ///     This is called recursively on all of the bone's children.
        // TODO: This should not be recursive! What happens if there are two bones sharing the same parent, but each bone is for a different mesh? This requires a change in the design or armatures.
        void AddBoneToMesh(Mesh &mesh, const BoneWithWeights &bone);



    public:

        /// The list of meshes making up the model.
        GTCore::Vector<Mesh*> meshes;

        /// The list of bones in the model.
        GTCore::Dictionary<Bone*> bones;

        /// The list of animations. The bones referenced in these animations are pointers to the bones in <bones>
        GTCore::Dictionary<ArmatureAnimation*> animations;

        /// A pointer to the animation that is currently being played. Set to null when no animation is playing.
        ArmatureAnimation* currentAnimation;
    };    
}

#endif
