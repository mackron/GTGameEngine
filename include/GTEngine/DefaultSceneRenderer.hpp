
#ifndef __GTEngine_DefaultSceneRenderer_hpp_
#define __GTEngine_DefaultSceneRenderer_hpp_

#include "SceneRenderer.hpp"
#include "SceneObject.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Rendering/RenderCommand.hpp"
#include "Rendering/RCCache.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderCommands/RCDrawVA.hpp"
#include "Rendering/RenderCommands/RCSetFaceCulling.hpp"
#include "MaterialShaderCache.hpp"
#include <GTCore/Map.hpp>
#include <GTCore/BinarySearchTree.hpp>
#include <GTImage/Utils.hpp>

class Scene;

namespace GTEngine
{
    /// The default scene renderer.
    class DefaultSceneRenderer : public SceneRenderer
    {
    public:

        /// Structure containing metadata for materials. There should be one of these for each material.
        struct MaterialMetadata
        {
            MaterialMetadata()
                : materialPassShader(nullptr), materialPassRCs()
            {
            }

            Shader* materialPassShader;         ///< The shader for the material pass.
            RCQueue materialPassRCs;            ///< The render commands for meshes using this material in the material pass.

        private:    // No copying
            MaterialMetadata(const MaterialMetadata &);
            MaterialMetadata & operator=(const MaterialMetadata &);
        };



        /// Structure containing rendering state for each layer.
        struct LayerState
        {
            LayerState()
                : ambientLights(), directionalLights(), pointLights(), spotLights(),
                  directionalLights_NoShadows(), pointLights_NoShadows(), spotLights_NoShadows(),
                  refractiveLightingDrawRCs(), usedMaterials(), usedRefractiveMaterials(),
                  cameraProjection(), cameraView(), cameraPosition(),
                  cameraFOV(90.0f), cameraAspect(16.0f / 9.0f), cameraZNear(0.1f), cameraZFar(1000.0f),
                  viewport(nullptr)
            {
            }

            /// The list of visible ambient lights for the currently rendering viewport.
            GTCore::Vector<const SceneObject*> ambientLights;

            /// The list of visible directional lights for the currently rendering viewport.
            GTCore::Vector<const SceneObject*> directionalLights;

            /// The list of visible point lights for the currently rendering viewport.
            GTCore::Vector<const SceneObject*> pointLights;

            /// The list of visible spot lights for the currently rendering viewport.
            GTCore::Vector<const SceneObject*> spotLights;


            /// The list of directional lights that are not casting shadows.
            GTCore::Vector<const SceneObject*> directionalLights_NoShadows;

            /// The list of point lights that are not casting shadows.
            GTCore::Vector<const SceneObject*> pointLights_NoShadows;

            /// The list of spot lights that are not casting shadows.
            GTCore::Vector<const SceneObject*> spotLights_NoShadows;


            /// Every piece of visible geometry will need to be re-rendered for every light. This needs to be efficient. To do this, we cache a draw call that
            /// will draw the geometry in each lighting pass, and then append that to the renderer's back RC cache in one go every time it's needed. It will
            /// be cleared at the beginning of every frame.
            //RCQueue lightingDrawRCs;

            /// The same as 'lightingDrawRCs', except for refractive geometry.
            RCQueue refractiveLightingDrawRCs;


            /// The metadata of the material definitions being used in the current frame. This is cleared at the beginning of every frame.
            GTCore::BinarySearchTree<MaterialMetadata*> usedMaterials;

            /// Same as 'usedMaterials', only containing refractive materials.
            GTCore::BinarySearchTree<MaterialMetadata*> usedRefractiveMaterials;


            /// The camera projection.
            glm::mat4 cameraProjection;

            /// The camera view matrix.
            glm::mat4 cameraView;

            /// The camera world position.
            glm::vec3 cameraPosition;

            /// The camera FOV.
            float cameraFOV;

