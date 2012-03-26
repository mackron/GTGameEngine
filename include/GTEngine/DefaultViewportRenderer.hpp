
#ifndef __GTEngine_DefaultViewportRenderer_hpp_
#define __GTEngine_DefaultViewportRenderer_hpp_

#include "ViewportRenderer.hpp"
#include "SceneViewport.hpp"
#include "ShaderParameter.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderCommands.hpp"
#include <GTCore/Map.hpp>
#include <GTCore/Dictionary.hpp>

namespace GTEngine
{
    class DefaultViewportRendererFramebuffer : public Framebuffer
    {
    public:

        DefaultViewportRendererFramebuffer()
            : Framebuffer(),
              width(0), height(0),
              depthStencil(nullptr),
              finalOutput(nullptr),
              lightingDiffuseOutput(nullptr), lightingDiffuseInput(nullptr), lightingSpecularOutput(nullptr), lightingSpecularInput(nullptr),
              materialDiffuse(nullptr), materialEmissive(nullptr),
              depthStencilDead(nullptr),
              finalOutputDead(nullptr),
              lightingDiffuseOutputDead(nullptr), lightingDiffuseInputDead(nullptr), lightingSpecularOutputDead(nullptr), lightingSpecularInputDead(nullptr),
              materialDiffuseDead(nullptr), materialEmissiveDead(nullptr)
        {
            this->CreateAttachments(1, 1);
        }

        ~DefaultViewportRendererFramebuffer()
        {
            this->DeleteAll();
        }


        void Resize(unsigned int newWidth, unsigned int newHeight)
        {
            // We need to create a new set of attachments. We don't delete the old attachments straight away. Instead we mark them as dead
            // and let CleanDeadAttachments() do the proper cleanup. This is required because of multithreading.
            this->MarkAttachmentsAsDead();
            this->CreateAttachments(newWidth, newHeight);
        }

        /// Swaps the output and input lighting buffers.
        void SwapLightingBuffers()
        {
            auto tempDiffuse            = this->lightingDiffuseInput;
            this->lightingDiffuseInput  = this->lightingDiffuseOutput;
            this->lightingDiffuseOutput = tempDiffuse;

            auto tempSpecular            = this->lightingSpecularInput;
            this->lightingSpecularInput  = this->lightingSpecularOutput;
            this->lightingSpecularOutput = tempSpecular;
        }

