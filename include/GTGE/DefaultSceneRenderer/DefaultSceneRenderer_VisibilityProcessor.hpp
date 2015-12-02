// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_DefaultSceneRenderer_VisibilityProcessor
#define GT_DefaultSceneRenderer_VisibilityProcessor

#include "DefaultSceneRenderer_Mesh.hpp"
#include "DefaultSceneRenderer_LightManager.hpp"

namespace GT
{
    /// Callback class that will be used when querying the visible objects.
    class DefaultSceneRenderer_VisibilityProcessor : public SceneCullingManager::VisibilityCallback
    {
    public:

        /// Constructor.
        DefaultSceneRenderer_VisibilityProcessor(Scene &scene, SceneViewport &viewport);

        /// Destructor.
        virtual ~DefaultSceneRenderer_VisibilityProcessor();


        ////////////////////////////////////////////
        // Virtual Implementations.

        /// SceneCullingManager::VisibilityCallback::ProcessModel().
        void ProcessModel(const SceneNode &sceneNode);

        /// SceneCullingManager::VisibilityCallback::ProcessPointLight().
        void ProcessPointLight(const SceneNode &sceneNode);

        /// SceneCullingManager::VisibilityCallback::ProcessSpotLight().
        void ProcessSpotLight(const SceneNode &sceneNode);

        /// SceneCullingManager::VisibilityCallback::ProcessAmbientLight().
        void ProcessAmbientLight(const SceneNode &sceneNode);

        /// SceneCullingManager::VisibilityCallback::ProcessDirectionalLight().
        void ProcessDirectionalLight(const SceneNode &sceneNode);

        /// SceneCullingManager::VisibilityCallback::ProcessParticleSystem().
        void ProcessParticleSystem(const SceneNode &sceneNode);



        /// Adds the given mesh.
        void AddMesh(const Mesh &mesh, const glm::mat4 &transform, const DefaultSceneRenderer_LightGroup* lights, bool drawHighlight, const glm::vec3 &highlightColour);
        void AddMesh(const DefaultSceneRendererMesh &mesh);

        /// Performs an optimization step that arranges everything in a way where the renderer can be a bit more efficient.
        void PostProcess();



        //////////////////////////////////////
        // Member Variables.

        /// A reference to the scene this container is associated with.
        Scene &scene;


        /// The list of opaque mesh objects, sorted by material definition.
        Map<const MaterialDefinition*, Vector<DefaultSceneRendererMesh>*> opaqueObjects;

        /// The list of transparent objects. This includes both blended and refracted transparent objects.
        Vector<DefaultSceneRendererMesh> transparentObjects;


        /// The list of opaque mesh objects that should be drawn last.
        Map<const MaterialDefinition*, Vector<DefaultSceneRendererMesh>*> opaqueObjectsLast;

        /// The list of transparent objects that should be drawn last.
        Vector<DefaultSceneRendererMesh> transparentObjectsLast;


        /// The light manager for managing lights.
        DefaultSceneRenderer_LightManager lightManager;



        /// A map structure for mapping a model's Mesh to a scene renderer mesh.
        Map<const Mesh*, DefaultSceneRendererMesh*> visibleMeshes;


        /// The flat list of visible models, mapped to the indices of the lights that touch them. We want to store pointers to the component and
        /// not the actual model object because we will later want access to the scene node for it's transformation.
        Map<const ModelComponent*, DefaultSceneRenderer_LightGroup*> visibleModels;

        /// The list of meshes whose skinning needs to be applied. The skinning will be applied in PostProcess().
        Vector<const ModelComponent*> modelsToAnimate;


        /// The flat list of visible particle systems, mapped to the indices of the lights that touch them.
        Map<const ParticleSystemComponent*, DefaultSceneRenderer_LightGroup*> visibleParticleSystems;



        /// The indices of every visible light.
        DefaultSceneRenderer_LightGroup allLights;


        /// The projection matrix.
        glm::mat4 projectionMatrix;

        /// The view matrix.
        glm::mat4 viewMatrix;

        /// The projection * view matrix.
        glm::mat4 projectionViewMatrix;



    private:

        /// Callback for point light containment queries.
        class PointLightContactsCallback : public SceneCullingManager::VisibilityCallback
        {
        public:

            /// Constructor.
            PointLightContactsCallback(DefaultSceneRenderer_VisibilityProcessor &ownerIn, size_t lightIndexIn, bool shadowCastingIn)
                : owner(ownerIn), lightIndex(lightIndexIn), shadowCasting(shadowCastingIn)
            {
            }


