// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneRenderer_hpp_
#define __GTEngine_SceneRenderer_hpp_

#include "SceneViewport.hpp"

namespace GTEngine
{
    /// Structure representing a mesh that can be attached to a scene renderer outside of those defined by the scene.
    ///
    /// @remarks
    ///     This structure is intended to be used to allow meshes to be displayed outside of those defined by the scene. This is useful
    ///     for editting and debugging tools.
    ///
    ///     This object is made up of a single vertex array, materia and transformation matrix. Any of these properties can be updated
    ///     at any time. A renderer will reference the object directly when rendering and will handle the state of the material, etc
    ///     every frame. It is up to the user to make sure the material is valid, but the pointer to the vertex array should not be
    ///     changed, but should instead have it's data modified when needed.
    ///
    ///     Renderers are not expected to render these objects as efficiently as possible - they are not intended to be used in speed
    ///     critical situations, but instead for things like editting and debugging tools. For maximum speed, use proper scene nodes.
    struct SceneRendererMesh
    {
        SceneRendererMesh()
            : vertexArray(nullptr), material(nullptr), transform()
        {
        }


        /// The vertex array containing the mesh data.
        VertexArray* vertexArray;

        /// The material to draw the vertex array with.
        Material* material;

        /// The transformation matrix to apply to the mesh.s
        glm::mat4 transform;
    };


    /// Class responsible for the rendering a scene.
    class SceneRenderer
    {
    public:

        /// Constructor.
        SceneRenderer();

        /// Destructor.
        virtual ~SceneRenderer();


        /// Called when rendering has started for the next frame.
        ///
        /// @remarks
        ///     This will be called once for every frame.
        virtual void Begin(Scene &scene) = 0;

        /// Called when rendering has ended.
        ///
        /// @remarks
        ///     This will be called once for every frame, after rendering has finished.
        virtual void End(Scene &scene) = 0;


        /// Called when rendering has started for the given viewport.
        ///
        /// @remarks viewport [in] The viewport that has started rendering.
        ///
        /// @remarks
        ///     This will be called once for every viewport.
        virtual void RenderViewport(Scene &scene, SceneViewport &viewport) = 0;


        /// Called when a viewport needs to be added to the renderer.
        ///
        /// @param viewport [in] A reference to the viewport this renderer will need to manage.
        virtual void AddViewport(SceneViewport &viewport) = 0;

        /// Called when a viewport needs to be removed from the renderer.
        ///
        /// @param viewport [in] A reference to the viewport this renderer will stop managing.
        virtual void RemoveViewport(SceneViewport &viewport) = 0;

        /// Called after a viewport has been resized.
        ///
        /// @param viewport [in] The viewport being resized.
        ///
        /// @remarks
        ///     This function is needed so that the renderer can resize the internal framebuffers and whatnot.
        virtual void OnViewportResized(SceneViewport &viewport) = 0;


        /// Adds the given mesh to the scene.
        ///
        /// @param meshToAdd [in] A pointer to the mesh to add.
        ///
        /// @remarks
        ///     This method is intended to be used to allow meshes to be drawn outside of those defined by the scene. Use this for debugging and editting tools.
        ///     @par
        ///     It is the responsibility of the caller to manage the given object. The object can be modified directly. Renderers will handle the object on a
        ///     per-frame basis. It is not expected that renderers draw these external meshes at full speed. Instead, these meshes should be used for editting
        ///     and debugging tools.
        virtual void AddExternalMesh(SceneRendererMesh &meshToAdd)
        {
            (void)meshToAdd;
        }

        /// Removes the given external mesh that was previously added with AddExternalMesh().
        ///
        /// @param meshToRemove [in] A reference to the mesh to remove.
        virtual void RemoveExternalMesh(SceneRendererMesh &meshToRemove)
        {
            (void)meshToRemove;
        }


        




        /// Enables background colour clearing.
        ///
        /// @param r [in] The red component of the clear colour.
        /// @param g [in] The green component of the clear colour.
        /// @param b [in] The blue component of the clear colour.
        virtual void EnableBackgroundColourClearing(float r, float g, float b);

        /// Disables background colour clearing.
        virtual void DisableBackgroundColourClearing();

        /// Determines whether or not background colour clearing is enabled.
        ///
        /// @return True if colour clearing is enabled.
        virtual bool IsBackgroundColourClearingEnabled() const;


        /// Retrieves the background clear colour.
        const glm::vec3 & GetBackgroundClearColour() const;
        



    private:

        /// Keeps track of whether or not colour clearing is enabled.
        bool isColourClearingEnabled;

        /// The background clearing colour. Defaults to black.
        glm::vec3 clearColour;
    };
}

#endif
