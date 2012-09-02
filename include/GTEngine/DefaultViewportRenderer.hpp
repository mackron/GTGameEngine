
// In this file, DVR = DefaultViewportRenderer.

#ifndef __GTEngine_DefaultViewportRenderer_hpp_
#define __GTEngine_DefaultViewportRenderer_hpp_

#include "ViewportRenderer.hpp"
#include "SceneViewport.hpp"
#include "ShaderParameter.hpp"
#include "Material.hpp"
#include "MaterialShaderCache.hpp"
#include "MaterialLibrary.hpp"
#include "Mesh.hpp"
#include "GarbageCollector.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderCommand.hpp"
#include "Rendering/RenderCommands/RCDrawVA.hpp"
#include "Rendering/RenderCommands/RCSetFaceCulling.hpp"
#include "Rendering/RCCache.hpp"
#include <GTCore/Map.hpp>
#include <GTCore/String.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    class DVRFramebuffer;
    class DVRFramebuffer_RendererEventHandler : public RendererEventHandler
    {
    public:

        /// Constructor.
        DVRFramebuffer_RendererEventHandler(DVRFramebuffer &framebuffer)
            : framebuffer(framebuffer)
        {
        }

        /// RendererEventHandler::OnSwapRCQueues.
        void OnSwapRCQueues();


        /// The framebuffer we need to resize.
        DVRFramebuffer &framebuffer;


    private:    // No copying.
        DVRFramebuffer_RendererEventHandler(const DVRFramebuffer_RendererEventHandler &);
        DVRFramebuffer_RendererEventHandler & operator=(const DVRFramebuffer_RendererEventHandler &);
    };


    class DVRFramebuffer : public Framebuffer
    {
    public:

        DVRFramebuffer()
            : Framebuffer(),
              width(0), height(0),
              depthStencil(nullptr),
              finalOutput(nullptr),
              lightingBuffer0(nullptr), lightingBuffer1(nullptr),
              materialBuffer0(nullptr), materialBuffer1(nullptr), materialBuffer2(nullptr),
              rendererEventHandler(*this),
              needsResize(false)
        {
            this->CreateAttachments(1, 1);
            Renderer::AttachEventHandler(this->rendererEventHandler);
        }

        ~DVRFramebuffer()
        {
            this->DeleteAll();
        }


        /// Marks the framebuffer as needing to be resized.
        ///
        /// @param newWidth  [in] The new width of the framebuffer.
        /// @param newHeight [in] The new height of the framebuffer.
        ///
        /// @remarks
        ///     Note that this is not instantaneous - it will be delayed until the next buffer swap where the actual resize can be done in a thread safe environment.
        void Resize(unsigned int newWidth, unsigned int newHeight)
        {
            this->needsResize = true;
            this->width  = newWidth;
            this->height = newHeight;
        }



    // The methods below should only be used internally.
    public:

        /// Determines whether or not the framebuffer needs to be resized. Should only be used internally by the event handler.
        bool __NeedsResize() const { return this->needsResize; }


        /// Performs the actual resize. Do not call this directly. This should only be called by the event handler.
        void __DoResize()
        {
            // We need to create a new set of attachments. We don't delete the old attachments straight away. Instead we mark them as dead
            // and let CleanDeadAttachments() do the proper cleanup. This is required because of multithreading.
            this->MarkAttachmentsAsDead();
            this->CreateAttachments(this->width, this->height);

            this->needsResize = false;
        }


    private:

        /// Creates the attachment points using the given width and height. This does not do any actual attachments, only creates the buffers.
        void CreateAttachments(unsigned int width, unsigned int height)
        {
            this->width  = width;
            this->height = height;

            this->depthStencil = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_Depth24_Stencil8);

            if (Renderer::SupportFloatTextures())
            {
                this->finalOutput     = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                this->lightingBuffer0 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                this->lightingBuffer1 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                this->materialBuffer0 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                this->materialBuffer1 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                this->materialBuffer2 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
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

        /// Deletes the attachments.
        void DeleteAll()
        {
            this->DetachAllBuffers();

            delete this->depthStencil;
            delete this->finalOutput;
            delete this->lightingBuffer0;
            delete this->lightingBuffer1;
            delete this->materialBuffer0;
            delete this->materialBuffer1;
            delete this->materialBuffer2;
            
            this->depthStencil    = nullptr;
            this->finalOutput     = nullptr;
            this->lightingBuffer0 = nullptr;
            this->lightingBuffer1 = nullptr;
            this->materialBuffer0 = nullptr;
            this->materialBuffer1 = nullptr;
            this->materialBuffer2 = nullptr;
        }

        /// Marks all of the currently attachment buffers as dead. Also detaches them.
        void MarkAttachmentsAsDead()
        {
            this->DetachAllBuffers();

            if (this->depthStencil    != nullptr) GarbageCollector::MarkForCollection(*this->depthStencil);
            if (this->finalOutput     != nullptr) GarbageCollector::MarkForCollection(*this->finalOutput);
            if (this->lightingBuffer0 != nullptr) GarbageCollector::MarkForCollection(*this->lightingBuffer0);
            if (this->lightingBuffer1 != nullptr) GarbageCollector::MarkForCollection(*this->lightingBuffer1);
            if (this->materialBuffer0 != nullptr) GarbageCollector::MarkForCollection(*this->materialBuffer0);
            if (this->materialBuffer1 != nullptr) GarbageCollector::MarkForCollection(*this->materialBuffer1);
            if (this->materialBuffer2 != nullptr) GarbageCollector::MarkForCollection(*this->materialBuffer2);

            this->depthStencil    = nullptr;
            this->finalOutput     = nullptr;
            this->lightingBuffer0 = nullptr;
            this->lightingBuffer1 = nullptr;
            this->materialBuffer0 = nullptr;
            this->materialBuffer1 = nullptr;
            this->materialBuffer2 = nullptr;
        }


    public:

        // The size of the framebuffer's dimensions.
        unsigned int width;
        unsigned int height;

        // These attachments need to be pointers because when resizing the framebuffer, we'll be deleting and re-creating the textures.
        Texture2D* depthStencil;
        Texture2D* finalOutput;

        // The lighting buffers.
        Texture2D* lightingBuffer0;     // RGB = Diffuse.  A = nothing.
        Texture2D* lightingBuffer1;     // RGB = Specular. A = nothing.

        // The material buffers.
        Texture2D* materialBuffer0;     // RGB = Diffuse.  A = Shininess.
        Texture2D* materialBuffer1;     // RGB = Emissive. A = Transparency.
        Texture2D* materialBuffer2;     // RGB = Normals for normal mapping. A = nothing.

        
        /// The event handler we'll attach to the renderer so we can handle the OnSwapRCQueues event and do a resize when required.
        DVRFramebuffer_RendererEventHandler rendererEventHandler;

        /// Keeps track of whether or not the framebuffer needs to be resized.
        bool needsResize;
    };
}

