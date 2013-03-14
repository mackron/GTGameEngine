// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer2_hpp_
#define __GTEngine_DefaultSceneRenderer2_hpp_

#include "SceneRenderer.hpp"
#include "Scene.hpp"
#include "MaterialLibrary.hpp"
#include "Rendering/Renderer2.hpp"

#include <GTCore/Map.hpp>
#include <utility>

namespace GTEngine
{
    /// Structure containing information about a framebuffer that is attached to each viewport in the default scene renderer.
    ///
    /// One of these framebuffers is created for every viewport.
    struct DefaultSceneRendererFramebuffer
    {
        /// A pointer to main framebuffer object.
        Framebuffer* framebuffer;

        /// The depth/stencil buffer.
        Texture2D* depthStencilBuffer;

        /// The main colour output buffer (RGBA16F).
        Texture2D* colourOutputBuffer;


        /// The first lighting accumulation buffer. RGBA16F. Stores diffuse RGB. A is unused right now.
        Texture2D* lightingBuffer0;

        /// The second light accumulation buffer. RGBA16F. Stores specular RGB. A is unused right now.
        Texture2D* lightingBuffer1;


        /// The width of the framebuffer.
        unsigned int width;

        /// The height of the framebuffer.
        unsigned int height;


        /// Resizes all of the attachments on the framebuffer.
        void Resize(unsigned int newWidth, unsigned int newHeight)
        {
            this->width  = newWidth;
            this->height = newHeight;

            this->depthStencilBuffer->SetData(newWidth, newHeight, GTImage::ImageFormat_Depth24_Stencil8);
            this->colourOutputBuffer->SetData(newWidth, newHeight, GTImage::ImageFormat_RGBA16F);
            this->lightingBuffer0->SetData(   newWidth, newHeight, GTImage::ImageFormat_RGBA16F);
            this->lightingBuffer1->SetData(   newWidth, newHeight, GTImage::ImageFormat_RGBA16F);

            Renderer2::PushTexture2DData(*this->depthStencilBuffer);
            Renderer2::PushTexture2DData(*this->colourOutputBuffer);
            Renderer2::PushTexture2DData(*this->lightingBuffer0);
            Renderer2::PushTexture2DData(*this->lightingBuffer1);
        }
    };


    /// The framebuffer for shadow maps.
    struct DefaultSceneRendererShadowFramebuffer
    {
        /// A pointer to the main framebuffer object.
        Framebuffer* framebuffer;

        /// The depth/stencil buffer.
        Texture2D* depthStencilBuffer;

        /// The main colour output buffer (RG16F)
        Texture2D* colourBuffer;


        /// The width of the framebuffer.
        unsigned int width;

        /// The height of the framebuffer.
        unsigned int height;


        /// Resizes the attachments on the framebuffer.
        void Resize(unsigned int newWidth, unsigned int newHeight)
        {
            this->width  = newWidth;
            this->height = newHeight;

            this->depthStencilBuffer->SetData(newWidth, newHeight, GTImage::ImageFormat_Depth24_Stencil8);
            this->colourBuffer->SetData(      newWidth, newHeight, GTImage::ImageFormat_RG32F);

            Renderer2::PushTexture2DData(*this->depthStencilBuffer);
            Renderer2::PushTexture2DData(*this->colourBuffer);
        }
    };



    /// Structure containing the shaders associated with a material.
    struct DefaultSceneRendererMaterialShaders
    {
        DefaultSceneRendererMaterialShaders()
            : ambientLightShader(nullptr),
              directionalLightShader(nullptr),
              pointLightShader(nullptr),
              spotLightShader(nullptr),
              shadowDirectionalLightShader(nullptr),
              shadowPointLightShader(nullptr),
              shadowSpotLightShader(nullptr),
              materialShader(nullptr)
        {
        }

        ~DefaultSceneRendererMaterialShaders()
        {
            Renderer2::DeleteShader(this->ambientLightShader);
            Renderer2::DeleteShader(this->directionalLightShader);
            Renderer2::DeleteShader(this->pointLightShader);
            Renderer2::DeleteShader(this->spotLightShader);
            Renderer2::DeleteShader(this->shadowDirectionalLightShader);
            Renderer2::DeleteShader(this->shadowPointLightShader);
            Renderer2::DeleteShader(this->shadowSpotLightShader);
            Renderer2::DeleteShader(this->materialShader);
        }


