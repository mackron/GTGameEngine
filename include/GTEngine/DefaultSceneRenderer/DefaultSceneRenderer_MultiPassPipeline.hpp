// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_MultiPassPipeline_hpp_
#define __GTEngine_DefaultSceneRenderer_MultiPassPipeline_hpp_

#include <GTCore/Map.hpp>
#include <GTCore/Vector.hpp>
#include <GTEngine/Material.hpp>

namespace GTEngine
{
    class  DefaultSceneRenderer;
    class  DefaultSceneRenderer_VisibilityProcessor;
    struct DefaultSceneRendererFramebuffer;
    struct DefaultSceneRendererMesh;

    class DefaultSceneRenderer_MultiPassPipeline
    {
    public:

        /// Constructor.
        DefaultSceneRenderer_MultiPassPipeline(DefaultSceneRenderer &renderer, DefaultSceneRendererFramebuffer &viewportFramebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, bool splitShadowLights);

        /// Destructor.
        ~DefaultSceneRenderer_MultiPassPipeline();


        /// Executes the pipeline.
        void Execute();



    private:

        /// Binds the objects that should be rendered during the next rendering operations.
        ///
        /// @param opaqueObjects      [in] A pointer to the list of opaque objects.
        /// @param transparentObjects [in] A pointer to the list of blended transparent objects.
        void BindObjects(const GTCore::Map<const MaterialDefinition*, GTCore::Vector<DefaultSceneRendererMesh>*>* opaqueObjects, const GTCore::Vector<DefaultSceneRendererMesh>* transparentObjects);


        /// Performs the opaque pass on the currently bound objects.
        void OpaquePass();

        /// Performs the transparent pass on the currently bound objects.
        void TransparentPass();


        /// Performs a depth-only pass.
        void DepthPass();

        /// Performs the lighting pass.
        void OpaqueLightingPass();

        /// Performs the material pass.
        void OpaqueMaterialPass();

        

        /// Subdivides the given light group into smaller groups for rendering.
        ///
        /// @param sourceLightGroup    [in ] A reference to the light group to subdivide.
        /// @param lightGroupsOut      [out] A reference to the container that will receive the subdivided groups.
        /// @param options             [in ] A bitfield containing options to control how the lights are subdivided.
        ///
        /// @remarks
        ///     This will always return at least 1 light group. If there are no lights, it will just be an empty group.
        void SubdivideLightGroup(const DefaultSceneRenderer_LightGroup* sourceLightGroup, GTCore::Vector<DefaultSceneRenderer_LightGroup> &lightGroupsOut, uint32_t options);


        ///////////////////////////////////////////
        // Rendering Helpers.

        /// Changes the framebuffer state to that needed for lighting.
        void SwitchToLightingFramebuffer();

        /// Enables lighting blending.
        void EnableLightingBlending();

        /// Enables lighting depth testing.
        void EnableLightingDepthTesting();

        /// Changes the state of the renderer in order to render lighting.
        ///
        /// @remarks
        ///     This will include a framebuffer switch, so only use this where convenient.
        void EnableLightingState();

        /// Helper for rendering the given mesh using the given light group.
        ///
        /// @param mesh        [in] A reference to the mesh to draw.
        /// @param lightGroup  [in] A reference to the light group.
        /// @param shaderFlags [in] The shader flags.
        void RenderMesh(const DefaultSceneRendererMesh &mesh, const DefaultSceneRenderer_LightGroup &lightGroup, uint32_t shaderFlags);

        /// Helper for rendering the lighting information of a mesh.
        ///
        /// @param mesh        [in] A reference to the mesh whose lighting is being drawn.
        /// @param lightGroups [in] A reference to the list of light groups.
        void RenderMeshLighting(const DefaultSceneRendererMesh &mesh, const GTCore::Vector<DefaultSceneRenderer_LightGroup> &lightGroups);

        /// Helper for rendering the highlight effect of the given mesh.
        ///
        /// @remarks
        ///     This requires that blending be enabled explicitly before calling this.
        void RenderMeshHighlight(const DefaultSceneRendererMesh &mesh);


        /// Helper for drawing the background texture for refraction.
        void RenderRefractionBackgroundTexture();


        /// Performs a background clear of the main colour buffer if it hasn't already been cleared.
        void TryClearBackground();


        /// Retrieves a reference to the main light group which will contain every light.
        const DefaultSceneRenderer_LightGroup & GetMainLightGroup() const;



    private:

        /// A reference to the owner renderer.
        DefaultSceneRenderer &renderer;

        /// A reference to the viewport framebuffer.
        DefaultSceneRendererFramebuffer &viewportFramebuffer;

        /// A reference to the visibility processor containing the visible objects.
        const DefaultSceneRenderer_VisibilityProcessor &visibleObjects;

        /// Whether or not the shadow-casting lights need to be split when rendering (not grouped).
        bool splitShadowLights;


        /// A pointer to the currently bound opaque objects.
        const GTCore::Map<const MaterialDefinition*, GTCore::Vector<DefaultSceneRendererMesh>*>* opaqueObjects;
        
        /// A pointer to the currently bound blended transparent objects.
        const GTCore::Vector<DefaultSceneRendererMesh>* transparentObjects;


        /// Keeps track of whether or not the background has been cleared.
        bool hasBackgroundBeenCleared;


        /// Keeps track of whether or not the main light group has been generated.
        mutable bool hasGeneratedMainLightGroup;

        /// The main light group containing every visible light. This will be empty by default until GetMainLightGroup()
        /// is called.
        mutable DefaultSceneRenderer_LightGroup mainLightGroup;



        /// Enumerator containing flags for light group subdivision options.
        enum LightSubdivionFlags
        {
            NoAmbientLights     = (1 << 1),     // <-- Exclude ambient lights.
            NoShadowLights      = (1 << 2),     // <-- Exclude shadow-casting lights.
            ConvertShadowLights = (1 << 3)      // <-- Convert shadow-casting lights to non-shadow-casting lights.
        };



    private:    // No copying.
        DefaultSceneRenderer_MultiPassPipeline(const DefaultSceneRenderer_MultiPassPipeline &);
        DefaultSceneRenderer_MultiPassPipeline & operator=(const DefaultSceneRenderer_MultiPassPipeline &);
    };
}

#endif