        /// Performs a cleanup of dead attachments.
        void DoCleanup()
        {
            this->DeleteDeadAttachments();
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
                this->finalOutput            = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                this->lightingDiffuseOutput  = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                this->lightingDiffuseInput   = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                this->lightingSpecularOutput = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                this->lightingSpecularInput  = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                this->materialDiffuse        = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
                this->materialEmissive       = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA16F);
            }
            else
            {
                this->finalOutput            = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                this->lightingDiffuseOutput  = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                this->lightingDiffuseInput   = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                this->lightingSpecularOutput = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                this->lightingSpecularInput  = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                this->materialDiffuse        = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
                this->materialEmissive       = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_RGBA8);
            }

            this->finalOutput->SetFilter(GTEngine::TextureFilter_Nearest);
            this->lightingDiffuseInput->SetFilter(GTEngine::TextureFilter_Nearest);
            this->lightingDiffuseOutput->SetFilter(GTEngine::TextureFilter_Nearest);
            this->lightingSpecularInput->SetFilter(GTEngine::TextureFilter_Nearest);
            this->lightingSpecularOutput->SetFilter(GTEngine::TextureFilter_Nearest);
            this->materialDiffuse->SetFilter(GTEngine::TextureFilter_Nearest);
            this->materialEmissive->SetFilter(GTEngine::TextureFilter_Nearest);

            this->finalOutput->SetWrapMode(GTEngine::TextureWrapMode_ClampToEdge);
        }

        /// Deletes the attachments.
        void DeleteAll()
        {
            this->DeleteDeadAttachments();

            delete this->depthStencil;
            delete this->finalOutput;
            delete this->lightingDiffuseInput;
            delete this->lightingDiffuseOutput;
            delete this->lightingSpecularInput;
            delete this->lightingSpecularOutput;
            delete this->materialDiffuse;
            delete this->materialEmissive;
            
            this->depthStencil           = nullptr;
            this->finalOutput            = nullptr;
            this->lightingDiffuseInput   = nullptr;
            this->lightingDiffuseOutput  = nullptr;
            this->lightingSpecularInput  = nullptr;
            this->lightingSpecularOutput = nullptr;
            this->materialDiffuse        = nullptr;
            this->materialEmissive       = nullptr;
        }

        /// Marks all of the currently attachment buffers as dead. Also detaches them.
        void MarkAttachmentsAsDead()
        {
            this->DetachAllBuffers();

            this->depthStencilDead           = this->depthStencil;
            this->finalOutputDead            = this->finalOutput;
            this->lightingDiffuseInputDead   = this->lightingDiffuseInput;
            this->lightingDiffuseOutputDead  = this->lightingDiffuseOutput;
            this->lightingSpecularInputDead  = this->lightingSpecularInput;
            this->lightingSpecularOutputDead = this->lightingSpecularOutput;
            this->materialDiffuseDead        = this->materialDiffuse;
            this->materialEmissiveDead       = this->materialEmissive;

            this->depthStencil           = nullptr;
            this->finalOutput            = nullptr;
            this->lightingDiffuseInput   = nullptr;
            this->lightingDiffuseOutput  = nullptr;
            this->lightingSpecularInput  = nullptr;
            this->lightingSpecularOutput = nullptr;
            this->materialDiffuse        = nullptr;
            this->materialEmissive       = nullptr;
        }

        /// When the framebuffer is resized, buffers are marked for deletion. This will delete the dead buffers.
        void DeleteDeadAttachments()
        {
            // We will do a quick check to see if deletion needs to occur. We can simply check just one of the dead buffers for null.
            if (this->finalOutputDead != nullptr)
            {
                delete this->depthStencilDead;
                delete this->finalOutputDead;
                delete this->lightingDiffuseInputDead;
                delete this->lightingDiffuseOutputDead;
                delete this->lightingSpecularInputDead;
                delete this->lightingSpecularOutputDead;
                delete this->materialDiffuseDead;
                delete this->materialEmissiveDead;
            
                this->depthStencilDead           = nullptr;
                this->finalOutputDead            = nullptr;
                this->lightingDiffuseInputDead   = nullptr;
                this->lightingDiffuseOutputDead  = nullptr;
                this->lightingSpecularInputDead  = nullptr;
                this->lightingSpecularOutputDead = nullptr;
                this->materialDiffuseDead        = nullptr;
                this->materialEmissiveDead       = nullptr;
            }
        }


    public:

        // The size of the framebuffer's dimensions.
        unsigned int width;
        unsigned int height;

        // These attachments need to be pointers because when resizing the framebuffer, we'll be deleting and re-creating the textures.
        Texture2D* depthStencil;
        Texture2D* finalOutput;

        // The lighting buffers are going to be swapped back and forth as rendering takes place. One buffer will be used for output,
        // while the other will be used as input. The reason is because often multiple passes will need to occur, and each pass needs
        // the output from the previous pass.
        Texture2D* lightingDiffuseOutput;
        Texture2D* lightingDiffuseInput;
        Texture2D* lightingSpecularOutput;
        Texture2D* lightingSpecularInput;

        // The material buffers.
        Texture2D* materialDiffuse;
        Texture2D* materialEmissive;


        // Dead buffers.
        Texture2D* depthStencilDead;
        Texture2D* finalOutputDead;
        Texture2D* lightingDiffuseOutputDead;
        Texture2D* lightingDiffuseInputDead;
        Texture2D* lightingSpecularOutputDead;
        Texture2D* lightingSpecularInputDead;
        Texture2D* materialDiffuseDead;
        Texture2D* materialEmissiveDead;
    };
}

/*** Render Commands ***/
namespace GTEngine
{
    // RCBegin
    class DefaultViewportRenderer_RCBegin : public RenderCommand
    {
    public:

        DefaultViewportRenderer_RCBegin();

