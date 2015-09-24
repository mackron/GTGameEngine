// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GraphicsComponent
#define GT_GraphicsComponent

#include "SceneNodeComponent.hpp"
#include "../Graphics/GraphicsAssetResourceManager.hpp"
#include "../Graphics/GraphicsModelObject.hpp"

namespace GT
{
    class GraphicsComponentDescriptor : public SceneNodeComponentDescriptor
    {
    public:

        /// Retrieves the type ID of the component.
        static SceneNodeComponentTypeID TypeID();

        /// Retrieves the name of the component.
        static const char* Name();



    public:

        /// Constructor.
        GraphicsComponentDescriptor();

        /// Destructor.
        virtual ~GraphicsComponentDescriptor();


        /// Retrieves the unique ID of the component.
        virtual SceneNodeComponentTypeID GetTypeID() const;

        /// Retrieves the name of the component.
        virtual const char* GetName() const;

        /// @copydoc SceneNodeComponentDescriptor::CreateComponent()
        virtual SceneNodeComponent* CreateComponent() const;

        /// @copydoc SceneNodeComponentDescriptor::DeleteComponent()
        virtual void DeleteComponent(SceneNodeComponent* pComponent) const;
    };




    class GraphicsComponent : public SceneNodeComponent
    {
    public:

        /// Constructor.
        GraphicsComponent(const GraphicsComponentDescriptor &descriptor);

        /// Destructor.
        ~GraphicsComponent();


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
            return GraphicsComponentDescriptor::TypeID();
        }

        static const char* GetName() {
            return GraphicsComponentDescriptor::Name();
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
