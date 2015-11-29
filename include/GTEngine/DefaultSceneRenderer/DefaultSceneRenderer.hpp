// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_DefaultSceneRenderer_hpp_
#define __GTEngine_DefaultSceneRenderer_hpp_

#include "../SceneRenderer.hpp"
#include "../Scene.hpp"
#include "../MaterialLibrary.hpp"
#include "../Rendering/Renderer.hpp"

#include "DefaultSceneRenderer_VisibilityProcessor.hpp"
#include "DefaultSceneRenderer_MaterialShaders.hpp"
#include "DefaultSceneRenderer_ShaderBuilder.hpp"
#include "DefaultSceneRenderer_LuminanceChain.hpp"
#include "DefaultSceneRenderer_MaterialUniformNameCache.hpp"


#include <GTEngine/Core/Map.hpp>
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
        Texture2D* colourBuffer0;

        /// The final HDR colour buffer. RGBA16F.
        Texture2D* colourBuffer1;




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

        /// The intermediary buffer to use for blurring the bloom buffer.
        Texture2D* bloomBlurBuffer;


        /// The width of the framebuffer.
        unsigned int width;

        /// The height of the framebuffer.
        unsigned int height;


        static const int ColourBuffer0Index    = 0;
        static const int ColourBuffer1Index    = 1;
        static const int LightingBuffer0Index  = 2;
        static const int LightingBuffer1Index  = 3;
        static const int FinalColourIndex      = 4;


        /// Constructor
        DefaultSceneRendererFramebuffer(unsigned int widthIn, unsigned int heightIn)
            : framebuffer(nullptr),
              depthStencilBuffer(nullptr), colourBuffer0(nullptr), colourBuffer1(nullptr),
              lightingBuffer0(nullptr), lightingBuffer1(nullptr),
              finalColourBuffer(nullptr),
              bloomFramebuffer(nullptr), bloomBuffer(nullptr), bloomBlurBuffer(nullptr),
              width(widthIn), height(heightIn)
        {
            this->framebuffer             = Renderer::CreateFramebuffer();
            this->depthStencilBuffer      = Renderer::CreateTexture2D();
            this->colourBuffer0           = Renderer::CreateTexture2D();
            this->colourBuffer1           = Renderer::CreateTexture2D();
            this->lightingBuffer0         = Renderer::CreateTexture2D();
            this->lightingBuffer1         = Renderer::CreateTexture2D();
            this->finalColourBuffer       = Renderer::CreateTexture2D();

            this->bloomFramebuffer        = Renderer::CreateFramebuffer();
            this->bloomBuffer             = Renderer::CreateTexture2D();
            this->bloomBlurBuffer         = Renderer::CreateTexture2D();


            // Sizes and formats need to be set. All we need to do is call the Resize() method.
            this->Resize(width, height);


            // Filters.
            Renderer::SetTexture2DFilter(*this->colourBuffer0,     TextureFilter_Nearest, TextureFilter_Nearest);
            Renderer::SetTexture2DFilter(*this->colourBuffer1,     TextureFilter_Nearest, TextureFilter_Nearest);
            Renderer::SetTexture2DFilter(*this->lightingBuffer0,   TextureFilter_Nearest, TextureFilter_Nearest);
            Renderer::SetTexture2DFilter(*this->lightingBuffer1,   TextureFilter_Nearest, TextureFilter_Nearest);
            Renderer::SetTexture2DFilter(*this->finalColourBuffer, TextureFilter_Nearest, TextureFilter_Nearest);
            Renderer::SetTexture2DFilter(*this->bloomBuffer,       TextureFilter_Linear,  TextureFilter_Linear);
            Renderer::SetTexture2DFilter(*this->bloomBlurBuffer,   TextureFilter_Nearest, TextureFilter_Nearest);

            // Wrap Modes.
            Renderer::SetTexture2DWrapMode(*this->colourBuffer0,   TextureWrapMode_ClampToEdge);
            Renderer::SetTexture2DWrapMode(*this->colourBuffer1,   TextureWrapMode_ClampToEdge);
            Renderer::SetTexture2DWrapMode(*this->bloomBuffer,     TextureWrapMode_ClampToEdge);
            Renderer::SetTexture2DWrapMode(*this->bloomBlurBuffer, TextureWrapMode_ClampToEdge);


            // Attach to the main framebuffer.
            this->framebuffer->AttachDepthStencilBuffer(this->depthStencilBuffer);
            //this->framebuffer->AttachWriteOnlyDepthStencilBuffer(0, 0);
            this->framebuffer->AttachColourBuffer(this->colourBuffer0,        ColourBuffer0Index);
            this->framebuffer->AttachColourBuffer(this->colourBuffer1,        ColourBuffer1Index);
            this->framebuffer->AttachColourBuffer(this->lightingBuffer0,      LightingBuffer0Index);
            this->framebuffer->AttachColourBuffer(this->lightingBuffer1,      LightingBuffer1Index);
            this->framebuffer->AttachColourBuffer(this->finalColourBuffer,    FinalColourIndex);
            Renderer::PushAttachments(*this->framebuffer);

            this->bloomFramebuffer->AttachColourBuffer(this->bloomBuffer,     0);
            this->bloomFramebuffer->AttachColourBuffer(this->bloomBlurBuffer, 1);
            Renderer::PushAttachments(*this->bloomFramebuffer);
        }

        /// Destructor
        ~DefaultSceneRendererFramebuffer()
        {
            Renderer::DeleteTexture2D(this->depthStencilBuffer);
            Renderer::DeleteTexture2D(this->colourBuffer0);
            Renderer::DeleteTexture2D(this->colourBuffer1);
            Renderer::DeleteTexture2D(this->lightingBuffer0);
            Renderer::DeleteTexture2D(this->lightingBuffer1);
            Renderer::DeleteTexture2D(this->finalColourBuffer);
            Renderer::DeleteFramebuffer(this->framebuffer);

            Renderer::DeleteTexture2D(this->bloomBlurBuffer);
            Renderer::DeleteTexture2D(this->bloomBuffer);
            Renderer::DeleteFramebuffer(this->bloomFramebuffer);
        }


        /// Resizes all of the attachments on the framebuffer.
        void Resize(unsigned int newWidth, unsigned int newHeight)
        {
            this->width  = newWidth;
            this->height = newHeight;


            this->depthStencilBuffer->SetData(newWidth, newHeight, GTLib::ImageFormat_Depth24_Stencil8);
            //this->framebuffer->ResizeWriteOnlyDepthStencilBuffer(newWidth, newHeight);
            this->colourBuffer0->SetData(    newWidth, newHeight, GTLib::ImageFormat_RGB16F);
            this->colourBuffer1->SetData(    newWidth, newHeight, GTLib::ImageFormat_RGB16F);
            this->lightingBuffer0->SetData(  newWidth, newHeight, GTLib::ImageFormat_RGB16F);
            this->lightingBuffer1->SetData(  newWidth, newHeight, GTLib::ImageFormat_RGB16F);
            this->finalColourBuffer->SetData(newWidth, newHeight, GTLib::ImageFormat_RGB8);

            Renderer::PushTexture2DData(*this->depthStencilBuffer);
            Renderer::PushTexture2DData(*this->colourBuffer0);
            Renderer::PushTexture2DData(*this->colourBuffer1);
            Renderer::PushTexture2DData(*this->lightingBuffer0);
            Renderer::PushTexture2DData(*this->lightingBuffer1);
            Renderer::PushTexture2DData(*this->finalColourBuffer);


            unsigned int bloomWidth  = GTLib::Max(1U, newWidth  / 4);
            unsigned int bloomHeight = GTLib::Max(1U, newHeight / 4);
            this->bloomBuffer->SetData(bloomWidth, bloomHeight, GTLib::ImageFormat_RGB8);
            this->bloomBlurBuffer->SetData(bloomWidth, bloomHeight, GTLib::ImageFormat_RGB8);

            Renderer::PushTexture2DData(*this->bloomBuffer);
            Renderer::PushTexture2DData(*this->bloomBlurBuffer);

            // For the write-only depth/stencil buffer. Might need to implement something more optimized here.
            Renderer::PushAttachments(*this->framebuffer);
        }


    private:    // No copying.
        DefaultSceneRendererFramebuffer(const DefaultSceneRendererFramebuffer &);
        DefaultSceneRendererFramebuffer & operator=(const DefaultSceneRendererFramebuffer &);
    };


    /// The framebuffer for shadow maps.
    struct DefaultSceneRendererShadowFramebuffer
    {
        /// A pointer to the main framebuffer object.
        Framebuffer* framebuffer;

        /// The depth/stencil buffer.
        //Texture2D* depthStencilBuffer;

        /// The main colour output buffer (RG32F)
        Texture2D* colourBuffer;

        /// The buffer for helping perform the blurring step.
        Texture2D* blurBuffer;


        /// The width of the framebuffer.
        unsigned int width;

        /// The height of the framebuffer.
        unsigned int height;


        /// Constructor.
        DefaultSceneRendererShadowFramebuffer(unsigned int widthIn, unsigned int heightIn)
            : framebuffer(nullptr), /*depthStencilBuffer(nullptr),*/ colourBuffer(nullptr), blurBuffer(nullptr),
              width(widthIn), height(heightIn)
        {
            this->framebuffer        = Renderer::CreateFramebuffer();
            //this->depthStencilBuffer = Renderer::CreateTexture2D();
            this->colourBuffer       = Renderer::CreateTexture2D();
            this->blurBuffer         = Renderer::CreateTexture2D();

            // We just resize to setup the texture formats and whatnot.
            this->Resize(widthIn, heightIn);

            // Now we can setup the filtering and attach the textures to the framebuffer itself.
            Renderer::SetTexture2DFilter(  *this->colourBuffer, TextureFilter_Linear,  TextureFilter_Linear);
            Renderer::SetTexture2DFilter(  *this->blurBuffer,   TextureFilter_Nearest, TextureFilter_Nearest);
            Renderer::SetTexture2DWrapMode(*this->colourBuffer, TextureWrapMode_ClampToEdge);
            Renderer::SetTexture2DWrapMode(*this->blurBuffer,   TextureWrapMode_ClampToEdge);

            //this->framebuffer->AttachDepthStencilBuffer(this->depthStencilBuffer);
            this->framebuffer->AttachWriteOnlyDepthStencilBuffer(0, 0);
            this->framebuffer->AttachColourBuffer(this->colourBuffer, 0);
            this->framebuffer->AttachColourBuffer(this->blurBuffer,   1);

            Renderer::PushAttachments(*this->framebuffer);
        }

        /// Destructor.
        ~DefaultSceneRendererShadowFramebuffer()
        {
            Renderer::DeleteTexture2D(this->blurBuffer);
            Renderer::DeleteTexture2D(this->colourBuffer);
            //Renderer::DeleteTexture2D(this->depthStencilBuffer);
            Renderer::DeleteFramebuffer(this->framebuffer);
        }




        /// Resizes the attachments on the framebuffer.
        void Resize(unsigned int newWidth, unsigned int newHeight)
        {
            this->width  = newWidth;
            this->height = newHeight;

            //this->depthStencilBuffer->SetData(newWidth, newHeight, GTLib::ImageFormat_Depth24_Stencil8);
            this->framebuffer->ResizeWriteOnlyDepthStencilBuffer(newWidth, newHeight);
            this->colourBuffer->SetData(      newWidth, newHeight, GTLib::ImageFormat_RG32F);
            this->blurBuffer->SetData(        newWidth, newHeight, GTLib::ImageFormat_RG32F);

            //Renderer::PushTexture2DData(*this->depthStencilBuffer);
            Renderer::PushTexture2DData(*this->colourBuffer);
            Renderer::PushTexture2DData(*this->blurBuffer);

            Renderer::PushAttachments(*this->framebuffer);
        }


    private:    // No copying.
        DefaultSceneRendererShadowFramebuffer(const DefaultSceneRendererShadowFramebuffer &);
        DefaultSceneRendererShadowFramebuffer & operator=(const DefaultSceneRendererShadowFramebuffer &);
    };

    /// The framebuffer for point light shadow maps.
    struct DefaultSceneRendererPointShadowFramebuffer
    {
        /// A pointer to the main framebuffer.
        Framebuffer* framebuffer;

        /// The depth/stencil buffer. This is cleared and reused for each face.
        //Texture2D* depthStencilBuffer;

        /// The cube map that will contain the shadow information. RG32F.
        TextureCube* colourBuffer;

        /// Buffer for performing the blur stage.
        Texture2D* blurBuffer0;
        Texture2D* blurBuffer1;


        /// The width of each face of the framebuffer.
        unsigned int width;

        /// The height of each face of the framebuffer.
        unsigned int height;



        /// Constructor.
        DefaultSceneRendererPointShadowFramebuffer(unsigned int widthIn, unsigned int heightIn)
            : framebuffer(nullptr), /*depthStencilBuffer(nullptr),*/ colourBuffer(nullptr), blurBuffer0(nullptr), blurBuffer1(nullptr),
              width(widthIn), height(heightIn)
        {
            this->framebuffer        = Renderer::CreateFramebuffer();
            //this->depthStencilBuffer = Renderer::CreateTexture2D();
            this->colourBuffer       = Renderer::CreateTextureCube();
            this->blurBuffer0        = Renderer::CreateTexture2D();
            this->blurBuffer1        = Renderer::CreateTexture2D();

            // We just call Resize() to get the data setup.
            this->Resize(widthIn, heightIn);


            // Filters.
            Renderer::SetTextureCubeFilter(*this->colourBuffer, TextureFilter_Linear,  TextureFilter_Linear);
            Renderer::SetTexture2DFilter(  *this->blurBuffer0,  TextureFilter_Nearest, TextureFilter_Nearest);
            Renderer::SetTexture2DFilter(  *this->blurBuffer1,  TextureFilter_Nearest, TextureFilter_Nearest);

            // Wrapping modes.
            Renderer::SetTextureCubeWrapMode(*this->colourBuffer, TextureWrapMode_ClampToEdge);
            Renderer::SetTexture2DWrapMode(  *this->blurBuffer0,  TextureWrapMode_ClampToEdge);
            Renderer::SetTexture2DWrapMode(  *this->blurBuffer1,  TextureWrapMode_ClampToEdge);


            // Attach.
            //this->framebuffer->AttachDepthStencilBuffer(this->depthStencilBuffer);
            this->framebuffer->AttachWriteOnlyDepthStencilBuffer(0, 0);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->PositiveX, 0);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->NegativeX, 1);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->PositiveY, 2);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->NegativeY, 3);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->PositiveZ, 4);
            this->framebuffer->AttachColourBuffer(this->colourBuffer->NegativeZ, 5);
            this->framebuffer->AttachColourBuffer(this->blurBuffer0,             6);
            this->framebuffer->AttachColourBuffer(this->blurBuffer1,             7);

            Renderer::PushAttachments(*this->framebuffer);
        }

        /// Destructor.
        ~DefaultSceneRendererPointShadowFramebuffer()
        {
            Renderer::DeleteTexture2D(this->blurBuffer0);
            Renderer::DeleteTexture2D(this->blurBuffer1);
            Renderer::DeleteTextureCube(this->colourBuffer);
            //Renderer::DeleteTexture2D(this->depthStencilBuffer);
            Renderer::DeleteFramebuffer(this->framebuffer);
        }


        /// Resizes the attachments on the framebuffer.
        void Resize(unsigned int newWidth, unsigned int newHeight)
        {
            this->width  = newWidth;
            this->height = newHeight;

            //this->depthStencilBuffer->SetData(newWidth, newHeight, GTLib::ImageFormat_Depth24_Stencil8);
            this->framebuffer->ResizeWriteOnlyDepthStencilBuffer(newWidth, newHeight);
            this->colourBuffer->PositiveX->SetData(newWidth, newHeight, GTLib::ImageFormat_RG32F);
            this->colourBuffer->NegativeX->SetData(newWidth, newHeight, GTLib::ImageFormat_RG32F);
            this->colourBuffer->PositiveY->SetData(newWidth, newHeight, GTLib::ImageFormat_RG32F);
            this->colourBuffer->NegativeY->SetData(newWidth, newHeight, GTLib::ImageFormat_RG32F);
            this->colourBuffer->PositiveZ->SetData(newWidth, newHeight, GTLib::ImageFormat_RG32F);
            this->colourBuffer->NegativeZ->SetData(newWidth, newHeight, GTLib::ImageFormat_RG32F);
            this->blurBuffer0->SetData(newWidth, newHeight, GTLib::ImageFormat_RG32F);
            this->blurBuffer1->SetData(newWidth, newHeight, GTLib::ImageFormat_RG32F);

            //Renderer::PushTexture2DData(  *this->depthStencilBuffer, 0);
            Renderer::PushTextureCubeData(*this->colourBuffer);
            Renderer::PushTexture2DData(  *this->blurBuffer0);
            Renderer::PushTexture2DData(  *this->blurBuffer1);

            Renderer::PushAttachments(*this->framebuffer);
        }


    private:    // No copying.
        DefaultSceneRendererPointShadowFramebuffer(const DefaultSceneRendererPointShadowFramebuffer &);
        DefaultSceneRendererPointShadowFramebuffer & operator=(const DefaultSceneRendererPointShadowFramebuffer &);
    };



    
    /// Class representing the default scene renderer.
    class DefaultSceneRenderer : public SceneRenderer
    {
    public:

        /// Constructor.
        DefaultSceneRenderer();

        /// Destructor.
        virtual ~DefaultSceneRenderer();


        /// Retrieves a reference to the internal cache of material names.
        ///
        /// @remarks
        ///     This should only be used internally by components of the default renderer.
        DefaultSceneRenderer_MaterialUniformNameCache & GetMaterialUniformNameCache() { return this->materialUniformNames; } 


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
        // Properties (Overridden from virtual methods from SceneRenderer)

        /// SceneRenderer::SetProperty()
        void SetProperty(const char* name, const char* value);
        void SetProperty(const char* name, int value);
        void SetProperty(const char* name, float value);
        void SetProperty(const char* name, bool value);
        void SetProperty(const char* name, const glm::vec2 &value);
        void SetProperty(const char* name, const glm::vec3 &value);
        void SetProperty(const char* name, const glm::vec4 &value);

        /// SceneRenderer::Get*Property()
        GTLib::String GetStringProperty(const char* name) const;
        int            GetIntegerProperty(const char* name) const;
        float          GetFloatProperty(const char* name) const;
        bool           GetBooleanProperty(const char* name) const;
        glm::vec2      GetVector2Property(const char* name) const;
        glm::vec3      GetVector3Property(const char* name) const;
        glm::vec4      GetVector4Property(const char* name) const;




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
        // Resource Access.

        /// Retrieves the shader to use for a depth pre-pass.
        Shader* GetDepthPassShader() const;

        /// Retrieves a directional light shadow map by it's index.
        Texture2D* GetDirectionalShadowMapByIndex(size_t index);

        /// Retrieves a point light shadow map by it's index.
        TextureCube* GetPointShadowMapByIndex(size_t index);

        /// Retrieves a spot light shadow map by it's index.
        Texture2D* GetSpotShadowMapByIndex(size_t index);

        /// Retrieves a pointer to the shader for the given light group and flags.
        ///
        /// @param material     [in] A reference to the material whose shader is being retrieved.
        /// @param lightGroupID [in] A reference to the ID of the light group.
        /// @param flags        [in] The additional flags controlling some properties of the shader.
        Shader* GetMaterialShader(Material &material, const DefaultSceneRenderer_LightGroupID &lightGroupID, uint32_t flags);

        /// Retrieves the shader for doing the selection/highlight effect.
        Shader* GetHighlightShader() const;

        /// Retrieves the shader for doing a fullscreen quad copy.
        Shader* GetFullscreenTriangleCopyShader() const;

        /// Retrieves a reference to the fullscreen triangle vertex array.
        const VertexArray & GetFullscreenTriangleVA() const;



        ////////////////////////////////////////////////////////////////
        // Rendering Helpers.

        /// Renders a directional light shadow map into the specified shadow map index.
        ///
        /// @param light          [in] A reference to the light.
        /// @param shadowMapIndex [in] The index of the shadow map to render into.
        Texture2D* RenderDirectionalLightShadowMap(DefaultSceneRendererDirectionalLight &light, size_t shadowMapIndex);

        /// Renders a point light shadow map into the specified shadow map index.
        ///
        /// @param light          [in] A reference to the light.
        /// @param shadowMapIndex [in] The index of the shadow map to render into.
        TextureCube* RenderPointLightShadowMap(DefaultSceneRendererPointLight &light, size_t shadowMapIndex);

        /// Renders a spot light shadow map into the specified shadow map index.
        ///
        /// @param light          [in] A reference to the light.
        /// @param shadowMapIndex [in] The index of the shadow map to render into.
        Texture2D* RenderSpotLightShadowMap(DefaultSceneRendererSpotLight &light, size_t shadowMapIndex);

        /// Helper function for rendering an individual face of a point light.
        ///
        /// @param light          [in] A reference to the light having it's shadow map face drawn.
        /// @param faceViewMatrix [in] The view matrix of the face being drawn.
        /// @param faceIndex      [in] The index of the face being drawn.
        /// @param meshes         [in] The list of meshes to draw.
        void RenderPointShapowMapFace(const DefaultSceneRendererPointLight &light, const glm::mat4 &faceViewMatrix, int faceIndex, const GTLib::Vector<DefaultSceneRendererMesh> &meshes);

        /// Sets the uniforms of the given material shader using the given data.
        ///
        /// @param shader         [in] A reference to the shader whose uniforms are being set.
        /// @param material       [in] A reference to the owner material.
        /// @param lightGroup     [in] A reference to the light group.
        /// @param flags          [in] The flags specifying additional properties about the contents of the shader.
        /// @param visibleObjects [in] A reference to the visibility processor containing information about the lights.
        void SetMaterialShaderUniforms(Shader &shader, const Material &material, const DefaultSceneRenderer_LightGroup &lightGroup, uint32_t flags, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects);




        ////////////////////////////////////////////////////////////////
        // Event Handlers from MaterialLibrary.

        /// Called when a material definition is deleted.
        void OnDeleteMaterialDefinition(MaterialDefinition &definition);

        /// Called when a material definition is reloaded.
        void OnReloadMaterialDefinition(MaterialDefinition &definition);




    private:

        /// Retrieves a pointer to the framebuffer of the given viewport.
        ///
        /// @param viewport [in] A reference to the viewport whose framebuffer is being retrieved.
        DefaultSceneRendererFramebuffer* GetViewportFramebuffer(SceneViewport &viewport);



        /////////////////////////
        // Rendering.

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
        DefaultSceneRenderer_MaterialShaders* GetMaterialShaders(Material &material);




    private:

        /// The framebuffers for each attached viewport. Keyed by the viewport.
        GTLib::Map<SceneViewport*, DefaultSceneRendererFramebuffer*> viewportFramebuffers;

        /// Keeps track of the shaders associated with each referenced material definition. Keyed by the material definition.
        GTLib::Map<const MaterialDefinition*, DefaultSceneRenderer_MaterialShaders*> m_materialShaders;


        /// The shader to use with the depth pre-pass.
        Shader* depthPassShader;

        /// The list of external meshes.
        GTLib::Vector<const SceneRendererMesh*> externalMeshes;


        /// The framebuffer for drawing shadow maps.
        DefaultSceneRendererShadowFramebuffer directionalShadowMapFramebuffer;

        /// The framebuffer for drawing point light shadow maps.
        DefaultSceneRendererPointShadowFramebuffer pointShadowMapFramebuffer;

        /// The framebuffer for drawing spot light shadow maps.
        DefaultSceneRendererShadowFramebuffer spotShadowMapFramebuffer;

        
        /// Vertex array for drawing a full screen triangle. This is in P3T2 format. The z coordinate is at 0.
        VertexArray* fullscreenTriangleVA;


        /// The shader to use when building directional and point shadow maps.
        Shader* shadowMapShader;

        /// The shader to use when building point light shadow maps.
        Shader* pointShadowMapShader;


        /// The shader to use when doing the final composition.
        Shader* finalCompositionShaderHDR;
        Shader* finalCompositionShaderHDRNoBloom;
        Shader* finalCompositionShaderLDR;

        /// The shader to use when doing the bloom map.
        Shader* bloomShader;

        /// Shader for doing highlights.
        Shader* highlightShader;


        /// The shader use for doing a gaussian blur.
        Shader* bloomBlurShaderX;
        Shader* bloomBlurShaderY;
        Shader* shadowBlurShaderX;
        Shader* shadowBlurShaderY;


        /// The shader builder for dynamically building shaders for various things.
        DefaultSceneRenderer_ShaderBuilder shaderBuilder;

        /// The luminance chain for computing the luminance of the scene.
        DefaultSceneRenderer_LuminanceChain luminanceChain;


        /// The cache for material uniform names so we don't need to keep generating them with sprintf().
        DefaultSceneRenderer_MaterialUniformNameCache materialUniformNames;


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

        /// Keeps track of the size of directional light shadow maps.
        unsigned int directionalShadowMapSize;

        /// Keeps track of the size of point light shadow maps.
        unsigned int pointShadowMapSize;

        /// Keeps track of the size of spot light shadow maps.
        unsigned int spotShadowMapSize;



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

            /// MaterialLibrary::EventHandler::OnReloadMaterialDefinition().
            void OnReloadMaterialDefinition(MaterialDefinition &definition)
            {
                renderer.OnReloadMaterialDefinition(definition);
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