        void Init(DefaultViewportRendererFramebuffer &framebuffer);

        void Execute();


    public:

        /// The framebuffer to make current.
        DefaultViewportRendererFramebuffer* framebuffer;

        Texture2D* depthStencil;
        Texture2D* materialDiffuse;
        Texture2D* materialEmissive;

        unsigned int viewportWidth;
        unsigned int viewportHeight;
    };


    // RCBeginLighting
    class DefaultViewportRenderer_RCBeginLighting : public RenderCommand
    {
    public:

        DefaultViewportRenderer_RCBeginLighting(DefaultViewportRendererFramebuffer &framebuffer);
        void Execute();
        void OnExecuted() { delete this; }  // TODO: Delete this. Use a cache later.


    private:

        DefaultViewportRendererFramebuffer &framebuffer;

        Texture2D* lightingDiffuseInput;
        Texture2D* lightingDiffuseOutput;
        Texture2D* lightingSpecularInput;
        Texture2D* lightingSpecularOutput;
    };


    // RCBeginLightingPass
    class DefaultViewportRenderer_RCBeginLightingPass : public RenderCommand
    {
    public:

        DefaultViewportRenderer_RCBeginLightingPass(DefaultViewportRendererFramebuffer &framebuffer, Shader* shader, const glm::vec2 &screenSize, const glm::vec3 &cameraPosition);
        void Execute();
        void OnExecuted() { delete this; }  // TODO: Delete this. Use a cache later.


    private:

        DefaultViewportRendererFramebuffer &framebuffer;

        Shader* shader;

        glm::vec2 screenSize;
        glm::vec3 cameraPosition;

        Texture2D* lightingDiffuseInput;
        Texture2D* lightingSpecularInput;

        
    };

    
    // RCEnd
    class DefaultViewportRenderer_RCEnd : public RenderCommand
    {
    public:
        
        DefaultViewportRenderer_RCEnd();

        void Init(DefaultViewportRendererFramebuffer &framebuffer);

        void Execute();


    public:

        /// The framebuffer to make current.
        DefaultViewportRendererFramebuffer* framebuffer;

        /// The combiner shader.
        Shader* combinerShader;

        /// The buffer that will eventually contain the final output.
        Texture2D* finalOutputBuffer;

        /// Input textures for the combiner.
        Texture2D* lightingDiffuse;
        Texture2D* lightingSpecular;
        Texture2D* materialDiffuse;
        Texture2D* materialEmissive;
    };


    // RCDrawMesh
    class DefaultViewportRenderer_RCDrawMesh : public RenderCommand
    {
    public:

        DefaultViewportRenderer_RCDrawMesh(VertexArray* mesh);
       ~DefaultViewportRenderer_RCDrawMesh();
        void Execute();
        void OnExecuted() { delete this; }  // TODO: Delete this. Use a cache later.

        /// Helpers for setting properties. We use MaterialProperty classes here.
        void SetParameter(const char* name, float x);
        void SetParameter(const char* name, float x, float y);
        void SetParameter(const char* name, float x, float y, float z);
        void SetParameter(const char* name, float x, float y, float z, float w);

        void SetParameter(const char* name, const glm::vec2 &v) { this->SetParameter(name, v.x, v.y); }
        void SetParameter(const char* name, const glm::vec3 &v) { this->SetParameter(name, v.x, v.y, v.z); }
        void SetParameter(const char* name, const glm::vec4 &v) { this->SetParameter(name, v.x, v.y, v.z, v.w); }

        void SetParameter(const char* name, const glm::mat2 &v);
        void SetParameter(const char* name, const glm::mat3 &v);
        void SetParameter(const char* name, const glm::mat4 &v);

        void SetParameter(const char* name, Texture2D* texture);

        void SetParameter(const char* name, ShaderParameter* parameter);

    public:

        /// The mesh to draw.
        VertexArray* mesh;

        /// A pointer to the shader to draw the node with.
        Shader* shader;

        /// The list of properties needing to be set on the shader before drawing the mesh.
        GTCore::Dictionary<char, ShaderParameter*> parameters;
    };


