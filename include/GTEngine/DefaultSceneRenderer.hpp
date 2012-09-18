
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


        /// SceneRenderer::EnableBackgroundColourClearing().
        virtual void EnableBackgroundColourClearing(float r, float g, float b);

        /// SceneRenderer::DisableBackgroundColourClearing().
        virtual void DisableBackgroundColourClearing();


        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // Methods below should only be called internally, but need to be public for a few things.

        /// Called for a model that's visible in the currently rendering viewport.
        void __MaterialPass_Model(const SceneObject &object);

        /// Called for a model that's visible in a shadow pass.
        void __ShadowPass_Model(const SceneObject &object, const glm::mat4 &projection, const glm::mat4 &view);


        /// Called for an ambient light that's visible in the currently rendering viewport.
        void __AmbientLight(const SceneObject &object);

        /// Called for a directional light that's visible in the currently rendering viewport.
        void __DirectionalLight(const SceneObject &object);

        /// Called for a point light that's visible in the currently rendering viewport.
        void __PointLight(const SceneObject &object);

        /// Called for a spot light that's visible in the currently rendering viewport.
        void __SpotLight(const SceneObject &object);



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
                  materialBuffer0(nullptr), materialBuffer1(nullptr), materialBuffer2(nullptr)
            {
                this->depthStencil = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_Depth24_Stencil8);

                if (Renderer::SupportFloatTextures())
                {
                    this->finalOutput     = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                    this->lightingBuffer0 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                    this->lightingBuffer1 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                    this->materialBuffer0 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->materialBuffer1 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->materialBuffer2 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);       // <-- Look into using R10G10B10A2. Need to handle sign.
                }
                else
                {
                    this->finalOutput     = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->lightingBuffer0 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->lightingBuffer1 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->materialBuffer0 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->materialBuffer1 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                    this->materialBuffer2 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                }

                this->finalOutput->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);
                this->lightingBuffer0->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);
                this->lightingBuffer1->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);
                this->materialBuffer0->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);
                this->materialBuffer1->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);
                this->materialBuffer2->SetFilter(GTEngine::TextureFilter_Nearest, GTEngine::TextureFilter_Nearest);

                this->finalOutput->SetWrapMode(GTEngine::TextureWrapMode_ClampToEdge);


                this->AttachDepthStencilBuffer(this->depthStencil);
                this->AttachColourBuffer(this->finalOutput,     0);
                this->AttachColourBuffer(this->materialBuffer0, 1);
                this->AttachColourBuffer(this->materialBuffer1, 2);
                this->AttachColourBuffer(this->materialBuffer2, 3);
                this->AttachColourBuffer(this->lightingBuffer0, 4);
                this->AttachColourBuffer(this->lightingBuffer1, 5);
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
            
                this->finalOutput     = nullptr;
                this->depthStencil    = nullptr;
                this->lightingBuffer0 = nullptr;
                this->lightingBuffer1 = nullptr;
                this->materialBuffer0 = nullptr;
                this->materialBuffer1 = nullptr;
                this->materialBuffer2 = nullptr;
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

            /// The depth/stencil attachment.
            Texture2D* depthStencil;

            // The lighting buffers.
            Texture2D* lightingBuffer0;     // RGB = Diffuse.  A = nothing.
            Texture2D* lightingBuffer1;     // RGB = Specular. A = nothing.

            // The material buffers.
            Texture2D* materialBuffer0;     // RGB = Diffuse.  A = Shininess.
            Texture2D* materialBuffer1;     // RGB = Emissive. A = Transparency.
            Texture2D* materialBuffer2;     // RGB = Normals for normal mapping. A = nothing.
        };



        /// Render command for beginning the frame.
        struct RCBegin : public RenderCommand
        {
            /// RenderCommand::Execute().
            void Execute();


            /// The framebuffer to render to.
            Framebuffer* framebuffer;
        };

        /// Render command for ending the frame.
        struct RCEnd : public RenderCommand
        {
            /// RenderCommand::Execute().
            void Execute();


            /// Whether or not the draw the background.
            bool drawBackground;

            /// The background colour.
            glm::vec3 backgroundColour;
        };

        // Render command for beginning a layer.
        struct RCBeginLayer : public RenderCommand
        {
            void Execute();

            bool isFirstLayer;
        };

        // Render command for ending a layer.
        struct RCEndLayer : public RenderCommand
        {
            void Execute();

            DefaultSceneRenderer::Framebuffer* framebuffer;
            Shader* compositingShader;
        };

        // Render command for beginning the lighting pass.
        struct RCBeginLighting : public RenderCommand
        {
            void Execute();

            DefaultSceneRenderer::Framebuffer* framebuffer;
        };

        // Render command for controlling blending.
        struct RCControlBlending : public RenderCommand
        {
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
            void Execute();

            /// The shader to activate.
            Shader* shader;
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
                : doingMaterialPass(true)
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

            /// Keeps track of whether or not to do the material pass. Should default to true.
            bool doingMaterialPass;
        };



        // Sets a shader.
        struct RCLighting_SetShader : public RenderCommand
        {
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
            glm::vec2  screenSize;
        };



        // Begins the generation of a directional light shadow map.
        struct RCLighting_BeginDirectionalShadowMap : public RenderCommand
        {
            void Execute();

            /// The shader to use with the geometry for the light.
            Shader* shader;

            /// The shadow map framebuffer.
            GTEngine::Framebuffer* framebuffer;

            /// The MVP mattrix.
            glm::mat4 mvpMatrix;
        };


        
        // Begins the generation of a point light shadow map.
        struct RCLighting_BeginPointShadowMap : public RenderCommand
        {
            void Execute();

            /// The shader to use with geometry for the light.
            Shader* shader;

            /// The shadow map framebuffer.
            GTEngine::Framebuffer* framebuffer;
        };

        // Begins the generation of an individual face on a point light shadow map.
        struct RCLighting_BeginPointShadowMapFace : public RenderCommand
        {
            void Execute();

            /// The index of the colour buffer to use as the render target.
            int colourBufferIndex;
        };

        // Ends the shadow map pass.
        struct RCLighting_EndShadowMap : public RenderCommand
        {
            void Execute();

            /// The new framebuffer.
            Framebuffer* newFramebuffer;
        };

        // Draws some geometry on the shadow pass.
        struct RCLighting_DrawShadowPassGeometry : public RenderCommand
        {
            void Execute();

            /// The vertex array to draw.
            VertexArray* va;

            glm::mat4 mvpMatrix;
            glm::mat4 modelViewMatrix;
        };



    private:

        /// Retrieves the framebuffer of the given viewport.
        DefaultSceneRenderer::Framebuffer* GetViewportFramebuffer(SceneViewport &viewport);


        /// Performs the material pass. This is always the first pass.
        void MaterialPass(Scene &scene);

        /// Performs the lighting pass. This always comes after the material pass.
        void LightingPass(Scene &scene, DefaultSceneRenderer::Framebuffer &framebuffer);


        // Builds the shadow map of the given directional light.
        void LightingPass_BuildShadowMap(Scene &scene, DefaultSceneRenderer::Framebuffer &mainFramebuffer, const glm::mat4 &projection, const glm::mat4 &view);

        // Builds the shadow map of the given point light.
        void LightingPass_BuildPointLightShadowMap(Scene &scene, DefaultSceneRenderer::Framebuffer &mainFramebuffer, const glm::vec3 &position, float radius);


        /// Structure containing metadata for materials. There should be one of these for each material.
        struct MaterialMetadata
        {
            MaterialMetadata()
                : materialPassShader(nullptr)
            {
            }

            Shader* materialPassShader;         ///< The shader for the material pass.
            RCQueue materialPassRCs;            ///< The render commands for meshes using this material in the material pass.
        };

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


        /// A container for mapping a viewport to it's framebuffer.
        GTCore::Map<SceneViewport*, DefaultSceneRenderer::Framebuffer*> viewportFramebuffers;


        /// Keeps track of whether or not the colour buffer should be cleared.
        bool clearColourBuffer;

        /// The colour to clear the colour buffer with.
        glm::vec3 clearColour;


        /// The projection matrix. This is updated at the start of each render.
        glm::mat4 projection;
        
        /// The view matrix. This is updated at the start of each render.
        glm::mat4 view;


        // Below are caches for render commands. There are always 2 caches - one for the front RC queue, and another for the back.
        RCCache<RCBegin,      8>                      rcBegin[2];
        RCCache<RCEnd,        8>                      rcEnd[2];
        RCCache<RCBeginLayer, 8>                      rcBeginLayer[2];
        RCCache<RCEndLayer,   8>                      rcEndLayer[2];
        RCCache<RCBeginLighting>                      rcBeginLighting[2];
        RCCache<RCControlBlending>                    rcControlBlending[2];
        RCCache<RCSetShader>                          rcSetShader[2];
        RCCache<RCDrawGeometry>                       rcDrawGeometry[2];
        RCCache<RCLighting_SetShader>                 rcLighting_SetShader[2];
        RCCache<RCLighting_BeginDirectionalShadowMap> rcLighting_BeginDirectionalShadowMap[2];
        RCCache<RCLighting_BeginPointShadowMap>       rcLighting_BeginPointShadowMap[2];
        RCCache<RCLighting_BeginPointShadowMapFace>   rcLighting_BeginPointShadowMapFace[2];
        RCCache<RCLighting_EndShadowMap>              rcLighting_EndShadowMap[2];
        RCCache<RCLighting_DrawShadowPassGeometry>    rcLighting_DrawShadowPassGeometry[2];

        
        // Below are various shaders for use by the renderer.
        struct
        {
            Shader* Lighting_NoShadow_A1;
            
            Shader* Lighting_NoShadow_D1;
            Shader* Lighting_D1;
            Shader* Lighting_NoShadow_P1;
            Shader* Lighting_P1;
            Shader* Lighting_NoShadow_S1;
            Shader* Lighting_S1;

            Shader* Lighting_ShadowMap;
            Shader* Lighting_PointLightShadowMap;

            Shader* Compositor_DiffuseOnly;
            Shader* Compositor_NormalsOnly;
            Shader* Compositor_DiffuseLightingOnly;
            Shader* Compositor_FinalOutput;

            /// A cache of shaders used by materials in the material pass.
            MaterialShaderCache materialPassShaders;

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


        /// The metadata of the material definitions being used in the current frame. This is cleared at the beginning of every frame.
        GTCore::BinarySearchTree<MaterialMetadata*> usedMaterials;


        /// Every piece of visible geometry will need to be re-rendered for every light. This needs to be efficient. To do this, we cache a draw call that
        /// will draw the geometry in each lighting pass, and then append that to the renderer's back RC cache in one go every time it's needed. It will
        /// be cleared at the beginning of every frame.
        RCQueue lightingDrawRCs;
    };
}

#endif