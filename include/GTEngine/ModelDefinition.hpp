// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ModelDefinition_hpp_
#define __GTEngine_ModelDefinition_hpp_

#include "Rendering/VertexArray.hpp"
#include "Material.hpp"
#include "Bone.hpp"
#include "Mesh.hpp"
#include "ConvexHull.hpp"
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
        ModelDefinition();

        /// Destructor.
        ~ModelDefinition();


        /// Loads the model definition from a file.
        ///
        /// @param relativePathIn [in] The relative path of the file. Only used when 'fileName' is absolute. See remarks.
        ///
        /// @remarks
        ///     This method will not clear data that is not present in the specified file. For example, if you first load a .gtmodel file,
        ///     which contains convex hulls, and then load the corresponding .dae file (Collada file), it will NOT clear the convex hulls.
        ///     The purpose behind this system is to make it so reloading a model does not delete information. To clear data, use the
        ///     Clear*() methods before loading.
        bool LoadFromFile(const char* relativePathIn);
        
        /// Internal implementation of LoadFromFile(const char*) that should only be used internally.
        ///
        /// @param fileName       [in]  The path of the file to load.
        /// @param relativePath   [in]  The relative path of the file. Only used when 'fileName' is absolute.
        /// @param needsSerialize [out] A reference to a boolean that will receive a value determining whether or not the model needs to be serialized.
        ///
        /// @remarks
        ///     The relative path of model (and every other asset) must be known. Thus, if 'fileName' is absolute, a relative path must
        ///     be specified. Typically, absolute paths will only ever be passed to this method when called by the engine.
        ///
        /// @see
        ///     GTEngine::ModelDefinition::LoadFromFile(const char*)
        bool LoadFromFile(const char* fileNameIn, const char* relativePathIn, bool &needsSerialize);


        /// Retrieves the absolute path of the file.
        ///
        /// @remarks
        ///     The returned string is of the original source file that was used to generate the native file.
        const char* GetAbsolutePath() const;

        /// Retrieves the relative path of the file.
        ///
        /// @remarks
        ///     The returned string is of the original source file that was used to generate the native file.
        const char* GetRelativePath() const;


        /// Generates the tangents and bitangents of the mesh geometry.
        void GenerateTangentsAndBitangents();

        /// Creates an animation key for the given bone and returns it.
        TransformAnimationKey* CreateAnimationKey(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale);

        /// Maps an animation channel to a bone.
        void MapAnimationChannelToBone(Bone &bone, AnimationChannel &channel);


        /// Builds the convex decomposition of the model.
        ///
        /// @remarks
        ///     This builds the composition based off the static mesh data. Thus, this should only be used for non-animated models.
        void BuildConvexDecomposition(ConvexHullBuildSettings &settings);
        void BuildConvexHulls(ConvexHullBuildSettings &settings) { this->BuildConvexDecomposition(settings); }

        /// Retrieves a constant reference to the internal list of convex hulls for this model.
        const GTCore::Vector<ConvexHull*> & GetConvexHulls() const { return this->convexHulls; }



        /// Clears the mesh geometry.
        void ClearMeshGeometries();

        /// Clears the meshes skinning vertex attributes.
        void ClearMeshSkinningVertexAttributes();

        /// Clears the bones.
        void ClearBones();

        /// Clears the animations of the definition.
        void ClearAnimations(bool clearNamedSegments = false);

        /// Clears the named animation segments.
        void ClearNamedAnimationSegments();

        /// Clears the materials.
        void ClearMaterials();

        /// Clears the convex hulls.
        void ClearConvexHulls();


        
        /// Determines whether or not the definition has convex hulls.
        bool HasConvexHulls() const { return this->convexHulls.count > 0; }


        ////////////////////////////////////////////////////////
        // Serialization/Deserialization

        /// Serializes the model definition.
        void Serialize(GTCore::Serializer &serializer) const;

        /// Deserializes the model definition.
        ///
        /// @return True if successful; false otherwise.
        bool Deserialize(GTCore::Deserializer &deserializer);


    private:

        /// Loads the model definition from a native .gtmodel file.
        ///
        /// @param absolutePathIn [in] The absolute path of the file to load.
        ///
        /// @return True if successful; false otherwise.
        bool LoadFromNativeFile(const GTCore::String &absolutePathIn);

        /// Loads a model definition from a non-native file (a non-gtmodel file).
        ///
        /// @param absolutePathIn [in] The absolute path of the file to load.
        ///
        /// @return True if successful; false otherwise.
        bool LoadFromForeignFile(const GTCore::String &absolutePathIn);

        /// Loads a model definition from an Assimp-supported file.
        ///
        /// @param absolutePathIn [in] the absolute path of the file to load.
        ///
        /// @return True if successful; false otherwise.
        bool LoadFromAssimpFile(const GTCore::String &absolutePathIn);


    public:

        /// The name of the file used to create the definition. This will be an empty string if it was not created from a file.
        //GTCore::String fileName;

        /// The absolute file path. This is of the foreign (source) file.
        GTCore::String absolutePath;

        /// The relative file path. This is of the foreign (source) file.
        GTCore::String relativePath;


        // TODO: Look into combinging these separated arrays in a ModelDefinitionMesh object to make things a little clearer.

        /// The vertex arrays containing the mesh data. This is always in P3T2N3T3B3 format.
        GTCore::Vector<GTEngine::VertexArray*> meshGeometries;

        /// Pointers to the materials for each mesh.
        GTCore::Vector<GTEngine::Material*> meshMaterials;

        /// The list of skinning vertex attributes for each mesh. The value is a pointer to a buffer containing the skinning attributes
        /// for each vertex in the mesh.
        GTCore::Vector<SkinningVertexAttribute*> meshSkinningVertexAttributes;


        /// A map of every bone of the model, indexed by it's name. We use a map here to make it easier for avoiding duplication and
        /// also fast lookups.
        GTCore::Dictionary<Bone*> bones;


        /// The model's animation object.
        Animation animation;

        /// The map for mapping a bone to an animation channel.
        GTCore::Map<Bone*, AnimationChannel*> animationChannelBones;

        /// The cache of animation keys.
        GTCore::Vector<TransformAnimationKey*> animationKeyCache;


        /// The list of the convex hulls making up the convex decomposition of this model. This is either loaded from the model's file or
        /// generated with BuildConvexDecomposition().
        GTCore::Vector<ConvexHull*> convexHulls;

        /// The settings that were used to build the convex hulls.
        ConvexHullBuildSettings convexHullBuildSettings;
    };
}

#endif