/*** Render Commands ***/
namespace GTEngine
{
    // RCBegin
    class DVR_RCBegin : public RenderCommand
    {
    public:

        DVR_RCBegin();

        void Init(DVRFramebuffer &framebuffer);

        void Execute();


    public:

        /// The framebuffer to make current.
        DVRFramebuffer* framebuffer;

        unsigned int viewportWidth;
        unsigned int viewportHeight;
    };


    // RCEnd
    class DVR_RCEnd : public RenderCommand
    {
    public:
        
        /// Constructor.
        DVR_RCEnd();

        /// RenderCommand::Execute().
        void Execute();


    public:

        /// Whether or not to draw the background.
        bool drawBackground;

        /// The colour of the background.
        glm::vec3 backgroundColour;
    };



    // RCBeginLayer.
    class DVR_RCBeginLayer : public RenderCommand
    {
    public:

        /// Constructor.
        DVR_RCBeginLayer();

        /// RenderCommand::Execute().
        void Execute();


    public:

        /// Whether or not this is being called for the first layer.
        bool isFirstLayer;
    };


    // RCEndLighting
    class DVR_RCEndLayer : public RenderCommand
    {
    public:

        /// Constructor.
        DVR_RCEndLayer();

        /// Initialises the command from the given framebuffer.
        void Init(DVRFramebuffer &framebuffer, Shader* combinerShader);