        /// The shader to use when doing an ambient light pass.
        Shader* ambientLightShader;

        /// The shader to use when doing a directional light pass.
        Shader* directionalLightShader;

        /// The shader to use when doing a point light pass.
        Shader* pointLightShader;

        /// The shader to use when doing a spot light pass.
        Shader* spotLightShader;


        /// The shader to use when doing a shadow directional light pass.
        Shader* shadowDirectionalLightShader;

        /// The shader to use when doing a shadow point light pass.
        Shader* shadowPointLightShader;

        /// The shader to use when doing a shadow spot light pass.
        Shader* shadowSpotLightShader;


        /// The shader to use when doing the material pass.
        Shader* materialShader;
        
        
    private:    // No copying.
        DefaultSceneRendererMaterialShaders(const DefaultSceneRendererMaterialShaders &);
        DefaultSceneRendererMaterialShaders & operator=(const DefaultSceneRendererMaterialShaders &);
    };



    /// Callback class that will be used when querying the visible objects in a lights view frustum.
    class DefaultSceneRendererShadowObjects : public SceneCullingManager::VisibilityCallback
    {
    public:

        /// Constructor.
        DefaultSceneRendererShadowObjects();

        /// Destructor.
        ~DefaultSceneRendererShadowObjects();


        ////////////////////////////////////////////
        // Virtual Implementations.

        /// SceneCullingManager::VisibilityCallback::ProcessObjectModel().
        void ProcessObjectModel(SceneObject &object);



        //////////////////////////////////////
        // Member Variables.

        /// The list of meshes.
        GTCore::Vector<SceneRendererMesh> meshes;


    private:
        DefaultSceneRendererShadowObjects(const DefaultSceneRendererShadowObjects &);
        DefaultSceneRendererShadowObjects & operator=(const DefaultSceneRendererShadowObjects &);
    };



    /// Structure representing a shadow-casting spot light. The main difference between this and the normal one is that we have a list
    /// of meshes that need to be drawn when drawing the shadow map.
    struct DefaultSceneRendererShadowSpotLight : public SceneRendererSpotLight
    {
        /// A list of meshes to draw when building the shadow map.
        DefaultSceneRendererShadowObjects containedMeshes;

        /// The projection matrix to use when building the shadow map.
        glm::mat4 projection;

        /// The view matrix to use when building the shadow map.
        glm::mat4 view;
    };



    /// Callback class that will be used when querying the visible objects.
    class DefaultSceneRendererVisibleObjects : public SceneCullingManager::VisibilityCallback
    {
    public:

        /// Constructor.
        DefaultSceneRendererVisibleObjects(Scene &scene, SceneViewport &viewport);

        /// Destructor.
        virtual ~DefaultSceneRendererVisibleObjects();


        ////////////////////////////////////////////
        // Virtual Implementations.

        /// SceneCullingManager::VisibilityCallback::ProcessObjectModel().
        void ProcessObjectModel(SceneObject &object);

        /// SceneCullingManager::VisibilityCallback::ProcessObjectPointLight().
        void ProcessObjectPointLight(SceneObject &object);

        /// SceneCullingManager::VisibilityCallback::ProcessObjectSpotLight().
        void ProcessObjectSpotLight(SceneObject &object);

        /// SceneCullingManager::VisibilityCallback::ProcessObjectAmbientLight().
        void ProcessObjectAmbientLight(SceneObject &object);

        /// SceneCullingManager::VisibilityCallback::ProcessObjectDirectionalLight().
        void ProcessObjectDirectionalLight(SceneObject &object);



        /// Adds the given mesh.
        void AddMesh(Mesh &mesh, const glm::mat4 &transform);
        void AddMesh(const SceneRendererMesh &mesh);

        /// Performs an optimization step that arranges everything in a way where the renderer can be a bit more efficient.
        void PostProcess();



        //////////////////////////////////////
        // Member Variables.

        /// A reference to the scene this container is associated with.
        Scene &scene;


        /// The list of opaque mesh objects, sorted by material definition.
        GTCore::Map<const MaterialDefinition*, GTCore::Vector<SceneRendererMesh>*> opaqueObjects;

        /// The list of alpha-transparent objects, sorted by material definition. This needs to be separate from refractive transparent objects.
        GTCore::Map<const MaterialDefinition*, GTCore::Vector<SceneRendererMesh>*> alphaTransparentObjects;

