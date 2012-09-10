
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


        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // Methods below should only be called internally, but need to be public for a few things.

        /// Called for a model that's visible in the currently rendering viewport.
        void __MaterialPass_Model(const SceneObject &object);

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
                    this->materialBuffer2 = new GTEngine::Texture2D(width, height, GTImage::ImageFormat_R10G10B10A2);
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





    private:

        /// Retrieves the framebuffer of the given viewport.
        DefaultSceneRenderer::Framebuffer* GetViewportFramebuffer(SceneViewport &viewport);


        /// Performs the material pass. This is always the first pass.
        void MaterialPass(Scene &scene);

        /// Renders a mesh on the material pass.
        void MaterialPass_Mesh(const Mesh &mesh, Material &material, const glm::mat4 &transform);

        /// Changes the face culling on the material pass.
        void MaterialPass_FaceCulling(bool cullFrontFaces, bool cullBackFaces);



        /// Structure containing metadata for materials. There should be one of these for each material.
        struct MaterialMetadata
        {
            MaterialMetadata()
                : materialPassShader(nullptr)
            {
            }

            Shader* materialPassShader;  ///< The shader for the material pass.
        };

        /// Retrieves the metadata of a material. This should never return null. If the metadata hasn't yet been created,
        /// it will be created and then returned. Future calls will return that same object.
        MaterialMetadata & GetMaterialMetadata(Material &material);

        /// Deletes the given material's metadata.
        void DeleteMaterialMetadata(Material &material);



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
        RCCache<RCBegin,      8>  rcBegin[2];
        RCCache<RCEnd,        8>  rcEnd[2];
        RCCache<RCBeginLayer, 8>  rcBeginLayer[2];
        RCCache<RCEndLayer,   8>  rcEndLayer[2];
        RCCache<RCDrawVA>         rcDrawVA[2];
        RCCache<RCSetFaceCulling> rcSetFaceCulling[2];

        
        // Below are various shaders for use by the renderer.
        struct
        {
            Shader* Compositor_DiffuseOnly;

            /// A cache of shaders used by materials in the material pass.
            MaterialShaderCache materialPassShaders;

        }Shaders;


        /// The list of metadata pointers that will need to be removed when the renderer is destructed. We only really keep these for clean destruction.
        GTCore::List<MaterialMetadata*> materialMetadatas;
    };
}

#endif