            /// The camera aspect ratio.
            float cameraAspect;

            /// The camera's z-near plane.
            float cameraZNear;

            /// The camera's z-far plane.
            float cameraZFar;


            /// The viewport being rendered to.
            SceneViewport* viewport;


            /// Resets the state. Should be called at the start of each render.
            void Reset()
            {
                this->ambientLights.Clear();
                this->directionalLights.Clear();
                this->pointLights.Clear();
                this->spotLights.Clear();
                this->directionalLights_NoShadows.Clear();
                this->pointLights_NoShadows.Clear();
                this->spotLights_NoShadows.Clear();

                //this->lightingDrawRCs.Clear();
                this->refractiveLightingDrawRCs.Clear();

                this->usedMaterials.Clear();
                this->usedRefractiveMaterials.Clear();
            }

            /// Determines whether or not this state has refractive geometry needing to be drawn. We use this in determining whether or not we need to do
            /// additional rendering operations to support refractions.
            bool HasRefractiveGeometry()
            {
                return !refractiveLightingDrawRCs.IsEmpty();
            }


        private:    // No copying.
            LayerState(const LayerState &);
            LayerState & operator=(const LayerState &);
        };




    public:

        /// Constructor.
        DefaultSceneRenderer();

        /// Destructor.
        virtual ~DefaultSceneRenderer();


        /// SceneRenderer::Begin().
        virtual void Begin(Scene &scene);

        /// SceneRenderer::End().
        virtual void End(Scene &scene);


        /// SceneRenderer::BeginViewport().
        virtual void RenderViewport(Scene &scene, SceneViewport &viewport);


        /// SceneRenderer::AddViewport().
        virtual void AddViewport(SceneViewport &viewport);

        /// SceneRenderer::RemoveViewport().
        virtual void RemoveViewport(SceneViewport &viewport);

        /// SceneRenderer::OnViewportResized().
        virtual void OnViewportResized(SceneViewport &viewport);



        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // Methods below should only be called internally, but need to be public for a few things.

        /// Called for a model that's visible in the currently rendering viewport.
        void __MaterialPass_Model(const SceneObject &object, LayerState &state);

        /// Called for a model that's visible in a shadow pass.
        void __ShadowPass_Model(const SceneObject &object, const glm::mat4 &projection, const glm::mat4 &view);



    // Below are data structures for use by this renderer.
    private:

        /// Class representing a framebuffer for use by this renderer. There will usually be a framebuffer for every viewport.
        class Framebuffer : public GTEngine::Framebuffer
        {
        public:

            /// Constructor.
            Framebuffer()
                : width(1), height(1),
                  finalOutput(nullptr),
                  depthStencil(nullptr),
                  lightingBuffer0(nullptr), lightingBuffer1(nullptr),
                  materialBuffer0(nullptr), materialBuffer1(nullptr), materialBuffer2(nullptr),
                  opaqueColourBuffer(nullptr), linearDepthBuffer(nullptr)
            {
                this->depthStencil = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_Depth24_Stencil8);

                if (Renderer::SupportFloatTextures())
                {
                    this->finalOutput        = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                    this->lightingBuffer0    = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                    this->lightingBuffer1    = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                    this->materialBuffer0    = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->materialBuffer1    = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->materialBuffer2    = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);       // <-- Look into using R10G10B10A2. Need to handle sign.
                    this->opaqueColourBuffer = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                    this->linearDepthBuffer  = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_R32F);
                }
                else
                {
                    this->finalOutput        = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->lightingBuffer0    = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->lightingBuffer1    = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->materialBuffer0    = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->materialBuffer1    = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->materialBuffer2    = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->opaqueColourBuffer = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->linearDepthBuffer  = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                }

