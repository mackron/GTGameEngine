// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_SceneCullingManager_hpp_
#define __GTEngine_SceneCullingManager_hpp_

#include "SceneNode.hpp"

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


            /// Called when the model of the given object is processed by ProcessVisibleSceneNodes().
            ///
            /// @param sceneNode [in] The object being processed.
            virtual void ProcessModel(const SceneNode &sceneNode)
            {
                (void)sceneNode;
            }

            /// Called when the point light of the given object is processed by ProcessVisibleSceneNodes().
            ///
            /// @param sceneNode [in] The object being processed.
            virtual void ProcessPointLight(const SceneNode &sceneNode)
            {
                (void)sceneNode;
            }

            /// Called when the spot light of the given object is processed by ProcessVisibleSceneNodes().
            ///
            /// @param sceneNode [in] The object being processed.
            virtual void ProcessSpotLight(const SceneNode &sceneNode)
            {
                (void)sceneNode;
            }


            /// Called when the ambient light of the given object is processed by ProcessVisibleSceneNodes().
            ///
            /// @param sceneNode [in] The object being processed.
            virtual void ProcessAmbientLight(const SceneNode &sceneNode)
            {
                (void)sceneNode;
            }

            /// Called when the directional light of the given object is processed by ProcessVisibleSceneNodes().
            ///
            /// @param sceneNode [in] The object being processed.
            virtual void ProcessDirectionalLight(const SceneNode &sceneNode)
            {
                (void)sceneNode;
            }


            /// Called when the particle system of the given scene node is processed by ProcessVisibleSceneNodes().
            ///
            /// @param sceneNode [in] The scene node being processed.
            virtual void ProcessParticleSystem(const SceneNode &sceneNode)
            {
                (void)sceneNode;
            }
        };


    public:

        /// Constructor.
        SceneCullingManager()
            : m_flags(0)
        {
        }

        /// Destructor.
        virtual ~SceneCullingManager() {}



        /// Adds a model.
        ///
        /// @param object [in] A reference to the scene node with an attached ModelComponent.
        virtual void AddModel(SceneNode &sceneNode) = 0;

        /// Removes a model.
        ///
        /// @param object [in] A reference to the scene node with an attached ModelComponent.
        virtual void RemoveModel(SceneNode &sceneNode) = 0;


        /// Adds a point light.
        ///
        /// @param object [in] A reference to the scene node with an attached PointLightComponent.
        virtual void AddPointLight(SceneNode &sceneNode) = 0;

        /// Removes a point light.
        ///
        /// @param object [in] A reference to the scene node with an attached PointLightComponent.
        virtual void RemovePointLight(SceneNode &sceneNode) = 0;


        /// Adds a spot light.
        ///
        /// @param object [in] A reference to the scene node with an attached SpotLightComponent.
        virtual void AddSpotLight(SceneNode &sceneNode) = 0;

        /// Removes a spot light.
        ///
        /// @param object [in] A reference to the scene node with an attached SpotLightComponent.
        virtual void RemoveSpotLight(SceneNode &sceneNode) = 0;


        /// Adds a directional light.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached DirectionalLightComponent.
        virtual void AddDirectionalLight(SceneNode &sceneNode) = 0;

        /// Removes a directional light.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached DirectionalLightComponent.
        virtual void RemoveDirectionalLight(SceneNode &sceneNode) = 0;


        /// Adds an ambient light.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached AmbientLightComponent.
        virtual void AddAmbientLight(SceneNode &sceneNode) = 0;

        /// Removes an ambient light.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached AmbientLightComponent.
        virtual void RemoveAmbientLight(SceneNode &sceneNode) = 0;


        /// Adds a particle system.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached ParticleSystemComponent.
        virtual void AddParticleSystem(SceneNode &sceneNode) = 0;

        /// Removes a particle system.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached ParticleSystemComponent.
        virtual void RemoveParticleSystem(SceneNode &sceneNode) = 0;



        /// Adds an occluder.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached OccluderComponent.
        virtual void AddOccluder(SceneNode &sceneNode) = 0;

        /// Removes an occluder.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached OccluderComponent to remove.
        virtual void RemoveOccluder(SceneNode &sceneNode) = 0;




        /// Updates the transformation of the given model object.
        ///
        /// @param scene node [in] A reference to the scene node with an attached ModelComponent whose transformation is being updated.
        ///
        /// @remarks
        ///     Scaling should be applied separately with UpdateModelScale().
        virtual void UpdateModelTransform(SceneNode &sceneNode) = 0;

        /// Updates the transformation of the given point light.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached PointLightComponent whose transformation is being updated.
        virtual void UpdatePointLightTransform(SceneNode &sceneNode) = 0;

        /// Updates the transformation of the given spot light.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached SpotLightComponent whose transformation is being updated.
        virtual void UpdateSpotLightTransform(SceneNode &sceneNode) = 0;

        /// Updates the transformation of the given directional light.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached DirectionalLightComponent whose transformation is being updated.
        virtual void UpdateDirectionalLightTransform(SceneNode &sceneNode) = 0;

        /// Updates the transformation of the given ambient light.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached AmbientLightComponent whose transformation is being updated.
        virtual void UpdateAmbientLightTransform(SceneNode &sceneNode) = 0;

        /// Updates the transformation of the given particle system.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached ParticleSystemComponent whose transformation is being updated.
        virtual void UpdateParticleSystemTransform(SceneNode &sceneNode) = 0;

        /// Updates the transformation of the occluder.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached OccluderComponent whose transformation is being updated.
        virtual void UpdateOccluderTransform(SceneNode &sceneNode) = 0;


        /// Updates the scale of the given model object.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached ModelComponent whose scale is being updated.
        virtual void UpdateModelScale(SceneNode &sceneNode) = 0;

        /// Updates the scale of the given occluder object.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached OccluderComponent whose scale is being updated.
        virtual void UpdateOccluderScale(SceneNode &sceneNode) = 0;


        /// Updates the culling of a model scene node based on it's AABB.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached ModelComponent whose culling object is being updated.
        virtual void UpdateModelAABB(SceneNode &sceneNode) = 0;

        /// Updates the culling of a particle system scene node based on it's AABB.
        ///
        /// @param sceneNode [in] A reference to the scene node with an attached ParticleSystemComponent whose culling object is being updated.
        virtual void UpdateParticleSystemAABB(SceneNode &sceneNode) = 0;



        /// Retrieves the global bounds of every object.
        ///
        /// @param aabbMin [out] A reference to the variable that will receive the minimum bound.
        /// @param aabbMax [out] A reference to the variable that will receive the maximum bound.
        virtual void GetGlobalAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const = 0;


        /// Processes the visible objects for the given viewport.
        ///
        /// @param mvp      [in] The model-view-projection matrix to cull against.
        /// @param callback [in] The callback to use when processing each visible object.
        virtual void ProcessVisibleSceneNodes(const glm::mat4 &mvp, VisibilityCallback &callback) const = 0;

        /// Queries the objects contained within the volume of the given point light.
        ///
        /// @param light [in] A reference to the light scene node.
        ///
        /// @remarks
        ///     It is asserted that the light has a point light component and is part of the scene.
        virtual void QueryPointLightContacts(const SceneNode &light, VisibilityCallback &callback) const = 0;

        /// Queries the objects contained within the volume of the given spot light.
        ///
        /// @param light [in] A reference to the light scene node.
        ///
        /// @remakrs
        ///     It is asserted that the light has a spot light component and is part of the scene.
        virtual void QuerySpotLightContacts(const SceneNode &light, VisibilityCallback &callback) const = 0;


        /////////////////////////////////////////
        // Flags

        /// The different flags that can be set on the manager.
        enum Flags
        {
            NoOcclusionCulling = (1 << 1),
            NoFrustumCulling   = (1 << 2),
        };

        /// Retrieves the flags of the culling manager.
        unsigned int GetFlags() const { return m_flags; }

        /// Sets the flags of the culling manager.
        ///
        /// @param newFlags [in] The new flags to use with the manager.
        void SetFlags(unsigned int newFlags) { m_flags = newFlags; }


    private:

        /// The culling manager's flags.
        unsigned int m_flags;
    };
}

#endif