        /// The list of refractive-transparent objects, sorted by material definition.
        GTCore::Map<const MaterialDefinition*, GTCore::Vector<SceneRendererMesh>*> refractiveTransparentObjects;


        /// The list of opaque mesh objects that should be drawn last.
        GTCore::Map<const MaterialDefinition*, GTCore::Vector<SceneRendererMesh>*> opaqueObjectsLast;

        /// The list of blended-transparent objects that should be drawn last.
        GTCore::Map<const MaterialDefinition*, GTCore::Vector<SceneRendererMesh>*> alphaTransparentObjectsLast;

        /// The list of refractive-transparent objects that should be drawn last.
        GTCore::Map<const MaterialDefinition*, GTCore::Vector<SceneRendererMesh>*> refractiveTransparentObjectsLast;




        /// The list of ambient lights.
        GTCore::Vector<SceneRendererAmbientLight*> ambientLights;

        /// The list of directional lights.
        GTCore::Vector<SceneRendererDirectionalLight*> directionalLights;

        /// The list of point lights.
        GTCore::Vector<SceneRendererPointLight*> pointLights;

        /// The list of spot lights.
        GTCore::Vector<SceneRendererSpotLight*> spotLights;


        /// The list of shadow-casting directional lights.

        /// The list of shadow-casting point lights.

        /// The list of shadow-casting spot lights.
        GTCore::Vector<DefaultSceneRendererShadowSpotLight*> shadowSpotLights;



        /// The list of meshes whose skinning needs to be applied. The skinning will be applied in PostProcess(). The value is the transformation.
        GTCore::Map<Mesh*, glm::mat4> meshesToAnimate;


        /// The projection matrix.
        glm::mat4 projectionMatrix;

        /// The view matrix.
        glm::mat4 viewMatrix;

        /// The projection * view matrix.
        glm::mat4 projectionViewMatrix;
        
        
    private:    // No copying
        DefaultSceneRendererVisibleObjects(const DefaultSceneRendererVisibleObjects &);
        DefaultSceneRendererVisibleObjects & operator=(const DefaultSceneRendererVisibleObjects &);
    };


    





    /// Class representing the default scene renderer.
    class DefaultSceneRenderer2 : public SceneRenderer
    {
    public:

        /// Constructor.
        DefaultSceneRenderer2();

        /// Destructor.
        virtual ~DefaultSceneRenderer2();



        ////////////////////////////////////////////////////////////////
        // Virtual Methods.

        /// SceneRenderer::Begin().
        void Begin(Scene &scene);

        /// SceneRenderer::End().
        void End(Scene &scene);

        /// SceneRenderer::RenderViewport()
        void RenderViewport(Scene &scene, SceneViewport &viewport);

        /// SceneRenderer::AddViewport()
        void AddViewport(SceneViewport &viewport);

        /// SceneRenderer::RemoveViewport()
        void RemoveViewport(SceneViewport &viewport);

        /// SceneRenderer::OnViewportResized()
        void OnViewportResized(SceneViewport &viewport);

        /// SceneRenderer::AddExternalMesh()
        void AddExternalMesh(const SceneRendererMesh &meshToAdd);

        /// SceneRenderer::RemoveExternalMesh()
        void RemoveExternalMesh(const SceneRendererMesh &meshToRemove);



        ////////////////////////////////////////////////////////////////
        // Event Handlers from MaterialLibrary.

        /// Called when a material definition is deleted.
        void OnDeleteMaterialDefinition(MaterialDefinition &definition);




    private:

        /// Creates a new framebuffer of the given dimensions.
        ///
        /// @param width  [in] The width of the new framebuffer.
        /// @param height [in] The height of the new framebuffer.
        DefaultSceneRendererFramebuffer* CreateFramebuffer(unsigned int width, unsigned int height);

        /// Deletes the given framebuffer that was created with CreateFramebuffer().
        ///
        /// @param framebufferToDelete [in] A pointer to the framebuffer being deleted. This must have been created with CreateFramebuffer().
        void DeleteFramebuffer(DefaultSceneRendererFramebuffer* framebufferToDelete);

        /// Retrieves a pointer to the framebuffer of the given viewport.
        ///
        /// @param viewport [in] A reference to the viewport whose framebuffer is being retrieved.
        DefaultSceneRendererFramebuffer* GetViewportFramebuffer(SceneViewport &viewport);



        /////////////////////////
        // Rendering.

        /// Renders the opaque pass.
        void RenderOpaquePass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects);

