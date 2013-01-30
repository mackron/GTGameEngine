
#ifndef __GTEngine_SceneCullingManager_hpp_
#define __GTEngine_SceneCullingManager_hpp_

#include "SceneObject.hpp"

namespace GTEngine
{
    /// Base class for managing culling in a scene.
    ///
    /// This class can allow an application to do custom culling. This class is useful for applications with unique requirements
    /// that can take advantage of specific optimizations or whatnot.
    class SceneCullingManager
    {
    public:

        ////////////////////////////////////////////////////
        // Callback Structures.


        struct VisibilityCallback
        {
            /// Constructor.
            VisibilityCallback() {};

            /// Destructor.
            virtual ~VisibilityCallback() {};


            /// Called when the model of the given object is processed by ProcessVisibleObjects().
            ///
            /// @param object [in] The object being processed.
            virtual void ProcessObjectModel(SceneObject &object) = 0;

            /// Called when the point light of the given object is processed by ProcessVisibleObjects().
            ///
            /// @param object [in] The object being processed.
            virtual void ProcessObjectPointLight(SceneObject &object) = 0;

            /// Called when the spot light of the given object is processed by ProcessVisibleObjects().
            ///
            /// @param object [in] The object being processed.
            virtual void ProcessObjectSpotLight(SceneObject &object) = 0;


            /// Called when the ambient light of the given object is processed by ProcessVisibleObjects().
            ///
            /// @param object [in] The object being processed.
            virtual void ProcessObjectAmbientLight(SceneObject &object) = 0;

            /// Called when the directional light of the given object is processed by ProcessVisibleObjects().
            ///
            /// @param object [in] The object being processed.
            virtual void ProcessObjectDirectionalLight(SceneObject &object) = 0;
        };


    public:

        /// Constructor.
        SceneCullingManager()
            : flags(0)
        {
        }

        /// Destructor.
        virtual ~SceneCullingManager() {}


#if 0
        /// Adds an object to the manager.
        ///
        /// @param object [in] A reference to the object.
        ///
        /// @remarks
        ///     This can include occluder objects.
        virtual void AddObject(SceneObject &object) = 0;

        /// Removes an object from the manager.
        ///
        /// @param object [in] A reference to the object to remove.
        ///
        /// @remarks
        ///     It is possible that this will be call on a object that was not necessarily added with AddObject(). This case needs to be handled safely.
        virtual void RemoveObject(SceneObject &object) = 0;
#endif


        /// Adds a model.
        ///
        /// @param object [in] A reference to the model object.
        ///
        /// @remarks
        ///     This can be a scene node with a ModelComponent.
        virtual void AddModel(SceneObject &object) = 0;

        /// Removes a model.
        ///
        /// @param object [in] A reference to the model object.
        ///
        /// @remarks
        ///     This can be a scene node with a ModelComponent.
        virtual void RemoveModel(SceneObject &object) = 0;


        /// Adds a point light.
        ///
        /// @param object [in] A reference to the point light object.
        ///
        /// @remarks
        ///     This can be a scene node with a PointLightComponent.
        virtual void AddPointLight(SceneObject &object) = 0;

        /// Removes a point light.
        ///
        /// @param object [in] A reference to the point light object.
        ///
        /// @remarks
        ///     This can be a scene node with a PointLightComponent.
        virtual void RemovePointLight(SceneObject &object) = 0;


        /// Adds a spot light.
        ///
        /// @param object [in] A reference to the spot light object.
        ///
        /// @remarks
        ///     This can be a scene node with a SpotLightComponent.
        virtual void AddSpotLight(SceneObject &object) = 0;

        /// Removes a spot light.
        ///
        /// @param object [in] A reference to the spot light object.
        ///
        /// @remarks
        ///     This can be a scene node with a SpotLightComponent.
        virtual void RemoveSpotLight(SceneObject &object) = 0;


        /// Adds a directional light.
        ///
        /// @param object [in] A reference to the directional light object.
        ///
        /// @remarks
        ///     This can be a scene node with a DirectionalLightComponent.
        virtual void AddDirectionalLight(SceneObject &object) = 0;

        /// Removes a directional light.
        ///
        /// @param object [in] A reference to the directional light object.
        ///
        /// @remarks
        ///     This can be a scene node with a DirectionalLightComponent.
        virtual void RemoveDirectionalLight(SceneObject &object) = 0;


        /// Adds an ambient light.
        ///
        /// @param object [in] A reference to the ambient light object.
        ///
        /// @remarks
        ///     This can be a scene node with an AmbientLightComponent.
        virtual void AddAmbientLight(SceneObject &object) = 0;

        /// Removes an ambient light.
        ///
        /// @param object [in] A reference to the ambient light object.
        ///
        /// @remarks
        ///     This can be a scene node with an AmbientLightComponent.
        virtual void RemoveAmbientLight(SceneObject &object) = 0;


        /// Adds an occluder.
        ///
        /// @param object [in] A reference to the occluder object.
        ///
        /// @remarks
        ///     This can be a scene node with an OccluderComponent.
        virtual void AddOccluder(SceneObject &object) = 0;