    // RCSwapLightingBuffers
    class DefaultViewportRenderer_RCSetLightingBuffers : public RenderCommand
    {
    public:

        DefaultViewportRenderer_RCSetLightingBuffers(DefaultViewportRendererFramebuffer &framebuffer);
        void Execute();
        void OnExecuted();  // TODO: Delete this. Use a cache later.


    private:

        DefaultViewportRendererFramebuffer &framebuffer;

        Texture2D* lightingDiffuse;
        Texture2D* lightingSpecular;
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
        void OnSwapRCBuffers();


        Texture2D* GetFinalColourOutputBuffer();
        Texture2D* GetFinalDepthStencilOutputBuffer();


    // Private Structures.
    private:

        /// Structure containing metadata for materials. There should be one of these for each material.
        struct MaterialMetadata
        {
            MaterialMetadata()
                : materialPassShader(nullptr), simpleUnlitShader(nullptr)
            {
            }

            Shader* materialPassShader;  ///< The shader for the material pass.
            Shader* simpleUnlitShader;   ///< A temporary shader that will simply render using no lighting, but full bright (not darkness).
        };


    private:

        /// Helper function for drawing a scene node.
        void RenderNode(SceneNode* node);

        /// Retrieves the metadata of a material. This should never return null. If the metadata hasn't yet been created,
        /// it will be created and then returned. Future calls will return that same object.
        MaterialMetadata & GetMaterialMetadata(Material &material);

        /// Creates and returns the shader to use with the material for simple unlit rendering.
        Shader* CreateSimpleUnlitShader(Material &material);

        /// Creates and returns the shader for use in the material pass.
        Shader* CreateMaterialPassShader(Material &material);

        /// Deletes a shader created with a Create*() function.
        void DeleteShader(Shader* shader);


        /// Performs the material pass. This pass will fill the depth buffer..
        void DoMaterialPass(const GTCore::Vector<SceneNode*> &nodes);

        /// Performs the lighting pass.
        void DoLightingPass(const GTCore::Vector<SceneNode*> &modelNodes, const GTCore::Vector<SceneNode*> &ambientLightNodes, const GTCore::Vector<SceneNode*> &directionalLightNodes, const GTCore::Vector<SceneNode*> &pointLightNodes);

        /// Performs a lighting pass for a directional light.
        void DoLightingPass_D1(SceneNode* lightNode, const GTCore::Vector<SceneNode*> &modelNodes);
        void DoLightingPass_A1(SceneNode* lightNode, const GTCore::Vector<SceneNode*> &modelNodes);
        void DoLightingPass_P1(SceneNode* lightNode, const GTCore::Vector<SceneNode*> &modelNodes);


        /// Swaps the lighting buffers in preperation for future lighting sub-passes.
        void SwapLightingBuffers();


    private:

        /// The owner viewport. This can be null, thus it needs to be a pointer.
        SceneViewport* owner;

        /// The framebuffer where the final output will be drawn to.
        DefaultViewportRendererFramebuffer framebuffer;

        /// Structure containing the different shaders.
        struct
        {
            Shader* lightingD1;
            Shader* lightingA1;
            Shader* lightingP1;
            Shader* lightingA1D1;

            Shader* combiner;

        }Shaders;

        /// The cached rendering commands.
        struct
        {
            DefaultViewportRenderer_RCBegin rcBegin[2];
            DefaultViewportRenderer_RCEnd   rcEnd[2];

        }RenderCommands;

        /// There is two of every render command. We use this index to tell us which one is the back one.
        int backRCIndex;


        /// A map containing the metadata of every material.
        GTCore::Map<Material*, MaterialMetadata*> materialMetadata;


        /// The projection matrix. This is updated at the start of each render.
        glm::mat4 projection;
        
        /// The view matrix. This is updated at the start of each render.
        glm::mat4 view;

        /// The screen size.
        glm::vec2 screenSize;

        /// This keeps track of whether or not the framebuffer needs to be resized. When ResizeFramebuffer() is called, it's not actually
        /// performed straight away. Instead we do it synchronously between frames in order to avoid synchronization issues.
        bool framebufferNeedsResize;
    };
}

#endif