        /// RenderCommand::Execute().
        void Execute();


    public:

        /// The framebuffer to make current.
        DVRFramebuffer* framebuffer;

        /// The combiner shader.
        Shader* combinerShader;

        /// The buffer that will eventually contain the final output.
        Texture2D* finalOutputBuffer;

        /// Input textures for the combiner.
        Texture2D* lightingBuffer0;
        Texture2D* lightingBuffer1;
        Texture2D* materialBuffer0;
        Texture2D* materialBuffer1;
        Texture2D* materialBuffer2;
    };




    // RCBeginLighting
    class DVR_RCBeginLighting : public RenderCommand
    {
    public:

        DVR_RCBeginLighting();

        /// Sets the framebuffer.
        void SetFramebuffer(DVRFramebuffer &framebuffer)
        {
            this->framebuffer = &framebuffer;
            this->lightingBuffer0 = framebuffer.lightingBuffer0;
            this->lightingBuffer1 = framebuffer.lightingBuffer1;
        }

        void Execute();


    private:

        DVRFramebuffer* framebuffer;

        Texture2D* lightingBuffer0;
        Texture2D* lightingBuffer1;
    };


    /// RCControlBlending
    class DVR_RCControlBlending : public RenderCommand
    {
    public:

        /// Constructor.
        DVR_RCControlBlending();

        // Enables blending.
        void Enable(BlendFunc sourceFactor, BlendFunc destFactor)
        {
            this->enable = true;
            this->sourceFactor = sourceFactor;
            this->destFactor   = destFactor;
        }

        /// Sets up for alpha blending.
        void EnableAlphaBlending()
        {
            this->Enable(BlendFunc_SourceAlpha, BlendFunc_OneMinusSourceAlpha);
        }

        // Disables blending.
        void Disable()
        {
            this->enable = false;
        }
        


        void Execute();


    private:

        /// Controls whether or not the blending should be enabled.
        bool enable;

        /// Controls the source and dest factors.
        BlendFunc sourceFactor;
        BlendFunc destFactor;
    };


    // TODO: Look into removing this. All it does it set shader variables. We can update these manually only when needed.
    // RCBeginLightingPass
    class DVR_RCBeginLightingPass : public RenderCommand
    {
    public:

        DVR_RCBeginLightingPass();

        void Init(DVRFramebuffer &framebuffer, Shader* shader, const glm::vec2 &screenSize)
        {
            this->framebuffer = &framebuffer;

            this->materialBuffer2 = framebuffer.materialBuffer2;

            this->shader = shader;
            
            this->screenSize = screenSize;
        }

        void Execute();


    private:

        DVRFramebuffer* framebuffer;

        Texture2D* materialBuffer2;

        Shader* shader;

        glm::vec2 screenSize;
    };
}

namespace GTEngine
{
    class DefaultViewportRenderer : public ViewportRenderer
    {
    public:

        DefaultViewportRenderer();
        ~DefaultViewportRenderer();

        void SetOwnerViewport(SceneViewport* owner);
        SceneViewport* GetOwnerViewport();

        Framebuffer & GetFramebuffer();
        void ResizeFramebuffer(unsigned int newWidth, unsigned int newHeight);

        void Render();


        Texture2D* GetFinalColourOutputBuffer();
        Texture2D* GetFinalDepthStencilOutputBuffer();


        /// Sets the clear colour.
        void SetClearColour(float r, float g, float b);

        /// Disables clearing of the colour buffers.
        void DisableColourClears();