                this->finalOutput->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);
                this->lightingBuffer0->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);
                this->lightingBuffer1->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);
                this->materialBuffer0->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);
                this->materialBuffer1->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);
                this->materialBuffer2->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);
                this->opaqueColourBuffer->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);
                this->linearDepthBuffer->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);

                this->finalOutput->SetWrapMode(GTEngine::TextureWrapMode_ClampToEdge);
                this->opaqueColourBuffer->SetWrapMode(GTEngine::TextureWrapMode_ClampToEdge);


                this->AttachDepthStencilBuffer(this->depthStencil);
                this->AttachColourBuffer(this->finalOutput,        0);
                this->AttachColourBuffer(this->materialBuffer0,    1);
                this->AttachColourBuffer(this->materialBuffer1,    2);
                this->AttachColourBuffer(this->materialBuffer2,    3);
                this->AttachColourBuffer(this->lightingBuffer0,    4);
                this->AttachColourBuffer(this->lightingBuffer1,    5);
                this->AttachColourBuffer(this->opaqueColourBuffer, 6);
                this->AttachColourBuffer(this->linearDepthBuffer,  7);
            }

            /// Destructor
            ~Framebuffer()
            {
                delete this->finalOutput;
                delete this->depthStencil;
                delete this->lightingBuffer0;
                delete this->lightingBuffer1;
                delete this->materialBuffer0;
                delete this->materialBuffer1;
                delete this->materialBuffer2;
                delete this->opaqueColourBuffer;
                delete this->linearDepthBuffer;
            }


            /// Marks the framebuffer for resizing.
            void Resize(unsigned int newWidth, unsigned int newHeight)
            {
                this->width  = newWidth;
                this->height = newHeight;

                this->finalOutput->Resize( this->width, this->height);

                this->depthStencil->Resize(this->width, this->height);

                this->lightingBuffer0->Resize(this->width, this->height);
                this->lightingBuffer1->Resize(this->width, this->height);

                this->materialBuffer0->Resize(this->width, this->height);
                this->materialBuffer1->Resize(this->width, this->height);
                this->materialBuffer2->Resize(this->width, this->height);

                this->opaqueColourBuffer->Resize(this->width, this->height);

                this->linearDepthBuffer->Resize(this->width, this->height);
            }


            /// Retrieves the width of framebuffer.
            unsigned int GetWidth() const { return this->width; }

            /// Retrieves the height of the framebuffer.
            unsigned int GetHeight() const { return this->height; }


            /// Retrieves a pointer to the final output colour buffer.
            Texture2D* GetFinalOutputColourBuffer() { return this->finalOutput; }


        public:

            /// The width of the framebuffer.
            unsigned int width;

            /// The height of the framebuffer.
            unsigned int height;


            /// The final output colour buffer.
            Texture2D* finalOutput;

            /// The depth/stencil attachments.
            Texture2D* depthStencil;

            // The lighting buffers.
            Texture2D* lightingBuffer0;     // RGB = Diffuse.  A = nothing.
            Texture2D* lightingBuffer1;     // RGB = Specular. A = nothing.

            // The material buffers.
            Texture2D* materialBuffer0;     // RGB = Diffuse.  A = Shininess.
            Texture2D* materialBuffer1;     // RGB = Emissive. A = Transparency.
            Texture2D* materialBuffer2;     // RGB = Normals for normal mapping. A = nothing.

            // The colour buffer that will contain the opaque geometry which will be used as the background for transparent geometry.
            Texture2D* opaqueColourBuffer;

            /// The colour buffer containing the linear depth of each pixel. This will be used when reconstructing the world and view space positions of a pixel.
            Texture2D* linearDepthBuffer;


        private:    // No copying
            Framebuffer(const Framebuffer &);
            Framebuffer & operator=(const Framebuffer &);
        };



        /// Render command for beginning the frame.
        struct RCBegin : public RenderCommand
        {
            RCBegin()
                : framebuffer(nullptr)
            {
            }


            /// RenderCommand::Execute().
            void Execute();


            /// The framebuffer to render to.
            Framebuffer* framebuffer;


        private:    // No copying.
            RCBegin(const RCBegin &);
            RCBegin & operator=(const RCBegin &);
        };


        // Render command for beginning the background.
        struct RCBeginBackground : public RenderCommand
        {
            void Execute();
        };


        // Render command for beginning the lighting pass.
        struct RCBeginLighting : public RenderCommand
        {
            RCBeginLighting()
                : framebuffer(), colourClearShader(), stencilIndex()
            {
            }

            void Execute();

            DefaultSceneRenderer::Framebuffer* framebuffer;
            Shader* colourClearShader;
            int stencilIndex;


        private:    // No copying.
            RCBeginLighting(const RCBeginLighting &);
            RCBeginLighting & operator=(const RCBeginLighting &);
        };

        // Render command for beginning transparency.
        struct RCBeginTransparency : public RenderCommand
        {
            void Execute();
        };

        // Render command for beginning the foreground transparency.
        struct RCBeginForegroundTransparency : public RenderCommand
        {
            RCBeginForegroundTransparency()
                : depthClearShader()
            {
            }

            void Execute();

            Shader* depthClearShader;


        private:    // No copying.
            RCBeginForegroundTransparency(const RCBeginForegroundTransparency &);
            RCBeginForegroundTransparency & operator=(const RCBeginForegroundTransparency &);
        };

        // Render command for beginning a transparent material pass.
        struct RCBeginTransparentMaterialPass : public RenderCommand
        {
            RCBeginTransparentMaterialPass()
                : shader(), backgroundTexture(), zFar()
            {
            }

            void Execute();

            Shader* shader;
            Texture2D* backgroundTexture;

            /// The zfar plane.
            float zFar;


        private:    // No copying.
            RCBeginTransparentMaterialPass(const RCBeginTransparentMaterialPass &);
            RCBeginTransparentMaterialPass & operator=(const RCBeginTransparentMaterialPass &);
        };




        // Render command for controlling blending.
        struct RCControlBlending : public RenderCommand
        {
            RCControlBlending()
                : enable(), sourceFactor(), destFactor()
            {
            }

            void Execute();

            /// Controls whether or not the blending should be enabled.
            bool enable;

            /// Controls the source and dest factors.
            BlendFunc sourceFactor;
            BlendFunc destFactor;
        };


        // Render command for setting a shader.
        struct RCSetShader : public RenderCommand
        {
            RCSetShader()
                : shader(), zFar()
            {
            }

            void Execute();

            /// The shader to activate.
            Shader* shader;

            /// The zfar plane.
            float zFar;


        private:    // No copying.
            RCSetShader(const RCSetShader &);
            RCSetShader & operator=(const RCSetShader &);
        };


        // A general function for drawing a vertex array.
        //
        // The way the renderer works is that is creates a single set of render commands that will be reused over multiple passes for
        // drawing the visible geometry. This is done for efficiency.
        //
        // There are two general phases to the renderer. As the command is executed, it sets flags to control branching. The first
        // operation will perform the required operations for the material pass. Once this has been executed, it will set a flag
        // which will cause it to branch for the second phase, which is the lighting pass.
        struct RCDrawGeometry : public RenderCommand
        {
            RCDrawGeometry()
                : materialParameters(), va(),
                  mvpMatrix(), normalMatrix(), modelViewMatrix(), modelMatrix(),
                  changeFaceCulling(), cullFrontFace(), cullBackFace(), fill(true),
                  enablePolygonOffset(false), polygonOffsetFactor(1.0f), polygonOffsetUnits(1.0f),
                  doingMaterialPass(true)
            {
            }


            /// RenderCommand::Execute()
            void Execute();



            /// The shader parameters to set for the material.
            ShaderParameterCache materialParameters;


            /// The vertex array to draw.
            VertexArray* va;

            /// The MVP matrix to apply to the geometry.
            glm::mat4 mvpMatrix;
            glm::mat3 normalMatrix;
            glm::mat4 modelViewMatrix;
            glm::mat4 modelMatrix;

            /// Whether or not the face culling should change.
            bool changeFaceCulling;
            bool cullFrontFace;
            bool cullBackFace;
            bool fill;

            /// Keeps track of whether or not to do a polygon offset.
            bool enablePolygonOffset;

            float polygonOffsetFactor;
            float polygonOffsetUnits;

            /// Keeps track of whether or not to do the material pass. Should default to true.
            bool doingMaterialPass;


        private:    // No copying.
            RCDrawGeometry(const RCDrawGeometry &);
            RCDrawGeometry & operator=(const RCDrawGeometry &);
        };

        // Draws the light geometry.
        struct RCDrawLightGeometry : public RenderCommand
        {
            RCDrawLightGeometry()
                : va(), mvpMatrix(), modelViewMatrix()
            {
            }

            void Execute();

            VertexArray* va;

            glm::mat4 mvpMatrix;
            glm::mat4 modelViewMatrix;


        private:    // No copying.
            RCDrawLightGeometry(const RCDrawLightGeometry &);
            RCDrawLightGeometry & operator=(const RCDrawLightGeometry &);
        };

        // Draws a fullscreen quad light geometry, with the quad vertices being at the four corners of the far clipping plane.
        struct RCDrawFSQuadLightGeometry : public RenderCommand
        {
            RCDrawFSQuadLightGeometry()
                : viewport(), mvpMatrix(), inverseProjectionMatrix()
            {
            }

            void Execute();

            SceneViewport* viewport;
            glm::mat4 mvpMatrix;
            glm::mat4 inverseProjectionMatrix;


        private:    // No copying.
            RCDrawFSQuadLightGeometry(const RCDrawFSQuadLightGeometry &);
            RCDrawFSQuadLightGeometry & operator=(const RCDrawFSQuadLightGeometry &);
        };



        // Sets a shader.
        struct RCLighting_SetShader : public RenderCommand
        {
            RCLighting_SetShader()
                : shader(), parameters(),
                  materialBuffer2(), linearDepthBuffer(),
                  screenSize(), cameraPosition(), cameraFOV(), cameraAspect(),
                  viewMatrix(), zFar()
            {
            }


            void Execute();

            /// Sets a shader parameter that will be set on the shader in Execute(). It's important that parameters are not set
            /// while Execute() is running.
            template <typename T>
            void SetParameter(const char* name, const T &value)
            {
                this->parameters.Set(name, value);
            }


            Shader* shader;
            ShaderParameterCache parameters;

            Texture2D* materialBuffer2;
            Texture2D* linearDepthBuffer;

            glm::vec2  screenSize;
            glm::vec3  cameraPosition;
            float      cameraFOV;
            float      cameraAspect;

            glm::mat4  viewMatrix;

            float zFar;

        private:    // No copying.
            RCLighting_SetShader(const RCLighting_SetShader &);
            RCLighting_SetShader & operator=(const RCLighting_SetShader &);
        };



        // Begins the generation of a directional light shadow map.
        struct RCLighting_BeginDirectionalShadowMap : public RenderCommand
        {
            RCLighting_BeginDirectionalShadowMap()
                : shader(), framebuffer(), mvpMatrix()
            {
            }

            void Execute();

            /// The shader to use with the geometry for the light.
            Shader* shader;

            /// The shadow map framebuffer.
            GTEngine::Framebuffer* framebuffer;

            /// The MVP mattrix.
            glm::mat4 mvpMatrix;


        private:    // No copying.
            RCLighting_BeginDirectionalShadowMap(const RCLighting_BeginDirectionalShadowMap &);
            RCLighting_BeginDirectionalShadowMap & operator=(const RCLighting_BeginDirectionalShadowMap &);
        };



        // Begins the generation of a point light shadow map.
        struct RCLighting_BeginPointShadowMap : public RenderCommand
        {
            RCLighting_BeginPointShadowMap()
                : shader(), framebuffer()
            {
            }

            void Execute();

            /// The shader to use with geometry for the light.
            Shader* shader;

            /// The shadow map framebuffer.
            GTEngine::Framebuffer* framebuffer;


        private:    // No copying.
            RCLighting_BeginPointShadowMap(const RCLighting_BeginPointShadowMap &);
            RCLighting_BeginPointShadowMap & operator=(const RCLighting_BeginPointShadowMap &);
        };

        // Begins the generation of an individual face on a point light shadow map.
        struct RCLighting_BeginPointShadowMapFace : public RenderCommand
        {
            RCLighting_BeginPointShadowMapFace()
                : colourBufferIndex()
            {
            }

            void Execute();

            /// The index of the colour buffer to use as the render target.
            int colourBufferIndex;
        };

        // Ends the shadow map pass.
        struct RCLighting_EndShadowMap : public RenderCommand
        {
            RCLighting_EndShadowMap()
                : newFramebuffer()
            {
            }

            void Execute();

            /// The new framebuffer.
            Framebuffer* newFramebuffer;


        private:    // No copying.
            RCLighting_EndShadowMap(const RCLighting_EndShadowMap &);
            RCLighting_EndShadowMap & operator=(const RCLighting_EndShadowMap &);
        };

        // Draws some geometry on the shadow pass.
        struct RCLighting_DrawShadowPassGeometry : public RenderCommand
        {
            RCLighting_DrawShadowPassGeometry()
                : va(nullptr), mvpMatrix(), modelViewMatrix()
            {
            }

            void Execute();

            /// The vertex array to draw.
            VertexArray* va;

            glm::mat4 mvpMatrix;
            glm::mat4 modelViewMatrix;

        private:    // No copying.
            RCLighting_DrawShadowPassGeometry(const RCLighting_DrawShadowPassGeometry &);
            RCLighting_DrawShadowPassGeometry & operator=(const RCLighting_DrawShadowPassGeometry &);
        };


        /// Performs a colour clear, clipping against the stencil buffer.
        struct RCColourClear : public RenderCommand
        {
            RCColourClear()
                : colour()
            {
            }

            void Execute();

            glm::vec3 colour;
        };


        /// Performs the background colour clear.
        struct RCBackgroundColourClear : public RenderCommand
        {
            RCBackgroundColourClear()
                : colour(), colourClearShader()
            {
            }

            void Execute();

            glm::vec3 colour;
            Shader* colourClearShader;


        private:    // No copying.
            RCBackgroundColourClear(const RCBackgroundColourClear &);
            RCBackgroundColourClear & operator=(const RCBackgroundColourClear &);
        };



        /// Builds the final image from the different buffers.
        struct RCBuildFinalImage : public RenderCommand
        {
            RCBuildFinalImage()
                : colourBufferIndex(), framebuffer(), compositingShader()
            {
            }


            void Execute();


            // The index of the colour buffer to draw the final image into.
            int colourBufferIndex;

            /// The framebuffer containing the buffers to build the final image from.
            DefaultSceneRenderer::Framebuffer* framebuffer;

            /// The compositing shader.
            Shader* compositingShader;


        private:    // No copying.
            RCBuildFinalImage(const RCBuildFinalImage &);
            RCBuildFinalImage & operator=(const RCBuildFinalImage &);
        };



    private:

        /// Retrieves the framebuffer of the given viewport.
        DefaultSceneRenderer::Framebuffer* GetViewportFramebuffer(SceneViewport &viewport);


        /// Renders the opaque part of the main layer.
        void RenderOpaqueMainLayer(Scene &scene, SceneViewport &viewport);

        /// Renders the opaque part of the background layer.
        void RenderOpaqueBackgroundLayer(Scene &scene, SceneViewport &viewport);

        /// Renders the transparent part of the main layer.
        void RenderTransparentMainLayer(Scene &scene, SceneViewport &viewport);

        /// Renders the transparent part of the background layer.
        void RenderTransparentBackgroundLayer(Scene &scene, SceneViewport &viewport);



        /// Performs the material pass. This is always the first pass.
        void MaterialPass(Scene &scene, DefaultSceneRenderer::Framebuffer &framebuffer, LayerState &state, bool refractive);

        /// Performs the lighting pass. This always comes after the material pass.
        void LightingPass(Scene &scene, DefaultSceneRenderer::Framebuffer &framebuffer, LayerState &state, bool refractive, int stencilIndex);



        // Builds the shadow map of the given directional light.
        void LightingPass_BuildShadowMap(Scene &scene, DefaultSceneRenderer::Framebuffer &mainFramebuffer, const glm::mat4 &projection, const glm::mat4 &view);

        // Builds the shadow map of the given point light.
        void LightingPass_BuildPointLightShadowMap(Scene &scene, DefaultSceneRenderer::Framebuffer &mainFramebuffer, const glm::vec3 &position, float radius);



        /// Retrieves the metadata of a material. This should never return null. If the metadata hasn't yet been created,
        /// it will be created and then returned. Future calls will return that same object.
        MaterialMetadata & GetMaterialMetadata(Material &material);


        /// Creates and returns the shader for use in the material pass.
        Shader* CreateMaterialPassShader(Material &material);



        /// Retrieves the vertex array to use with the given mesh.
        ///
        /// @param mesh      [in] A reference to the mesh whose vertex array is being retrieved.
        /// @param animating [in] Whether or not the mesh is being animated. This is used to determine whether or not to retrieve the skinned geometry, or the base geometry.
        ///
        /// @remarks
        ///     Don't use this in the material pass.
        VertexArray* GetMeshGeometry(Mesh &mesh, bool animating);


    private:

        /// A container for mapping a viewport to it's framebuffer.
        GTCore::Map<SceneViewport*, DefaultSceneRenderer::Framebuffer*> viewportFramebuffers;


        // Below are caches for render commands. There are always 2 caches - one for the front RC queue, and another for the back.
        RCCache<RCBegin,                        8>    rcBegin[2];
        RCCache<RCBeginBackground,              8>    rcBeginBackground[2];
        RCCache<RCBackgroundColourClear,        8>    rcBackgroundColourClear[2];
        RCCache<RCBuildFinalImage,              8>    rcBuildFinalImage[2];
        RCCache<RCBeginLighting,                8>    rcBeginLighting[2];
        RCCache<RCBeginTransparency,            8>    rcBeginTransparency[2];
        RCCache<RCBeginForegroundTransparency,  8>    rcBeginForegroundTransparency[2];
        RCCache<RCBeginTransparentMaterialPass, 8>    rcBeginTransparentMaterialPass[2];
        RCCache<RCControlBlending>                    rcControlBlending[2];
        RCCache<RCSetShader>                          rcSetShader[2];
        RCCache<RCDrawGeometry>                       rcDrawGeometry[2];
        RCCache<RCDrawLightGeometry>                  rcDrawLightGeometry[2];
        RCCache<RCDrawFSQuadLightGeometry>            rcDrawFSQuadLightGeometry[2];
        RCCache<RCLighting_SetShader>                 rcLighting_SetShader[2];
        RCCache<RCLighting_BeginDirectionalShadowMap> rcLighting_BeginDirectionalShadowMap[2];
        RCCache<RCLighting_BeginPointShadowMap>       rcLighting_BeginPointShadowMap[2];
        RCCache<RCLighting_BeginPointShadowMapFace>   rcLighting_BeginPointShadowMapFace[2];
        RCCache<RCLighting_EndShadowMap>              rcLighting_EndShadowMap[2];
        RCCache<RCLighting_DrawShadowPassGeometry>    rcLighting_DrawShadowPassGeometry[2];


        // Below are various shaders for use by the renderer.
        struct _Shaders
        {
            _Shaders()
                : Lighting_NoShadow_A1(nullptr),
                  Lighting_NoShadow_D1(nullptr), Lighting_D1(nullptr),
                  Lighting_NoShadow_P1(nullptr), Lighting_P1(nullptr),
                  Lighting_NoShadow_S1(nullptr), Lighting_S1(nullptr),
                  Lighting_NoShadow_D1_Trans(nullptr), Lighting_D1_Trans(nullptr),
                  Lighting_NoShadow_P1_Trans(nullptr), Lighting_P1_Trans(nullptr),
                  Lighting_NoShadow_S1_Trans(nullptr), Lighting_S1_Trans(nullptr),
                  Lighting_ShadowMap(nullptr), Lighting_PointLightShadowMap(nullptr),
                  Lighting_ColourClear(nullptr),
                  Compositor_DiffuseOnly(nullptr), Compositor_NormalsOnly(nullptr), Compositor_DiffuseLightingOnly(nullptr),
                  Compositor_OpaqueFinalOutput(nullptr), Compositor_FinalOutput(nullptr),
                  MaterialPass_ClearBackground(nullptr),
                  materialPassShaders()
            {
            }


            Shader* Lighting_NoShadow_A1;

            Shader* Lighting_NoShadow_D1;
            Shader* Lighting_D1;
            Shader* Lighting_NoShadow_P1;
            Shader* Lighting_P1;
            Shader* Lighting_NoShadow_S1;
            Shader* Lighting_S1;

            Shader* Lighting_NoShadow_D1_Trans;
            Shader* Lighting_D1_Trans;
            Shader* Lighting_NoShadow_P1_Trans;
            Shader* Lighting_P1_Trans;
            Shader* Lighting_NoShadow_S1_Trans;
            Shader* Lighting_S1_Trans;

            Shader* Lighting_ShadowMap;
            Shader* Lighting_PointLightShadowMap;
            Shader* Lighting_ColourClear;

            Shader* Compositor_DiffuseOnly;
            Shader* Compositor_NormalsOnly;
            Shader* Compositor_DiffuseLightingOnly;
            Shader* Compositor_OpaqueFinalOutput;
            Shader* Compositor_FinalOutput;

            Shader* MaterialPass_ClearBackground;

            /// A cache of shaders used by materials in the material pass.
            MaterialShaderCache materialPassShaders;


        private:    // No copying.
            _Shaders(const _Shaders &);
            _Shaders & operator=(const _Shaders &);

        }Shaders;


        /// The list of metadata pointers that will need to be removed when the renderer is destructed. We only really keep these for clean destruction.
        GTCore::List<MaterialMetadata*> materialMetadatas;


        /// The normal shadow map for lights requiring only a single 2D texture.
        Texture2D shadowMap;

        /// The depth buffer for the normal shadow map.
        Texture2D shadowMapDepthBuffer;

        /// The framebuffer to use for normal shadow maps.
        GTEngine::Framebuffer shadowMapFramebuffer;


        /// The cube map to use for point lights.
        TextureCube pointLightShadowMap;

        /// The depth buffer for the point light shadow map framebuffer.
        Texture2D pointLightShadowMapDepthBuffer;

        /// The framebuffer to use for point light shadow maps.
        GTEngine::Framebuffer pointLightShadowMapFramebuffer;



        /// The rendering state of the main layer.
        LayerState mainLayerState;

        /// The rendering state of the background layer.
        LayerState backgroundLayerState;



        /// The material definition for drawing wireframes.
        MaterialDefinition wireframeMaterialDefinition;

        /// The material to use for drawing wireframes.
        Material* wireframeMaterial;
    };
}

#endif
