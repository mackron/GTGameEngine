// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsModelObject_hpp_
#define __GT_GraphicsModelObject_hpp_

#include <GTLib/Vector.hpp>
#include "GraphicsWorld.hpp"
#include "../AlignedType.hpp"


#include <limits.h>

namespace GT
{
    class GraphicsAssetResource_Mesh;
    class GraphicsAssetResource_Model;

    /// High-level class representing a full model in a graphics world.
    class GraphicsModelObject
    {
    public:

        /// Constructor.
        GraphicsModelObject();

        /// Destructor.
        ~GraphicsModelObject();


        /// Initialises the model object based on the given model resource and adds the objects to the given graphics world.
        ///
        /// @param pModelResource [in] A pointer to the model resource.
        /// @param pGraphicsWorld [in] A pointer to the graphics world to add the objects to.
        ///
        /// @remarks
        ///     The model resource must have been created against the same graphics world.
        bool Initialize(const GraphicsAssetResource_Model* pModelResource, GraphicsWorld* pGraphicsWorld, const vec4 &position, const quat &rotation, const vec4 &scale);

        /// Uninitializes the model object and removes the mesh objects from the graphics world.
        void Uninitialize();


        /// Sets the transform of the model.
        void SetTransform(const vec4 &position, const quat &rotation, const vec4 &scale);


    private:

        /// Structure representing an individual mesh.
        struct alignas(16) MeshObject : public SIMDAlignedType
        {
            /// The local position relative to the mesh's parent.
            vec4 localPosition;

            /// The local rotation relative to the mesh's parent.
            quat localRotation;

            /// The local rotation relative to the mesh's parent.
            vec4 localScale;

            /// A handle to the mesh object created by the graphics world.
            HGraphicsObject hGraphicsObject;

            /// A pointer to the mesh resource the object was created from.
            GraphicsAssetResource_Mesh* pMeshResource;


#if UINTPTR_MAX == UINT32_MAX
            uint32_t padding0;  //< Padding for alignment requirements. Not used. Only defined in 32-bit builds.
#endif
        };


    private:

        /// The model resource that the model object was initialized with.
        const GraphicsAssetResource_Model* m_pModelResource;

        /// The graphcis world that the model object was initialized with.
        GraphicsWorld* m_pGraphicsWorld;


        /// The list of meshes.
        GTLib::Vector<MeshObject> m_meshes;
    };
}

#endif
