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

        /// Structure representing a mesh in the model definition.
        struct Mesh
        {
            /// Constructor.
            Mesh()
                : name(), geometry(nullptr), material(nullptr), skinningVertexAttributes(nullptr)
            {
            }

            /// Copy constructor.
            Mesh(const Mesh &other)
                : name(other.name), geometry(other.geometry), material(other.material), skinningVertexAttributes(other.skinningVertexAttributes)
            {
            }

            /// The name of the mesh.
            GTCore::String name;

            /// The vertex array containing the base geeometry of the mesh.
            GTEngine::VertexArray* geometry;

            /// The material to use when drawing the mesh.
            GTEngine::Material* material;

            /// The skinning vertex attributes. This is a pointer to an array buffer whose size is equal to the number of vertices in the mesh. This can
            /// be null, in which case it is considered a static mesh.
            SkinningVertexAttribute* skinningVertexAttributes;
        };



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



        /// Clears the meshes.
        void ClearMeshes();

        /// Clears the bones.
        void ClearBones();

        /// Clears the animations of the definition.
        void ClearAnimations(bool clearNamedSegments = false);

        /// Clears the named animation segments.
        void ClearNamedAnimationSegments();

        /// Clears the convex hulls.
        void ClearConvexHulls();


        /// Determines whether or not the definition has convex hulls.
        bool HasConvexHulls() const { return this->convexHulls.count > 0; }



        /// Adds a mesh to the definition.
        ///
        /// @param mesh [in] A reference to the mesh to add.
        void AddMesh(const ModelDefinition::Mesh &mesh);

        /// Retrieves a pointer to the mesh with the given name.
        ///
        /// @param meshName [in] The name of the mesh to retrieve.
        ///
        /// @return A pointer to the mesh if it exists, null otherwise.
        ///
        /// @remarks
        ///     The pointer will be made invalid when a mesh is added or removed.
              ModelDefinition::Mesh* GetMeshByName(const char* meshName);
        const ModelDefinition::Mesh* GetMeshByName(const char* meshName) const;


        /// Adds a bone to the definition.
        ///
        /// @param bone [in] A pointer to the bone to add.
        ///
        /// @return The index of the bone.
        size_t AddBone(Bone* bone);

        /// Retrieves a pointer to the bone with the given name.
        ///
        /// @param boneName [in] The name of the bone to retrieve.
        ///
        /// @return A pointer to the bone if it exists, null otherwise.
              Bone* GetBoneByName(const char* boneName);
        const Bone* GetBoneByName(const char* boneName) const;

        /// Finds the index of the given bone.
        ///
        /// @param bone     [in] A reference to the bone to search for.
        /// @param indexOut [in] A reference to the variable that will receive the index.
        ///
        /// @return True if the bone was found, false otherwise.
        bool FindBoneIndex(const Bone* bone, size_t &indexOut) const;
        bool FindBoneIndex(const Bone& bone, size_t &indexOut) const { return FindBoneIndex(&bone, indexOut); }
        
        /// Finds the index of the given bone by it's name.
        ///
        /// @param boneName [in] A reference to the bone to search for.
        /// @param indexOut [in] A reference to the variable that will receive the index.
        ///
        /// @return True if the bone was found, false otherwise.
        bool FindBoneIndex(const char* boneName, size_t &indexOut) const;

        /// Retrieves the number of bones in the definition.
        size_t GetBoneCount() const;



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

        /// The absolute file path. This is of the foreign (source) file.
        GTCore::String absolutePath;

        /// The relative file path. This is of the foreign (source) file.
        GTCore::String relativePath;


        /// The list of meshes.
        GTCore::Vector<ModelDefinition::Mesh> meshes;

        /// A map of every bone of the model, indexed by it's name. We use a map here to make it easier for avoiding duplication and
        /// also fast lookups.
        GTCore::Vector<Bone*> bones;


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