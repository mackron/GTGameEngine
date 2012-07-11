
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

        struct VisibleCallback
        {
            /// Constructor.
            VisibleCallback() {};

            /// Destructor.
            virtual ~VisibleCallback() {};


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
        };


    public:

        /// Destructor.
        virtual ~SceneCullingManager() {}

        
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


        /// Called when an object has changed and needs it's culling information updated.
        ///
        /// @param object [in] The object whose culling properties have changed.
        ///
        /// @remarks
        ///     This is NOT called when the transform or scale has changed. Instead it is called when something like the attenuation or the model has changed.
        ///     @par
        ///     When 'object' is a SceneNode, this will NOT be called when a component has been added or removed.
        virtual void UpdateObject(SceneObject &object) = 0;


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
        virtual void ProcessVisibleObjects(const glm::mat4 &mvp, VisibleCallback &callback) = 0;
    };
}

#endif