        /// Enables colour clearing.
        void EnableColourClears();



    // Private Structures.
    private:

        /// Structure containing metadata for materials. There should be one of these for each material.
        struct MaterialMetadata
        {
            MaterialMetadata()
                : materialPassShader(nullptr)
            {
            }

            Shader* materialPassShader;  ///< The shader for the material pass.
        };


    private:

        /// Helper function for drawing a scene node.
        void RenderNode(SceneNode* node);

        /// Retrieves the metadata of a material. This should never return null. If the metadata hasn't yet been created,
        /// it will be created and then returned. Future calls will return that same object.
        MaterialMetadata & GetMaterialMetadata(Material &material);

        /// Deletes the given material's metadata.
        void DeleteMaterialMetadata(Material &material);


        /// Creates and returns the shader to use with the material for simple unlit rendering.
        Shader* CreateSimpleUnlitShader(Material &material);

        /// Creates and returns the shader for use in the material pass.
        Shader* CreateMaterialPassShader(Material &material);

        /// Deletes a shader created with a Create*() function.
        void DeleteShader(Shader* shader);


        /// Performs the material pass. This pass will fill the depth buffer..
        void DoMaterialPass(const GTCore::Vector<ModelComponent*> &nodes);

        /// Performs the lighting pass.
        void DoLightingPass(
            const GTCore::Vector<ModelComponent*> &modelNodes,
            const GTCore::Vector<AmbientLightComponent*> &ambientLightNodes,
            const GTCore::Vector<DirectionalLightComponent*> &directionalLightNodes,
            const GTCore::Vector<PointLightComponent*> &pointLightNodes,
            const GTCore::Vector<SpotLightComponent*> &spotLightNodes);


        /// Performs a lighting pass for an ambient light.
        void DoLightingPass_A1(AmbientLightComponent* A0, const GTCore::Vector<ModelComponent*> &models);

        /// Performs a lighting pass for a directional light.
        void DoLightingPass_D1(DirectionalLightComponent* D0, const GTCore::Vector<ModelComponent*> &models);
        
        /// Performs a lighting pass for a point light.
        void DoLightingPass_P1(PointLightComponent* P0, const GTCore::Vector<ModelComponent*> &models);

        /// Performs a lighting pass for a spot light.
        void DoLightingPass_S1(SpotLightComponent* S0, const GTCore::Vector<ModelComponent*> &models);

        void DoLightingPass_A1D1(AmbientLightComponent* A0, DirectionalLightComponent* D0, const GTCore::Vector<ModelComponent*> &models);
        void DoLightingPass_A1P1(AmbientLightComponent* A0, PointLightComponent* P0, const GTCore::Vector<ModelComponent*> &models);


        /// Retrieves the vertex array to use with the given mesh.
        ///
        /// @param mesh      [in] A reference to the mesh whose vertex array is being retrieved.
        /// @param animating [in] Whether or not the mesh is being animated. This is used to determine whether or not to retrieve the skinned geometry, or the base geometry.
        ///
        /// @remarks
        ///     Don't use this in the material pass.
        VertexArray* GetMeshGeometry(Mesh &mesh, bool animating);


        /// Acquires render commands and sets common properties for the given model component.
        ///
        /// @return True if the commands were retrieved successfully; false otherwise.
        bool AcquireMeshRCs(ModelComponent &modelComponent, size_t meshIndex, const glm::mat4 &mvpMatrix, const glm::mat3 &normalMatrix, RCDrawVA* &rcDrawVA, RCSetFaceCulling* &rcSetFaceCulling);
        bool AcquireMeshRCs(ModelComponent &modelComponent, size_t meshIndex, const glm::mat4 &mvpMatrix, const glm::mat3 &normalMatrix, const glm::mat4 &modelViewMatrix, RCDrawVA* &rcDrawVA, RCSetFaceCulling* &rcSetFaceCulling);


    private:

        /// The owner viewport. This can be null, thus it needs to be a pointer.
        SceneViewport* owner;

