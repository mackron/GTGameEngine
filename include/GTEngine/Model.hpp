
#ifndef __GTEngine_Model_hpp_
#define __GTEngine_Model_hpp_

#include "Rendering/VertexArray.hpp"
#include "Material.hpp"
#include "Physics.hpp"
#include "Math.hpp"
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    /**
    *   \brief  Class representing a mesh.
    *
    *   \remarks
    *       A mesh owns the vertex array that is attached to it. It will delete it with 'delete' in it's destructor.
    */
    struct Mesh
    {
        Mesh()
            : va(nullptr), collisionVA(nullptr), collisionShape(nullptr)
        {
        }

        Mesh(VertexArray *va, bool buildCollisionShape = true)
            : va(va), collisionVA(nullptr), collisionShape(nullptr)
        {
            if (buildCollisionShape)
            {
                this->RebuildCollisionShape();
            }
        }

        ~Mesh()
        {
            delete va;
            delete collisionShape;
            delete collisionVA;     // <-- should come after deleting the shape.
        }

        /**
        *   \brief  Rebuilds the collision shape.
        */
        void RebuildCollisionShape()
        {
            delete this->collisionShape;
            delete this->collisionVA;

            auto & vaFormat    = this->va->GetFormat();
            auto   indexCount  = this->va->GetIndexCount();
            auto   indexData   = reinterpret_cast<int *>(this->va->MapIndexData());
            auto   vertexCount = this->va->GetVertexCount();
            auto   vertexData  = this->va->MapVertexData();

            auto   positionOffset = vaFormat.GetAttributeOffset(VertexAttribs::Position);

            this->collisionVA = new btTriangleIndexVertexArray
            (
                static_cast<int>(indexCount / 3), indexData, 3 * sizeof(unsigned int),
                static_cast<int>(vertexCount), vertexData + positionOffset, static_cast<int>(vaFormat.GetSizeInBytes())
            );

            this->collisionShape = new btGImpactMeshShape(this->collisionVA);
            this->collisionShape->updateBound();        // <-- must call this, otherwise the AABB will be incorrect.

            this->va->UnmapIndexData();
            this->va->UnmapVertexData();
        }


        // TOOD: Scaling works, but we need to check that we can reuse the same vertex array.

        /// Builds a tri-mesh collision shape based on this mesh.
        /// @param scale [in] The scale to apply to the mesh when generating the shape.
        ///
        /// @remarks
        ///     Deletes the returned shape with 'delete'.
        btGImpactMeshShape* BuildCollisionShape(const glm::vec3 &scale = glm::vec3(1.0f, 1.0f, 1.0f))
        {
            auto &vaFormat    = this->va->GetFormat();
            auto  indexCount  = this->va->GetIndexCount();
            auto  vertexCount = this->va->GetVertexCount();
            auto  indexData   = reinterpret_cast<int *>(this->va->MapIndexData());
            auto  vertexData  = this->va->MapVertexData();

            auto  positionOffset = vaFormat.GetAttributeOffset(VertexAttribs::Position);

            this->collisionVA = new btTriangleIndexVertexArray
            (
                static_cast<int>(indexCount / 3), indexData, 3 * sizeof(unsigned int),
                static_cast<int>(vertexCount), vertexData + positionOffset, static_cast<int>(vaFormat.GetSizeInBytes())
            );

            auto newCollisionShape = new btGImpactMeshShape(this->collisionVA);
            newCollisionShape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
            newCollisionShape->updateBound();        // <-- must call this, otherwise the AABB will be incorrect.

            this->va->UnmapIndexData();
            this->va->UnmapVertexData();

            return newCollisionShape;
        }



        /// The vertex array containing the mesh data.
        VertexArray* va;

        /// The vertex array for use with the collision shape.
        btTriangleIndexVertexArray* collisionVA;

        /// The collision shape that can be used with this mesh.
        btGImpactMeshShape* collisionShape;


    private:    // No copying.
        Mesh(const Mesh &);
        Mesh & operator=(const Mesh &);
    };

    /**
    *   \brief  Class representing a model.
    *
    *   A model contains an arbitrary number of meshes, materials and animation data. 
    */
    class Model
    {
    public:

        Model();
        ~Model();

        /**
        *   \brief  Attaches a mesh to the model.
        */
        void AttachMesh(VertexArray* mesh, Material* material, bool buildCollisionShape = true);

        /**
        *   \brief  Applies a transformation to the model's geometric data.
        */
        void ApplyTransformation(const glm::mat4 &transform);

        /**
        *   \brief  Retrieves the AABB of the model.
        */
        const btAABB & GetAABB() const;

        /**
        *   \brief  Retrieves the collision shape that can be used for collision detection against this model.
        */
        const btCollisionShape & GetCollisionShape() const;

        /**
        *   \brief  Retrieves the vertex array for the AABB.
        */
        const VertexArray * GetAABBVertexArray() const;

    private:

        /**
        *   \brief  Updates the AABB of this model.
        */
        void UpdateAABB();


    public:

        // The list of vertex array meshes making up the model.
        GTCore::Vector<Mesh*> meshes;

        // The list of materials used by each mesh. This is a 1:1 mapping of 'meshes'.
        GTCore::Vector<Material*> materials;


        /// The collision shape that can be used with this mesh. This is a compound shape containing the collision shapes of each individual mesh.
        btCompoundShape collisionShape;

        /// The AABB for this model.
        btAABB aabb;

        /// The vertex array that can be used to draw the model's AABB.
        VertexArray * aabbVA;
    };    
}

#endif