            /// SceneCullingManager::VisibilityCallback::ProcessModel().
            virtual void ProcessModel(const SceneNode &sceneNode)
            {
                auto modelComponent = sceneNode.GetComponent<ModelComponent>();
                assert(modelComponent != nullptr);
                {
                    auto iModel = this->owner.visibleModels.Find(modelComponent);
                    if (iModel != nullptr)
                    {
                        if (!this->shadowCasting)
                        {
                            iModel->value->AddPointLight(static_cast<uint32_t>(this->lightIndex));
                        }
                        else
                        {
                            iModel->value->AddShadowPointLight(static_cast<uint32_t>(this->lightIndex));
                        }
                    }
                }
            }

            /// SceneCullingManager::VisibilityCallback::ProcessParticleSystem().
            virtual void ProcessParticleSystem(const SceneNode &sceneNode)
            {
                auto particleSystemComponent = sceneNode.GetComponent<ParticleSystemComponent>();
                assert(particleSystemComponent != nullptr);
                {
                    auto iParticleSystemComponent = this->owner.visibleParticleSystems.Find(particleSystemComponent);
                    if (iParticleSystemComponent != nullptr)
                    {
                        if (!this->shadowCasting)
                        {
                            iParticleSystemComponent->value->AddPointLight(static_cast<uint32_t>(this->lightIndex));
                        }
                        else
                        {
                            iParticleSystemComponent->value->AddShadowPointLight(static_cast<uint32_t>(this->lightIndex));
                        }
                    }
                }
            }


        private:

            /// The owner of the light.
            DefaultSceneRenderer_VisibilityProcessor &owner;

            /// The index of the light.
            size_t lightIndex;

            /// Whether or not the light is a shadow casting light.
            bool shadowCasting;



        private:    // No copying.
            PointLightContactsCallback(const PointLightContactsCallback &);
            PointLightContactsCallback & operator=(const PointLightContactsCallback &);
        };



        /// Callback for spot light containment queries.
        class SpotLightContactsCallback : public SceneCullingManager::VisibilityCallback
        {
        public:

            /// Constructor.
            SpotLightContactsCallback(DefaultSceneRenderer_VisibilityProcessor &ownerIn, size_t lightIndexIn, bool shadowCastingIn)
                : owner(ownerIn), lightIndex(lightIndexIn), shadowCasting(shadowCastingIn)
            {
            }


            /// SceneCullingManager::VisibilityCallback::ProcessModel().
            virtual void ProcessModel(const SceneNode &sceneNode)
            {
                auto modelComponent = sceneNode.GetComponent<ModelComponent>();
                assert(modelComponent != nullptr);
                {
                    auto iModel = this->owner.visibleModels.Find(modelComponent);
                    if (iModel != nullptr)
                    {
                        if (!this->shadowCasting)
                        {
                            iModel->value->AddSpotLight(static_cast<uint32_t>(this->lightIndex));
                        }
                        else
                        {
                            iModel->value->AddShadowSpotLight(static_cast<uint32_t>(this->lightIndex));
                        }
                    }
                }
            }

            /// SceneCullingManager::VisibilityCallback::ProcessParticleSystem().
            virtual void ProcessParticleSystem(const SceneNode &sceneNode)
            {
                auto particleSystemComponent = sceneNode.GetComponent<ParticleSystemComponent>();
                assert(particleSystemComponent != nullptr);
                {
                    auto iParticleSystemComponent = this->owner.visibleParticleSystems.Find(particleSystemComponent);
                    if (iParticleSystemComponent != nullptr)
                    {
                        if (!this->shadowCasting)
                        {
                            iParticleSystemComponent->value->AddSpotLight(static_cast<uint32_t>(this->lightIndex));
                        }
                        else
                        {
                            iParticleSystemComponent->value->AddShadowSpotLight(static_cast<uint32_t>(this->lightIndex));
                        }
                    }
                }
            }


        private:

            /// The owner of the light.
            DefaultSceneRenderer_VisibilityProcessor &owner;

            /// The index of the light.
            size_t lightIndex;

            /// Whether or not the light is a shadow casting light.
            bool shadowCasting;



        private:    // No copying.
            SpotLightContactsCallback(const SpotLightContactsCallback &);
            SpotLightContactsCallback & operator=(const SpotLightContactsCallback &);
        };



    private:    // No copying
        DefaultSceneRenderer_VisibilityProcessor(const DefaultSceneRenderer_VisibilityProcessor &);
        DefaultSceneRenderer_VisibilityProcessor & operator=(const DefaultSceneRenderer_VisibilityProcessor &);
    };
}

#endif