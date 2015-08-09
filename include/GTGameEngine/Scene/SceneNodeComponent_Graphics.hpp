// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneNodeComponent_Graphics_hpp_
#define __GT_SceneNodeComponent_Graphics_hpp_

#include "SceneNodeComponent.hpp"
#include "SceneNodeComponentDescriptor_Graphics.hpp"
#include "../Graphics/GraphicsWorld.hpp"

namespace GT
{
    class EngineContext;

    class SceneNodeComponent_Graphics : public SceneNodeComponent
    {
    public:

        /// Constructor.
        SceneNodeComponent_Graphics(GraphicsWorld* pGraphicsWorld);

        /// Destructor.
        ~SceneNodeComponent_Graphics();


        /// Sets the model to use with this graphics component.
        ///
        /// @param modelPath     [in] The path of the model asset to set as the model.
        /// @param engineContext [in] A reference to the engine context. This is used to gain access to the asset manager so the model can be loaded.
        ///
        /// @remarks
        ///     If the model has not already been pre-loaded, this will do a full load. This will also load any assets that the model depends on such
        ///     as materials and textures.
        bool SetModel(const char* modelPath, EngineContext &engineContext);



        /// Sets the mesh resource to draw.
        void SetMeshObject(HGraphicsObject hMeshObject);

        HGraphicsObject GetMeshObject() const;



        ///////////////////////////////////////////
        // Statics

        static SceneNodeComponentTypeID GetTypeID() {
            return SceneNodeComponentDescriptor_Graphics::TypeID();
        }

        static const char* GetName() {
            return SceneNodeComponentDescriptor_Graphics::Name();
        }



    private:

        /// A pointer to the graphics world where the graphics object will be created from.
        GraphicsWorld* m_pGraphicsWorld;

        /// A handle to the mesh object to draw.
        HGraphicsObject m_hMeshObject;
    };
}

#endif