        /// Removes an occluder.
        ///
        /// @param object [in] A reference to the occluder object.
        ///
        /// @remarks
        ///     This can be a scene node with an OccluderComponent.
        virtual void RemoveOccluder(SceneObject &object) = 0;



#if 0
        /// Updates the transformation of the given object.
        ///
        /// @param object [in] A reference to the object whose transformation is being updated.
        ///
        /// @remarks
        ///     Scaling should be applied seperately with UpdateScale().
        virtual void UpdateTransform(SceneObject &object) = 0;

        /// Updates the scale of the given object.
        ///
        /// @param object [in] A reference to the object whose scale is being updated.
        ///
        /// @remarks
        ///     The scale is not applied to lights. Changes to attenuation should be used instead.
        virtual void UpdateScale(SceneObject &object) = 0;
#endif


        /// Updates the transformation of the given model object.
        ///
        /// @param object [in] A reference to the model object whose transformation is being updated.
        ///
        /// @remarks
        ///     The object can be a scene node with a ModelComponent.
        ///     @par
        ///     Scaling should be applied separately with UpdateModelScale().
        virtual void UpdateModelTransform(SceneObject &object) = 0;

        /// Updates the transformation of the given point light.
        ///
        /// @param object [in] A reference to the point light object whose transformation is being updated.
        ///
        /// @remarks
        ///     The object can be a scene node with a PointLightComponent.
        virtual void UpdatePointLightTransform(SceneObject &object) = 0;

        /// Updates the transformation of the given spot light.
        ///
        /// @param object [in] A reference to the spot light object whose transformation is being updated.
        ///
        /// @remarks
        ///     The object can be a scene node with a SpotLightComponent.
        virtual void UpdateSpotLightTransform(SceneObject &object) = 0;

        /// Updates the transformation of the given directional light.
        ///
        /// @param object [in] A reference to the directional light object whose transformation is being updated.
        ///
        /// @remarks
        ///     The object can be a scene node with a DirectionalLightComponent.
        virtual void UpdateDirectionalLightTransform(SceneObject &object) = 0;

        /// Updates the transformation of the given ambient light.
        ///
        /// @param object [in] A reference to the ambient light object whose transformation is being updated.
        ///
        /// @remarks
        ///     The object can be a scene node with an AmbientLightComponent.
        virtual void UpdateAmbientLightTransform(SceneObject &object) = 0;

        /// Updates the transformation of the occluder.
        ///
        /// @param object [in] A reference to the occluder object whose transformation is being updated.
        ///
        /// @remarks
        ///     The object can be a scene node with an OccluderComponent.
        virtual void UpdateOccluderTransform(SceneObject &object) = 0;


        /// Updates the scale of the given model object.
        ///
        /// @param object [in] A reference to the model object whose scale is being updated.
        ///
        /// @remarks
        ///     The object can be a scene node with a ModelComponent.
        virtual void UpdateModelScale(SceneObject &object) = 0;

        /// Updates the scale of the given occluder object.
        ///
        /// @param object [in] A reference to the occluder object whose scale is being updated.
        ///
        /// @remarks
        ///     The object can be a scene node with an OccluderComponent.
        virtual void UpdateOccluderScale(SceneObject &object) = 0;



        /// Called when an object has changed and needs it's culling information updated.
        ///
        /// @param object [in] The object whose culling properties have changed.
        ///
        /// @remarks
        ///     This is NOT called when the transform or scale has changed. Instead it is called when something like the attenuation or the model has changed.
        ///     @par
        ///     When 'object' is a SceneNode, this will NOT be called when a component has been added or removed.
        //virtual void UpdateObject(SceneObject &object) = 0;


        /// Called when the model of an object has changed and needs to be updated.
        ///
        /// @param object [in] A reference to the object whose model has just changed.
        //virtual void UpdateObjectModel(SceneObject &object) = 0;

        /// Called when the point light of an object has changed and needs to be updated.
        ///
        /// @param object [in] A reference to the object whose point light has just changed.
        //virtual void UpdateObjectPointLight(SceneObject &object) = 0;

        /// Called when the spot light of an object has changed and needs to be updated.
        ///
        /// @param object [in] A reference to the object whose spot light has just changed.
        //virtual void UpdateObjectSpotLight(SceneObject &object) = 0;




        /// Retrieves the global bounds of every object.
        ///
        /// @param aabbMin [out] A reference to the variable that will receive the minimum bound.
        /// @param aabbMax [out] A reference to the variable that will receive the maximum bound.
        virtual void GetGlobalAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const = 0;


        /// Processes the visible objects for the given viewport.
        ///
        /// @param mvp      [in] The model-view-projection matrix to cull against.
        /// @param callback [in] The callback to use when processing each visible object.
        virtual void ProcessVisibleObjects(const glm::mat4 &mvp, VisibilityCallback &callback) = 0;



        /////////////////////////////////////////
        // Flags

        /// The different flags that can be set on the manager.
        enum Flags
        {
            NoOcclusionCulling = (1 << 1),
            NoFrustumCulling   = (1 << 2),
        };

        /// Retrieves the flags of the culling manager.
        unsigned int GetFlags() const { return this->flags; }

        /// Sets the flags of the culling manager.
        ///
        /// @param newFlags [in] The new flags to use with the manager.
        void SetFlags(unsigned int newFlags) { this->flags = newFlags; }


    private:

        /// The culling manager's flags.
        unsigned int flags;
    };
}

#endif