        /// The framebuffer where the final output will be drawn to.
        DVRFramebuffer framebuffer;

        /// Structure containing the different shaders.
        struct
        {
            Shader* lightingD1;
            Shader* lightingA1;
            Shader* lightingP1;
            Shader* lightingS1;
            Shader* lightingA1D1;
            Shader* lightingA1P1;

            Shader* combiner;



            /// A cache of shaders used by materials in the material pass.
            MaterialShaderCache materialPassShaders;

        }Shaders;


        /// The cached rendering commands.
        struct
        {
            DVR_RCBegin         rcBegin[2];
            DVR_RCBeginLighting rcBeginLighting[2];
            DVR_RCEnd           rcEnd[2];

            RCCache<DVR_RCBeginLayer>            rcBeginLayer[2];
            RCCache<DVR_RCEndLayer>              rcEndLayer[2];
            RCCache<DVR_RCBeginLightingPass, 32> rcBeginLightingPass[2];
            RCCache<DVR_RCControlBlending>       rcControlBlending[2];
            RCCache<RCDrawVA>                    rcDrawVA[2];
            RCCache<RCSetFaceCulling>            rcSetFaceCulling[2];
            
        }RenderCommands;


        /// The event handler to use with the material library.
        class MaterialLibraryEventHandler : public MaterialLibrary::EventHandler
        {
        public:

            /// Constructor.
            MaterialLibraryEventHandler(DefaultViewportRenderer &renderer)
                : renderer(renderer)
            {
            }

            /// Destructor.
            ~MaterialLibraryEventHandler()
            {
            }

            
            /// MaterialLibrary::EventHandler::OnDeleteMaterial().
            void OnDeleteMaterial(Material &material)
            {
                this->renderer.DeleteMaterialMetadata(material);
            }


        private:

            /// A reference to the DefaultViewportRenderer object that owns this event handler.
            DefaultViewportRenderer &renderer;


        private:    // No copying.
            MaterialLibraryEventHandler(const MaterialLibraryEventHandler &);
            MaterialLibraryEventHandler & operator=(const MaterialLibraryEventHandler &);

        }materialLibraryEventHandler;


        /// The list of metadata pointers that will need to be removed when the renderer is destructed. We only really keep these for clean destruction.
        GTCore::List<MaterialMetadata*> materialMetadatas;

        /// Keeps track of whether or not the colour buffer should be cleared.
        bool clearColourBuffer;

        /// The colour to clear the colour buffer with.
        glm::vec3 clearColour;


        /// The projection matrix. This is updated at the start of each render.
        glm::mat4 projection;
        
        /// The view matrix. This is updated at the start of each render.
        glm::mat4 view;

        /// The screen size.
        glm::vec2 screenSize;

        /// This keeps track of whether or not the framebuffer needs to be resized. When ResizeFramebuffer() is called, it's not actually
        /// performed straight away. Instead we do it synchronously between frames in order to avoid synchronization issues.
        bool framebufferNeedsResize;


        /// The cache of model components.
        GTCore::Vector<ModelComponent*> modelComponents;

        /// The cache of ambient light components.
        GTCore::Vector<AmbientLightComponent*> ambientLightComponents;

        /// The cache of directional light components.
        GTCore::Vector<DirectionalLightComponent*> directionalLightComponents;

        /// The cache of point light components.
        GTCore::Vector<PointLightComponent*> pointLightComponents;

        /// The cache of spot light components.
        GTCore::Vector<SpotLightComponent*> spotLightComponents;



        /// A vector containing the rcSetFaceCulling render commands for transparent meshes.
        GTCore::Vector<RCSetFaceCulling*> rcSetFaceCulling_Transparent;
        
        /// A vector containing the rcDrawVA render commands for transparent meshes.
        GTCore::Vector<RCDrawVA*> rcDrawVA_Transparent;


    friend class MaterialLibraryEventHandler;
    };
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif