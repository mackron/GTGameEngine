
#ifndef __GTEngine_Mesh_hpp_
#define __GTEngine_Mesh_hpp_

#include "Rendering/VertexArray.hpp"
#include "Material.hpp"
#include "Physics.hpp"
#include "Math.hpp"
#include "SkinningVertexAttribute.hpp"

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
        MeshSkinningData(VertexArray &source)
            : skinningVertexAttributes(nullptr),
              animatedGeometry()
        {
            skinningVertexAttributes = new SkinningVertexAttribute[source.GetVertexCount()];
            
            animatedGeometry[0] = nullptr;      // <-- AllocateAnimatedGeometryArrays() deletes these before setting the new data. Thus, initting to null is required.
            animatedGeometry[1] = nullptr;      // <-- as above
            this->AllocateAnimatedGeometryArrays(source);
        }

        /// Destructor.
        ~MeshSkinningData()
        {
            delete [] this->skinningVertexAttributes;
            delete this->animatedGeometry[0];
            delete this->animatedGeometry[1];
        }

        /// Allocates the vertex arrays for the animated geometry.
        void AllocateAnimatedGeometryArrays(VertexArray &source)
        {
            delete this->animatedGeometry[0];
            delete this->animatedGeometry[1];

            this->animatedGeometry[0] = new VertexArray(VertexArrayUsage_Stream, source.GetFormat());
            this->animatedGeometry[1] = new VertexArray(VertexArrayUsage_Stream, source.GetFormat());

            this->animatedGeometry[0]->SetData(nullptr, source.GetVertexCount(), source.GetIndexDataPtr(), source.GetIndexCount());
            this->animatedGeometry[1]->SetData(nullptr, source.GetVertexCount(), source.GetIndexDataPtr(), source.GetIndexCount());
        }


        /// A pointer to the buffer containing the skinning vertex attributes for the CPU skinning shader.
        SkinningVertexAttribute* skinningVertexAttributes;

        /// As a mesh is animated, it needs to store it's own local copy of the animated data (each mesh can be in a different animated
        /// state). Since one of these buffers will be used on the rendering thread, we'll need a separate one for the update thread.
        /// Thus, we store two copies of the buffers.
        ///
        /// TODO: Look into changing this with a cache or something. Storing copies of the buffers for each mesh may be too expensive on memory.
        VertexArray* animatedGeometry[2];
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
        Mesh()
            : geometry(nullptr), material(nullptr),
              collisionVA(nullptr), skinningData(nullptr)
        {
        }

        /// Constructor.
        Mesh(VertexArray* geometry, Material* material)
            : geometry(geometry), material(material),
              collisionVA(nullptr), skinningData(nullptr)
        {
        }

        /// Destructor.
        ///
        /// @remarks
        ///     The destructor does not delete the geometry vertex array or material.
        ~Mesh()
        {
            delete this->collisionVA;
            delete this->skinningData;
        }


        /// Sets the geometry of the mesh.
        ///
        /// @param newGeometry [in] A pointer to the vertex array containing the meshes new geometry.
        ///
        /// @remarks
        ///     Remember to consider that a change in the structure of the meshes geometry may break the animations defined by the current skeleton.
        void SetGeometry(VertexArray* newGeometry) { this->geometry = newGeometry; }

        /// Sets the material of the mesh.
        void SetMaterial(Material* newMaterial) { this->material = newMaterial; }


        /// Retrieves the geometry of the mesh.
              VertexArray* GetGeometry()       { return this->geometry; }
        const VertexArray* GetGeometry() const { return this->geometry; }

        /// Retrieves the material of the mesh.
              Material* GetMaterial()       { return this->material; }
        const Material* GetMaterial() const { return this->material; }


        /// Attaches the bone weights using a local bone and a source bone.
        ///
        /// @param localBone    [in] A reference to the bone the mesh will be directly referening when doing skinning.
        /// @param weightCount  [in] The number of weights in the weight buffer.
        /// @param weightBuffer [in] A pointer to the buffer containing the vertex/weight pairs.
        void AttachBoneWeights(const Bone &localBone, size_t weightCount, const VertexWeightPair* weightBuffer);



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
        btGImpactMeshShape* BuildCollisionShape(const glm::vec3 &scale = glm::vec3(1.0f, 1.0f, 1.0f));

        
        /// Fills the given vertex array will a skinned version of the base geometry using the current state of the mesh's bones.
        ///
        /// @remarks
        ///     This method asserts that <destination> is already pre-allocated.
        void ApplySkinning(VertexArray &destination);



        /// Retrieves a pointer to the animated geometry of the given index.
        ///
        /// @param index [in] The index of the animated geometry to retrieve - 0 or 1.
        VertexArray* GetAnimatedGeometry(size_t index);



    private:

        /// The vertex array containing the geometric data of the mesh.
        VertexArray* geometry;

        /// The material to use with this mesh.
        Material* material;

        /// The vertex array for use with the collision shape.
        btTriangleIndexVertexArray* collisionVA;

        /// A pointer to the skinning data of the mesh. This will be set to null if the mesh is not animated.
        MeshSkinningData* skinningData;
    };
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
