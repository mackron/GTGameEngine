
#ifndef __GTEngine_Mesh_hpp_
#define __GTEngine_Mesh_hpp_

#include "Rendering/VertexArray.hpp"
#include "Material.hpp"
#include "Physics.hpp"
#include "Math.hpp"
#include "SkinningVertexAttribute.hpp"
#include "GarbageCollector.hpp"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)
#endif

namespace GTEngine
{
    /// Structure containing the skinning information of a mesh.
    struct MeshSkinningData
    {
        /// Constructor.
        MeshSkinningData(const Bone* const* bonesIn, const SkinningVertexAttribute* skinningVertexAttributesIn, const VertexArray &source)
            : bones(bonesIn),
              skinningVertexAttributes(skinningVertexAttributesIn),
              skinnedGeometry()
        {
            skinnedGeometry[0] = nullptr;      // <-- AllocateAnimatedGeometryArrays() deletes these before setting the new data. Thus, initting to null is required.
            skinnedGeometry[1] = nullptr;      // <-- as above
            this->AllocateAnimatedGeometryArrays(source);
        }

        /// Destructor.
        ~MeshSkinningData()
        {
            // Important to use the garbage collector here.
            GarbageCollector::MarkForCollection(this->skinnedGeometry[0]);
            GarbageCollector::MarkForCollection(this->skinnedGeometry[1]);
        }

        /// Allocates the vertex arrays for the animated geometry.
        void AllocateAnimatedGeometryArrays(const VertexArray &source)
        {
            delete this->skinnedGeometry[0];
            delete this->skinnedGeometry[1];

            this->skinnedGeometry[0] = new VertexArray(VertexArrayUsage_Stream, source.GetFormat());
            this->skinnedGeometry[1] = new VertexArray(VertexArrayUsage_Stream, source.GetFormat());

            this->skinnedGeometry[0]->SetData(nullptr, source.GetVertexCount(), source.GetIndexDataPtr(), source.GetIndexCount());
            this->skinnedGeometry[1]->SetData(nullptr, source.GetVertexCount(), source.GetIndexDataPtr(), source.GetIndexCount());
        }


        /// A pointer to the buffer containing pointers to the bones each vertex will index into.
        const Bone* const* bones;

        /// A pointer to the buffer containing the skinning vertex attributes for the CPU skinning shader.
        const SkinningVertexAttribute* skinningVertexAttributes;

        /// As a mesh is animated, it needs to store it's own local copy of the animated data (each mesh can be in a different animated
        /// state). Since one of these buffers will be used on the rendering thread, we'll need a separate one for the update thread.
        /// Thus, we store two copies of the buffers.
        ///
        /// TODO: Look into changing this with a cache or something. Storing copies of the buffers for each mesh may be too expensive.
        VertexArray* skinnedGeometry[2];
    };
};

namespace GTEngine
{
    /// Class representing a mesh.
    ///
    /// A mesh is different to a model in that the mesh is a lower level chunck of geometry with a single material and (optionally) a
    /// single skeleton. A model is made up of multiple meshes.
    ///
    /// A mesh can be thought of as a container for various properties. The first property is the geometric data. The geometric data
    /// is represented with a vertex array (the VertexArray class). The material is represented with a Material object. The skeleton
    /// is represented with a Skeleton object.
    ///
    /// The mesh is not responsible for creating or deleting it's properties. The vertex array, material and skeleton must be created
    /// and deleted at a higher level.
    class Mesh
    {
    public:

        /// Default constructor.
        Mesh();

        /// Destructor.
        ///
        /// @remarks
        ///     The destructor does not delete the geometry vertex array or material.
        ~Mesh();


        /// Sets the geometry of the mesh.
        ///
        /// @param newGeometry [in] A pointer to the vertex array containing the meshes new geometry.
        ///
        /// @remarks
        ///     Remember to consider that a change in the structure of the meshes geometry may break the animations defined by the current skeleton.
        void SetGeometry(VertexArray* newGeometry) { this->geometry = newGeometry; }

        /// Sets the material of the mesh.
        ///
        /// @param newMaterial [in] A pointer to the new material. Can be nullptr.
        ///
        /// @remarks
        ///     When this method is used for setting the material, it is the responsibility of the caller to manage the deletion of the material.
        void SetMaterial(Material* newMaterial);

        /// Loads and sets the material of the mesh.
        ///
        /// @param materialFileName [in] The file name of the material to load and use as the material for this mesh.
        ///
        /// @remarks
        ///     When this method is used for setting the material, the mesh wi
        void SetMaterial(const char* materialFileName);


        /// Retrieves the base geometry of the mesh.
        const VertexArray* GetGeometry() const { return this->geometry; }
              VertexArray* GetGeometry()       { return this->geometry; }

        /// Retrieves a pointer to the vertex array containing the skinned geometry.
        ///
        /// @remarks
        ///     If the mesh is not animated, this will return the base geometry vertex array.
        ///     @par
        ///     There are two vertex arrays storing skinned geometry. One is being used by the rendering thread and the other is being used by the update
        ///     thread. This is where the <rcIndex> comes in. It simply controls
              VertexArray* GetSkinnedGeometry();
        const VertexArray* GetSkinnedGeometry() const;
        

        /// Retrieves the material of the mesh.
              Material* GetMaterial()       { return this->material; }
        const Material* GetMaterial() const { return this->material; }
        


        /// Sets the animation data for the mesh.
        ///
        /// @param bones                    [in] A pointer to the buffer containing pointers to the bones the skinning vertex attributes will index in to.
        /// @param skinningVertexAttributes [in] A pointer to the buffer containing the skinning vertex attributes for each vertex.
        void SetSkinningData(const Bone* const* bones, const SkinningVertexAttribute* skinningVertexAttributes);



        /// Generates the tangents and binormals.
        ///
        /// @remarks
        ///     This method uses VertexAttribs::Position, VertexAttribs::Normal and VertexAttribs::TexCoord for input and VertexAttribs::Tangent and VertexAttribs::Bitangent
        ///     for output. If any are not present, the method will fail.
        bool GenerateTangentsAndBitangents();

        
        /// Builds a tri-mesh collision shape based on this mesh.
        /// @param scale [in] The scale to apply to the mesh when generating the shape.
        ///
        /// @remarks
        ///     Deletes the returned shape with 'delete'.
        //btGImpactMeshShape* BuildCollisionShape(const glm::vec3 &scale = glm::vec3(1.0f, 1.0f, 1.0f));

        
        /// Fills the given vertex array will a skinned version of the base geometry using the current state of the mesh's bones.
        ///
        /// @remarks
        ///     This method asserts that <destination> is already pre-allocated.
        void ApplySkinning();



        /// Retrieves a pointer to the animated geometry of the given index.
        ///
        /// @param index [in] The index of the animated geometry to retrieve - 0 or 1.
        //VertexArray* GetAnimatedGeometry(size_t index);



    private:

        /// The vertex array containing the geometric data of the mesh.
        VertexArray* geometry;

        /// The material to use with this mesh.
        Material* material;

        /// A pointer to the skinning data of the mesh. This will be set to null if the mesh is not animated.
        MeshSkinningData* skinningData;

        /// Keeps track of whether or not the mesh has been animated. This is needed so we know whether or not to return the base
        /// geometry or skinned geometry for an animated mesh. If the mesh has not yet been animated, the base geometry needs to
        /// be used.
        bool hasAnimated;

        /// Keeps track of whether or not the material should be deleted.
        bool deleteMaterial;
    };
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
