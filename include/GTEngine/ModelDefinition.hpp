
#ifndef __GTEngine_ModelDefinition_hpp_
#define __GTEngine_ModelDefinition_hpp_

#include "Rendering/VertexArray.hpp"
#include "Material.hpp"
#include "Bone.hpp"
#include "Animation/Animation.hpp"
#include <GTCore/Vector.hpp>


namespace GTEngine
{
    /// Class representing the base definition of a model.
    ///
    /// When a model is loaded from a file, it loads the definition, and then creates a model instantiation from that definition. Definitions
    /// are mapped to file names, which is how a library will know to re-use an already-loaded definition.
    class ModelDefinition
    {
    public:

        /// Constructor.
        ModelDefinition(const char* fileName = "");

        /// Destructor.
        ~ModelDefinition();


        /// Generates the tangents and bitangents of the mesh geometry.
        void GenerateTangentsAndBitangents();

        /// Creates an animation key for the given bone and returns it.
        TransformAnimationKey* CreateAnimationKey(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale);

        /// Maps an animation channel to a bone.
        void MapAnimationChannelToBone(Bone &bone, AnimationChannel &channel);


    public:

        /// The name of the file used to create the definition. This will be an empty string if it was not created from a file.
        GTCore::String fileName;


        /// The vertex arrays containing the mesh data. This is always in P3T2N3T3B3 format.
        GTCore::Vector<GTEngine::VertexArray*> meshGeometries;

        /// Pointers to the materials for each mesh.
        GTCore::Vector<GTEngine::Material*> meshMaterials;

        /// The list of bone weights for each mesh. If the mesh does not use any bones, the entry will be set to null.
        GTCore::Vector<GTCore::Vector<BoneWeights*>*> meshBones;


        /// A map of every bone of the model, indexed by it's name. We use a map here to make it easier for avoiding duplication and
        /// also fast lookups.
        GTCore::Dictionary<Bone*> bones;


        /// The model's animation object.
        Animation animation;

        /// The map for mapping a bone to an animation channel.
        GTCore::Map<Bone*, AnimationChannel*> animationChannelBones;

        /// The cache of animation keys.
        GTCore::Vector<TransformAnimationKey*> animationKeyCache;
    };
}

#endif