        /// Renders the depth pre-pass.
        ///
        /// @remarks
        ///     This method assumes the renderer state has been set beforehand.
        void RenderDepthPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects);
        void RenderDepthPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes);

        /// Renders the opaque lighting pass.
        void RenderOpaqueLightingPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects);

        /// Renders the opaque material pass.
        void RenderOpaqueMaterialPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects);
        void RenderOpaqueMaterialPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes);

        /// Performs an ambient lighting pass in the opaque pass.
        void RenderOpaqueAmbientLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects);
        void RenderOpaqueAmbientLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes);
        
        /// Performs a directional lighting pass in the opaque pass.
        void RenderOpaqueDirectionalLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects);
        void RenderOpaqueDirectionalLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes);

        /// Performs a point lighting pass in the opaque pass.
        void RenderOpaquePointLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects);
        void RenderOpaquePointLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes);

        /// Performs a spot lighting pass in the opaque pass.
        void RenderOpaqueSpotLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects);
        void RenderOpaqueSpotLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes);


        /// Performs a shadow-casting spot lighting pass in the opaque pass.
        void RenderOpaqueShadowSpotLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer);
        void RenderOpaqueShadowSpotLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes);


        /// Renders the alpha transparency pass.
        void RenderAlphaTransparentPass(Scene &scene, DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects);

        /// Renders the refractive transparent pass.
        void RenderRefractiveTransparentPass(Scene &scene, DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects);


        /////////////////////////
        // Materials.

        /// Retrieves the shaders of the given material.
        ///
        /// @param material [in] A reference to the material whose shaders are being retrieved.
        ///
        /// @remarks
        ///     If the structure has not yet been created, it will be created here. Note, this will not create the individual shaders. This will be handled by
        ///     the method responsible for retrieving those specific shaders.
        DefaultSceneRendererMaterialShaders* GetMaterialShaders(Material &material);

        /// Retrieves the shader to use for the ambient light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialAmbientLightShader(Material &material);

        /// Retrieves the shader to use for the directional light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialDirectionalLightShader(Material &materail);

        /// Retrieves the shader to use for the point light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialPointLightShader(Material &materail);

        /// Retrieves the shader to use for the spot light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialSpotLightShader(Material &materail);


        /// Retrieves the shader to use for the shadow spot light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialShadowSpotLightShader(Material &materail);


        /// Retrieves the material shader of the given material.
        ///
        /// @param material [in] A reference to the material whose material shader is being retrieved.
        Shader* GetMaterialMaterialShader(Material &material);



    private:

        /// The framebuffers for each attached viewport. Keyed by the viewport.
        GTCore::Map<SceneViewport*, DefaultSceneRendererFramebuffer*> viewportFramebuffers;

        /// Keeps track of the material shaders that need to be deleted. Keyed by the material definition.
        GTCore::Map<MaterialDefinition*, DefaultSceneRendererMaterialShaders*> materialShadersToDelete;

        /// The shader to use with the depth pre-pass.
        Shader* depthPassShader;

        /// The list of external meshes.
        GTCore::Vector<const SceneRendererMesh*> externalMeshes;


        /// The framebuffer for drawing shadow maps.
        DefaultSceneRendererShadowFramebuffer shadowMapFramebuffer;

        /// The shader to use when building shadow maps.
        Shader* shadowMapShader;


        
        /// Material Library Event Handler.
        class MaterialLibraryEventHandler : public MaterialLibrary::EventHandler
        {
        public:

            /// Constructor.
            MaterialLibraryEventHandler(DefaultSceneRenderer2 &rendererIn)
                : renderer(rendererIn)
            {
            }


            /// MaterialLibrary::EventHandler::OnDeleteMaterialDefinition().
            void OnDeleteMaterialDefinition(MaterialDefinition &definition)
            {
                renderer.OnDeleteMaterialDefinition(definition);
            }



            /// A reference to the renderer that owns this event handler.
            DefaultSceneRenderer2 &renderer;


        private:    // No copying.
            MaterialLibraryEventHandler(const MaterialLibraryEventHandler &);
            MaterialLibraryEventHandler & operator=(const MaterialLibraryEventHandler &);

        }materialLibraryEventHandler;
        
        
    private:    // No copying.
        DefaultSceneRenderer2(const DefaultSceneRenderer2 &);
        DefaultSceneRenderer2 & operator=(const DefaultSceneRenderer2 &);
    };
}

#endif
