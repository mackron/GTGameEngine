// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_SceneNodeComponent_Graphics
#define GT_SceneNodeComponent_Graphics

#include "SceneNodeComponent.hpp"
#include "SceneNodeComponentDescriptor_Graphics.hpp"
#include "../Graphics/GraphicsAssetResourceManager.hpp"
#include "../Graphics/GraphicsModelObject.hpp"

namespace GT
{
    class EngineContext;

    class SceneNodeComponent_Graphics : public SceneNodeComponent
    {
    public:

        /// Constructor.
        SceneNodeComponent_Graphics(const SceneNodeComponentDescriptor_Graphics &descriptor);

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
        bool SetModel(const char* modelPath, GraphicsAssetResourceManager* pGraphicsResourceManager);

        /// Unsets the current model.
        void UnsetModel();


        /// Adds the model to the graphics world.
        void AddModelToGraphicsWorld(const vec4 &position, const quat &rotation, const vec4 &scale);

        /// Removes the model from the graphics world.
        void RemoveModelFromGraphicsWorld();

        /// Sets the transformation of the model in the graphics world.
        ///
        /// @param position [in] The position of the model.
        /// @param rotation [in] The rotation of the model.
        /// @param scale    [in] The scale of the model.
        void SetModelTransform(const vec4 &position, const quat &rotation, const vec4 &scale);



        ///////////////////////////////////////////
        // Statics

        static SceneNodeComponentTypeID GetTypeID() {
            return SceneNodeComponentDescriptor_Graphics::TypeID();
        }

        static const char* GetName() {
            return SceneNodeComponentDescriptor_Graphics::Name();
        }



    private:

        /// A pointer to the graphics resource manager that is managing the model's resources. This is used to load
        /// the model by SetModel().
        GraphicsAssetResourceManager* m_pGraphicsResourceManager;

        /// The model resource that was loaded by SetModel()
        GraphicsAssetResource_Model* m_pModelResource;

        /// The model object that will be added to the graphics world by AddModelToGraphicsWorld()
        GraphicsModelObject* m_pModelObject;
    };
}

#endif
