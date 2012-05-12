
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
        /// @remarks
        ///     The new mesh will use <mesh> and <material> directory, but <armature> will be copied. The reason for this is that the input
        ///     armature will be pointing to different bones.
        ///     @par
        ///     For armatures to work correctly, the bones must have been set first with CopyAndAttachBones(). If this is not the case, the
        ///     armature will be incorrect.
        void AttachMesh(VertexArray* mesh, Material* material, const Armature* armature);

        /// Creates copies and attaches a list of bones.
        ///
        /// @remarks
        ///     If a bone of the same name already exists, this method will do nothing.
        void CopyAndAttachBones(const GTCore::Dictionary<Bone*> &bones);


        /// Applies a transformation to the model's geometric data.
        void ApplyTransformation(const glm::mat4 &transform);

        /// Generates tangents and bitangents for every mesh.
        void GenerateTangentsAndBitangents();


    public:

        /// The list of meshes making up the model.
        GTCore::Vector<Mesh*> meshes;

        /// The list of bones in the model.
        GTCore::Dictionary<Bone*> bones;

        /// The list of animations. The bones referenced in these animations are pointers to the bones in <bones>
        GTCore::Dictionary<ArmatureAnimation*> animations;
    };    
}

#endif
