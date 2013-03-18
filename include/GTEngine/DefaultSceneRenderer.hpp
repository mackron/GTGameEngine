// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_hpp_
#define __GTEngine_DefaultSceneRenderer_hpp_

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

        /// The main colour buffer (RGBA16F). This is the HDR buffer.
        Texture2D* opaqueColourBuffer;

        /// The final HDR colour buffer. RGBA16F.
        Texture2D* finalColourBufferHDR;

        


        /// The first lighting accumulation buffer. RGBA16F. Stores diffuse RGB. A is unused right now.
        Texture2D* lightingBuffer0;

        /// The second light accumulation buffer. RGBA16F. Stores specular RGB. A is unused right now.
        Texture2D* lightingBuffer1;


        /// The final colour output buffer. This is the LDR buffer that will have the final image applied. RGBA8.
        Texture2D* finalColourBuffer;


        /// A pointer to the framebuffer object to use for bloom. We use a separate object so we can do a half-sized bloom map for efficiency.
        Framebuffer* bloomFramebuffer;

        /// The bloom colour buffer. RGBA16F.
        Texture2D* bloomBuffer;


        /// The width of the framebuffer.
        unsigned int width;

        /// The height of the framebuffer.
        unsigned int height;



        /// Constructor
        DefaultSceneRendererFramebuffer(unsigned int widthIn, unsigned int heightIn)
            : framebuffer(nullptr),
              depthStencilBuffer(nullptr), opaqueColourBuffer(nullptr), lightingBuffer0(nullptr), lightingBuffer1(nullptr),
              finalColourBufferHDR(nullptr), finalColourBuffer(nullptr),
              bloomFramebuffer(nullptr), bloomBuffer(nullptr),
              width(widthIn), height(heightIn)
        {
            this->framebuffer          = Renderer2::CreateFramebuffer();
            this->depthStencilBuffer   = Renderer2::CreateTexture2D();
            this->opaqueColourBuffer   = Renderer2::CreateTexture2D();
            this->lightingBuffer0      = Renderer2::CreateTexture2D();
            this->lightingBuffer1      = Renderer2::CreateTexture2D();
            this->finalColourBufferHDR = Renderer2::CreateTexture2D();
            this->finalColourBuffer    = Renderer2::CreateTexture2D();

            this->bloomFramebuffer     = Renderer2::CreateFramebuffer();
            this->bloomBuffer          = Renderer2::CreateTexture2D();

            // Sizes and formats need to be set. All we need to do is call the Resize() method.
            this->Resize(width, height);


            // Filters.
            Renderer2::SetTexture2DFilter(*this->opaqueColourBuffer,    TextureFilter_NearestNearest, TextureFilter_Nearest);
            Renderer2::SetTexture2DFilter(*this->bloomBuffer,           TextureFilter_LinearNearest,  TextureFilter_Linear);
            Renderer2::SetTexture2DFilter(*this->lightingBuffer0,       TextureFilter_Nearest,        TextureFilter_Nearest);
            Renderer2::SetTexture2DFilter(*this->lightingBuffer1,       TextureFilter_Nearest,        TextureFilter_Nearest);
            Renderer2::SetTexture2DFilter(*this->finalColourBufferHDR,  TextureFilter_NearestNearest, TextureFilter_Nearest);
            Renderer2::SetTexture2DFilter(*this->finalColourBuffer,     TextureFilter_Nearest,        TextureFilter_Nearest);

            // Wrap Modes.
            Renderer2::SetTexture2DWrapMode(*this->opaqueColourBuffer, TextureWrapMode_ClampToEdge);
            Renderer2::SetTexture2DWrapMode(*this->bloomBuffer,        TextureWrapMode_ClampToEdge);


            // Attach to the main framebuffer.
            this->framebuffer->AttachDepthStencilBuffer(this->depthStencilBuffer);
            this->framebuffer->AttachColourBuffer(this->opaqueColourBuffer,   0);
            this->framebuffer->AttachColourBuffer(this->lightingBuffer0,      1);
            this->framebuffer->AttachColourBuffer(this->lightingBuffer1,      2);
            this->framebuffer->AttachColourBuffer(this->finalColourBufferHDR, 3);
            this->framebuffer->AttachColourBuffer(this->finalColourBuffer,    4);
            Renderer2::PushAttachments(*this->framebuffer);

            this->bloomFramebuffer->AttachColourBuffer(this->bloomBuffer, 0);
            Renderer2::PushAttachments(*this->bloomFramebuffer);
        }

        /// Destructor
        ~DefaultSceneRendererFramebuffer()
        {
            Renderer2::DeleteTexture2D(this->depthStencilBuffer);
            Renderer2::DeleteTexture2D(this->opaqueColourBuffer);
            Renderer2::DeleteTexture2D(this->bloomBuffer);
            Renderer2::DeleteTexture2D(this->lightingBuffer0);
            Renderer2::DeleteTexture2D(this->lightingBuffer1);
            Renderer2::DeleteTexture2D(this->finalColourBufferHDR);
            Renderer2::DeleteTexture2D(this->finalColourBuffer);
        
            Renderer2::DeleteFramebuffer(this->framebuffer);
            Renderer2::DeleteFramebuffer(this->bloomFramebuffer);
        }


        /// Resizes all of the attachments on the framebuffer.
        void Resize(unsigned int newWidth, unsigned int newHeight)
        {
            this->width  = newWidth;
            this->height = newHeight;

            
            this->depthStencilBuffer->SetData(  newWidth, newHeight, GTImage::ImageFormat_Depth24_Stencil8);
            this->opaqueColourBuffer->SetData(  newWidth, newHeight, GTImage::ImageFormat_RGB16F);
            this->lightingBuffer0->SetData(     newWidth, newHeight, GTImage::ImageFormat_RGB16F);
            this->lightingBuffer1->SetData(     newWidth, newHeight, GTImage::ImageFormat_RGB16F);
            this->finalColourBufferHDR->SetData(newWidth, newHeight, GTImage::ImageFormat_RGB16F);
            this->finalColourBuffer->SetData(   newWidth, newHeight, GTImage::ImageFormat_RGB8);

            Renderer2::PushTexture2DData(*this->depthStencilBuffer);
            Renderer2::PushTexture2DData(*this->opaqueColourBuffer);
            Renderer2::PushTexture2DData(*this->lightingBuffer0);
            Renderer2::PushTexture2DData(*this->lightingBuffer1);
            Renderer2::PushTexture2DData(*this->finalColourBufferHDR);
            Renderer2::PushTexture2DData(*this->finalColourBuffer);


            unsigned int bloomWidth  = GTCore::Max(1U, newWidth  / 1);
            unsigned int bloomHeight = GTCore::Max(1U, newHeight / 1);
            this->bloomBuffer->SetData(bloomWidth, bloomHeight, GTImage::ImageFormat_RGB16F);

            Renderer2::PushTexture2DData(*this->bloomBuffer);
        }
    };


    /// The framebuffer for shadow maps.
    struct DefaultSceneRendererShadowFramebuffer
    {
        /// A pointer to the main framebuffer object.
        Framebuffer* framebuffer;

        /// The depth/stencil buffer.
        Texture2D* depthStencilBuffer;

        /// The main colour output buffer (RG32F)
        Texture2D* colourBuffer;


        /// The width of the framebuffer.
        unsigned int width;

        /// The height of the framebuffer.
        unsigned int height;


        /// Constructor.
        DefaultSceneRendererShadowFramebuffer(unsigned int widthIn, unsigned int heightIn)
            : framebuffer(nullptr), depthStencilBuffer(nullptr), colourBuffer(nullptr),
              width(widthIn), height(heightIn)
        {
            this->framebuffer        = Renderer2::CreateFramebuffer();
            this->depthStencilBuffer = Renderer2::CreateTexture2D();
            this->colourBuffer       = Renderer2::CreateTexture2D();

            // We just resize to setup the texture formats and whatnot.
            this->Resize(widthIn, heightIn);

            // Now we can setup the filtering and attach the textures to the framebuffer itself.
            Renderer2::SetTexture2DFilter(  *this->colourBuffer, TextureFilter_LinearLinear, TextureFilter_Linear);
            Renderer2::SetTexture2DWrapMode(*this->colourBuffer, TextureWrapMode_ClampToEdge);

            this->framebuffer->AttachDepthStencilBuffer(this->depthStencilBuffer);
            this->framebuffer->AttachColourBuffer(this->colourBuffer, 0);

            Renderer2::PushAttachments(*this->framebuffer);
        }

        /// Destructor.
        ~DefaultSceneRendererShadowFramebuffer()
        {
            Renderer2::DeleteTexture2D(this->colourBuffer);
            Renderer2::DeleteTexture2D(this->depthStencilBuffer);
            Renderer2::DeleteFramebuffer(this->framebuffer);
        }




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

    /// The framebuffer for point light shadow maps.
    struct DefaultSceneRendererPointShadowFramebuffer
    {
        /// A pointer to the main framebuffer.
        Framebuffer* framebuffer;

        /// The depth/stencil buffer. This is cleared and reused for each face.
        Texture2D* depthStencilBuffer;

        /// The cube map that will contain the shadow information. RG32F.
        TextureCube* colourBuffer;


        /// The width of each face of the framebuffer.
        unsigned int width;

        /// The height of each face of the framebuffer.
        unsigned int height;



        /// Constructor.
        DefaultSceneRendererPointShadowFramebuffer(unsigned int widthIn, unsigned int heightIn)
            : framebuffer(nullptr), depthStencilBuffer(nullptr), colourBuffer(nullptr),
              width(widthIn), height(heightIn)
        {
            this->framebuffer        = Renderer2::CreateFramebuffer();
            this->depthStencilBuffer = Renderer2::CreateTexture2D();
            this->colourBuffer       = Renderer2::CreateTextureCube();

            // We just call Resize() to get the data setup.
            this->Resize(widthIn, heightIn);


            // Filters.
            Renderer2::SetTextureCubeFilter(*this->colourBuffer, TextureFilter_Linear, TextureFilter_Linear);
            Renderer2::SetTextureCubeWrapMode(*this->colourBuffer, TextureWrapMode_ClampToEdge);
            

            // Attach.
            this->framebuffer->AttachDepthStencilBuffer(this->depthStencilBuffer);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->PositiveX, 0);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->NegativeX, 1);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->PositiveY, 2);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->NegativeY, 3);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->PositiveZ, 4);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->NegativeZ, 5);

            Renderer2::PushAttachments(*this->framebuffer);
        }

        /// Destructor.
        ~DefaultSceneRendererPointShadowFramebuffer()
        {
            Renderer2::DeleteTextureCube(this->colourBuffer);
            Renderer2::DeleteTexture2D(this->depthStencilBuffer);
            Renderer2::DeleteFramebuffer(this->framebuffer);
        }


        /// Resizes the attachments on the framebuffer.
        void Resize(unsigned int newWidth, unsigned int newHeight)
        {
            this->width  = newWidth;
            this->height = newHeight;

            this->depthStencilBuffer->SetData(newWidth, newHeight, GTImage::ImageFormat_Depth24_Stencil8);
            this->colourBuffer->PositiveX->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->colourBuffer->NegativeX->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->colourBuffer->PositiveY->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->colourBuffer->NegativeY->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->colourBuffer->PositiveZ->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);
            this->colourBuffer->NegativeZ->SetData(newWidth, newHeight, GTImage::ImageFormat_RG32F);

            Renderer2::PushTexture2DData(*this->depthStencilBuffer, 0);
            Renderer2::PushTextureCubeData(*this->colourBuffer);
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



    /// Structure containing indices of the different kinds of lights.
    struct LightIndices
    {
        /// The indices of the ambient lights.
        GTCore::Vector<uint32_t> ambientLights;

        /// The indices of the non-shadow directional lights.
        GTCore::Vector<uint32_t> directionalLights;

        /// The indices of the non-shadow point lights.
        GTCore::Vector<uint32_t> pointLights;

        /// The indices of the non-shadow spot lights.
        GTCore::Vector<uint32_t> spotLights;


        /// The indices of the shadow-casting directional lights.
        GTCore::Vector<uint32_t> shadowDirectionalLights;

        /// The indices of the shadow-casting point lights.
        GTCore::Vector<uint32_t> shadowPointLights;

        /// The indices of the shadow-casting spot lights.
        GTCore::Vector<uint32_t> shadowSpotLights;
    };


    /// Structure representing a mesh in the default renderer.
    struct DefaultSceneRendererMesh : public SceneRendererMesh
    {
        /// The indices of the lights that are touching this mesh.
        const LightIndices* touchingLights;



        /// Constructor.
        DefaultSceneRendererMesh()
            : touchingLights(nullptr)
        {
        }
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
        void ProcessObjectModel(const SceneObject &object);



        //////////////////////////////////////
        // Member Variables.

        /// The list of meshes.
        GTCore::Vector<DefaultSceneRendererMesh> meshes;


    private:
        DefaultSceneRendererShadowObjects(const DefaultSceneRendererShadowObjects &);
        DefaultSceneRendererShadowObjects & operator=(const DefaultSceneRendererShadowObjects &);
    };




    /// Structure representing a shadow-casting directional light.
    struct DefaultSceneRendererShadowDirectionalLight : public SceneRendererDirectionalLight
    {
        /// A list of meshes to draw when building the shadow map.
        DefaultSceneRendererShadowObjects containedMeshes;

        /// The projection matrix to use when building the shadow map.
        glm::mat4 projection;

        /// The view matrix to use when building the shadow map.
        glm::mat4 view;
    };

    /// Structure representing a shadow-casting point light.
    struct DefaultSceneRendererShadowPointLight : public SceneRendererPointLight
    {
        /// The list of meshes to draw when doing the positive X face.
        DefaultSceneRendererShadowObjects containedMeshesPositiveX;

        /// The list of meshes to draw when doing the negative X face.
        DefaultSceneRendererShadowObjects containedMeshesNegativeX;

        /// The list of meshes to draw when doing the positive Y face.
        DefaultSceneRendererShadowObjects containedMeshesPositiveY;

        /// The list of meshes to draw when doing the negative Y face.
        DefaultSceneRendererShadowObjects containedMeshesNegativeY;

        /// The list of meshes to draw when doing the positive Z face.
        DefaultSceneRendererShadowObjects containedMeshesPositiveZ;

        /// The list of meshes to draw when doing the negative Z face.
        DefaultSceneRendererShadowObjects containedMeshesNegativeZ;


        /// The projection matrix.
        glm::mat4 projection;

        /// The positive X view matrix.
        glm::mat4 positiveXView;

        /// The negative X view matrix.
        glm::mat4 negativeXView;

        /// The positive Y view matrix.
        glm::mat4 positiveYView;

        /// The negative Y view matrix.
        glm::mat4 negativeYView;

        /// The positive Z view matrix.
        glm::mat4 positiveZView;

        /// The negative Z view matrix.
        glm::mat4 negativeZView;
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
        void ProcessObjectModel(const SceneObject &object);

        /// SceneCullingManager::VisibilityCallback::ProcessObjectPointLight().
        void ProcessObjectPointLight(const SceneObject &object);

        /// SceneCullingManager::VisibilityCallback::ProcessObjectSpotLight().
        void ProcessObjectSpotLight(const SceneObject &object);

        /// SceneCullingManager::VisibilityCallback::ProcessObjectAmbientLight().
        void ProcessObjectAmbientLight(const SceneObject &object);

        /// SceneCullingManager::VisibilityCallback::ProcessObjectDirectionalLight().
        void ProcessObjectDirectionalLight(const SceneObject &object);



        /// Adds the given mesh.
        void AddMesh(const Mesh &mesh, const glm::mat4 &transform, const LightIndices* lights, bool drawHighlight);     // <-- TODO: Remove 'drawHighlight' later on.
        void AddMesh(const DefaultSceneRendererMesh &mesh);

        /// Performs an optimization step that arranges everything in a way where the renderer can be a bit more efficient.
        void PostProcess();



        //////////////////////////////////////
        // Member Variables.

        /// A reference to the scene this container is associated with.
        Scene &scene;


        /// The list of opaque mesh objects, sorted by material definition.
        GTCore::Map<const MaterialDefinition*, GTCore::Vector<DefaultSceneRendererMesh>*> opaqueObjects;

        /// The list of alpha-transparent objects, sorted by material definition. This needs to be separate from refractive transparent objects.
        GTCore::Vector<DefaultSceneRendererMesh> blendedTransparentObjects;

        /// The list of refractive-transparent objects, sorted by material definition.
        GTCore::Vector<DefaultSceneRendererMesh> refractiveTransparentObjects;


        /// The list of opaque mesh objects that should be drawn last.
        GTCore::Map<const MaterialDefinition*, GTCore::Vector<DefaultSceneRendererMesh>*> opaqueObjectsLast;

        /// The list of blended-transparent objects that should be drawn last.
        GTCore::Vector<DefaultSceneRendererMesh> blendedTransparentObjectsLast;




        /// The list of ambient lights.
        GTCore::Map<const AmbientLightComponent*, SceneRendererAmbientLight*> ambientLights;

        /// The list of directional lights.
        GTCore::Map<const DirectionalLightComponent*, SceneRendererDirectionalLight*> directionalLights;

        /// The list of point lights.
        GTCore::Map<const PointLightComponent*, SceneRendererPointLight*> pointLights;

        /// The list of spot lights.
        GTCore::Map<const SpotLightComponent*, SceneRendererSpotLight*> spotLights;


        /// The list of shadow-casting directional lights.
        GTCore::Map<const DirectionalLightComponent*, DefaultSceneRendererShadowDirectionalLight*> shadowDirectionalLights;

        /// The list of shadow-casting point lights.
        GTCore::Map<const PointLightComponent*, DefaultSceneRendererShadowPointLight*> shadowPointLights;

        /// The list of shadow-casting spot lights.
        GTCore::Map<const SpotLightComponent*, DefaultSceneRendererShadowSpotLight*> shadowSpotLights;



        /// The flat list of visible models, mapped to the indices of the lights that touch them. We want to store pointers to the component and
        /// not the actual model object because we will later want access to the scene node for it's transformation.
        GTCore::Map<const ModelComponent*, LightIndices*> visibleModels;

        /// The list of meshes whose skinning needs to be applied. The skinning will be applied in PostProcess(). The value is the transformation.
        GTCore::Vector<const ModelComponent*> modelsToAnimate;


        /// The indices of every visible light.
        LightIndices allLightIndices;


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
            PointLightContactsCallback(DefaultSceneRendererVisibleObjects &ownerIn, size_t lightIndexIn, bool shadowCastingIn)
                : owner(ownerIn), lightIndex(lightIndexIn), shadowCasting(shadowCastingIn)
            {
            }


            /// SceneCullingManager::VisibilityCallback::ProcessObjectModel().
            virtual void ProcessObjectModel(const SceneObject &object)
            {
                if (object.GetType() == SceneObjectType_SceneNode)
                {
                    auto &sceneNode = static_cast<const SceneNode &>(object);
                    {
                        auto modelComponent = sceneNode.GetComponent<ModelComponent>();
                        assert(modelComponent != nullptr);
                        {
                            auto iModel = this->owner.visibleModels.Find(modelComponent);
                            if (iModel != nullptr)
                            {
                                if (!shadowCasting)
                                {
                                    iModel->value->pointLights.PushBack(static_cast<uint32_t>(this->lightIndex));
                                }
                                else
                                {
                                    iModel->value->shadowPointLights.PushBack(static_cast<uint32_t>(this->lightIndex));
                                }
                            }
                        }
                    }
                }
            }


        private:

            /// The owner of the light.
            DefaultSceneRendererVisibleObjects &owner;

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
            SpotLightContactsCallback(DefaultSceneRendererVisibleObjects &ownerIn, size_t lightIndexIn, bool shadowCastingIn)
                : owner(ownerIn), lightIndex(lightIndexIn), shadowCasting(shadowCastingIn)
            {
            }


            /// SceneCullingManager::VisibilityCallback::ProcessObjectModel().
            virtual void ProcessObjectModel(const SceneObject &object)
            {
                if (object.GetType() == SceneObjectType_SceneNode)
                {
                    auto &sceneNode = static_cast<const SceneNode &>(object);
                    {
                        auto modelComponent = sceneNode.GetComponent<ModelComponent>();
                        assert(modelComponent != nullptr);
                        {
                            auto iModel = this->owner.visibleModels.Find(modelComponent);
                            if (iModel != nullptr)
                            {
                                if (!shadowCasting)
                                {
                                    iModel->value->spotLights.PushBack(static_cast<uint32_t>(this->lightIndex));
                                }
                                else
                                {
                                    iModel->value->shadowSpotLights.PushBack(static_cast<uint32_t>(this->lightIndex));
                                }
                            }
                        }
                    }
                }
            }


        private:

            /// The owner of the light.
            DefaultSceneRendererVisibleObjects &owner;

            /// The index of the light.
            size_t lightIndex;

            /// Whether or not the light is a shadow casting light.
            bool shadowCasting;



        private:    // No copying.
            SpotLightContactsCallback(const SpotLightContactsCallback &);
            SpotLightContactsCallback & operator=(const SpotLightContactsCallback &);
        };


        
    private:    // No copying
        DefaultSceneRendererVisibleObjects(const DefaultSceneRendererVisibleObjects &);
        DefaultSceneRendererVisibleObjects & operator=(const DefaultSceneRendererVisibleObjects &);
    };


    





    /// Class representing the default scene renderer.
    class DefaultSceneRenderer : public SceneRenderer
    {
    public:

        /// Constructor.
        DefaultSceneRenderer();

        /// Destructor.
        virtual ~DefaultSceneRenderer();



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
        // Settings.

        /// Enables HDR.
        void EnableHDR();

        /// Disables HDR.
        void DisableHDR();

        /// Determines whether or not HDR is enabled.
        bool IsHDREnabled() const;


        /// Enables bloom.
        void EnableBloom();

        /// Disables bloom.
        void DisableBloom();

        /// Determines whether or not bloom is enabled.
        bool IsBloomEnabled() const;


        /// Sets the HDR exposure.
        void SetHDRExposure(float newExposure);

        /// Retrieves the HDR exposure.
        float GetHDRExposure() const;

        
        /// Sets the bloom factor.
        void SetBloomFactor(float newBloomFactor);

        /// Retrieves the bloom factor.
        float GetBloomFactor() const;



        ////////////////////////////////////////////////////////////////
        // Event Handlers from MaterialLibrary.

        /// Called when a material definition is deleted.
        void OnDeleteMaterialDefinition(MaterialDefinition &definition);




    private:

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
        void RenderDepthPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Renders the opaque lighting pass.
        void RenderOpaqueLightingPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects);

        /// Renders the opaque material pass.
        void RenderOpaqueMaterialPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects);
        void RenderOpaqueMaterialPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Performs an ambient lighting pass in the opaque pass.
        void RenderOpaqueAmbientLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects);
        void RenderOpaqueAmbientLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);
        
        /// Performs a directional lighting pass in the opaque pass.
        void RenderOpaqueDirectionalLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects);
        void RenderOpaqueDirectionalLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Performs a point lighting pass in the opaque pass.
        void RenderOpaquePointLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects);
        void RenderOpaquePointLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Performs a spot lighting pass in the opaque pass.
        void RenderOpaqueSpotLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects);
        void RenderOpaqueSpotLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);


        /// Performs a shadow-casting directional lighting pass in the opaque pass.
        void RenderOpaqueShadowDirectionalLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer);
        void RenderOpaqueShadowDirectionalLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Performs a shadow-casting point lighting pass in the opaque pass.
        void RenderOpaqueShadowPointLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer);
        void RenderOpaqueShadowPointLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);
        void RenderPointShapowMapFace(const DefaultSceneRendererShadowPointLight &light, const glm::mat4 &faceViewMatrix, int faceIndex, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);

        /// Performs a shadow-casting spot lighting pass in the opaque pass.
        void RenderOpaqueShadowSpotLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer);
        void RenderOpaqueShadowSpotLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes);


        /// Renders the alpha transparency pass.
        void RenderBlendedTransparentPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects);

        /// Renders the refractive transparent pass.
        void RenderRefractiveTransparentPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects);


        /// Renders the lighting of the given mesh.
        void RenderMeshLighting(const DefaultSceneRendererMesh &mesh, const DefaultSceneRendererVisibleObjects &visibleObjects); 


        /// Renders the final composition.
        void RenderFinalComposition(DefaultSceneRendererFramebuffer* framebuffer, Texture2D* sourceColourBuffer);

        /// Renders the bloom map.
        void RenderBloomMap(DefaultSceneRendererFramebuffer* framebuffer, Texture2D* sourceColourBuffer);



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


        /// Retrieves the shader to use for the shadow directional light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialShadowDirectionalLightShader(Material &materail);

        /// Retrieves the shader to use for the shadow point light pass.
        ///
        /// @param material [in] A reference to the material whose shader is being retrieved.
        Shader* GetMaterialShadowPointLightShader(Material &materail);

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

        /// The framebuffer for drawing point light shadow maps.
        DefaultSceneRendererPointShadowFramebuffer pointShadowMapFramebuffer;

        /// The shader to use when building point light shadow maps.
        Shader* pointShadowMapShader;


        /// Vertex array for drawing a full screen triangle. This is in P3T2 format. The z coordinate is at 0.
        VertexArray* fullscreenTriangleVA;

        /// The shader to use when doing the final composition.
        Shader* finalCompositionShaderHDR;
        Shader* finalCompositionShaderHDRNoBloom;
        Shader* finalCompositionShaderLDR;

        /// The shader to use when doing the bloom map.
        Shader* bloomShader;



        /// TEMP shader for doing highlights.
        Shader* highlightShader;



        ///////////////////////////////////////////////////////
        // Settings.

        /// Keeps track of whether or not HDR is enabled.
        bool isHDREnabled;

        /// Keeps track of whether or not bloom is enabled.
        bool isBloomEnabled;

        /// Keeps track of the HDR exposure.
        float hdrExposure;

        /// Keeps track of the bloom factor.
        float bloomFactor;


        
        /// Material Library Event Handler.
        class MaterialLibraryEventHandler : public MaterialLibrary::EventHandler
        {
        public:

            /// Constructor.
            MaterialLibraryEventHandler(DefaultSceneRenderer &rendererIn)
                : renderer(rendererIn)
            {
            }


            /// MaterialLibrary::EventHandler::OnDeleteMaterialDefinition().
            void OnDeleteMaterialDefinition(MaterialDefinition &definition)
            {
                renderer.OnDeleteMaterialDefinition(definition);
            }



            /// A reference to the renderer that owns this event handler.
            DefaultSceneRenderer &renderer;


        private:    // No copying.
            MaterialLibraryEventHandler(const MaterialLibraryEventHandler &);
            MaterialLibraryEventHandler & operator=(const MaterialLibraryEventHandler &);

        }materialLibraryEventHandler;
        
        
    private:    // No copying.
        DefaultSceneRenderer(const DefaultSceneRenderer &);
        DefaultSceneRenderer & operator=(const DefaultSceneRenderer &);
    };
}

#endif
