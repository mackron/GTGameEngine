
#ifndef __GTEngine_Mesh_hpp_
#define __GTEngine_Mesh_hpp_

#include "Rendering/VertexArray.hpp"
#include "Material.hpp"
#include "Armature.hpp"
#include "Physics.hpp"
#include "Math.hpp"

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
            : geometry(nullptr), material(nullptr), armature(nullptr),
              collisionVA(nullptr)
        {
        }

        /// Constructor.
        Mesh(VertexArray* geometry, Material* material, Armature* armature)
            : geometry(geometry), material(material), armature(armature),
              collisionVA(nullptr)
        {
        }

        /// Destructor.
        ///
        /// @remarks
        ///     The destructor does not delete the geometry, material or armature.
        ~Mesh()
        {
            delete this->collisionVA;
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

        /// Sets the skeleton of the mesh.
        void SetArmature(Armature* newArmature) { this->armature = newArmature; }


        /// Retrieves the geometry of the mesh.
              VertexArray* GetGeometry()       { return this->geometry; }
        const VertexArray* GetGeometry() const { return this->geometry; }

        /// Retrieves the material of the mesh.
              Material* GetMaterial()       { return this->material; }
        const Material* GetMaterial() const { return this->material; }

        /// Retrieves the armature of the mesh.
              Armature* GetArmature()       { return this->armature; }
        const Armature* GetArmature() const { return this->armature; }


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

        
        /// Fills the given vertex array will a skinned version of the base geometry using the current state of the mesh's armature.
        ///
        /// @remarks
        ///     This method asserts that <destination> is already pre-allocated.
        void ApplySkinning(VertexArray &destination);


    private:

        /// Recursively applies the blended geometry of the given bone.
        void ApplySkinning(const Bone &bone, const VertexFormat &format, const float* srcVertices, float* dstVertices);


    private:

        /// The vertex array containing the geometric data of the mesh.
        VertexArray* geometry;

        /// The material to use with this mesh.
        Material* material;

        /// The skeleton to use with this mesh for animations.
        Armature* armature;

        /// The vertex array for use with the collision shape.
        btTriangleIndexVertexArray* collisionVA;
    };
}

#endif
