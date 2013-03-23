// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneRenderer_hpp_
#define __GTEngine_SceneRenderer_hpp_

#include "SceneViewport.hpp"
#include "Rendering/DrawModes.hpp"

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
            : vertexArray(nullptr), drawMode(DrawMode_Triangles), material(nullptr), transform(), flags(0)
        {
        }


        /// The vertex array containing the mesh data.
        VertexArray* vertexArray;

        /// The drawing mode to use when rendering the mesh (triangles or lines).
        DrawMode drawMode;

        /// The material to draw the vertex array with.
        Material* material;

        /// The transformation matrix to apply to the mesh.
        glm::mat4 transform;

        /// Rendering flags. The default is all unset.
        uint32_t flags;


        /// Enumerator for the various rendering options.
        enum Options
        {
            DrawLast     = (1 << 1),
            NoDepthTest  = (1 << 2),
            NoDepthWrite = (1 << 3),

            DrawHighlight = (1 << 31)           // <-- This is temporary and is only used for the editor.
        };
    };


    /// Structure representing an ambient light object.
    struct SceneRendererAmbientLight
    {
        /// The colour of the light.
        glm::vec3 colour;
    };

    /// Structure representing a directional light object.
    struct SceneRendererDirectionalLight
    {
        /// The colour of the light.
        glm::vec3 colour;

        /// The position of the light.
        glm::vec3 position;

        /// The directional of the light.
        glm::quat orientation;


        /// Helper method for calculating the forward vector.
        glm::vec3 GetForwardVector() const
        {
            return this->orientation * glm::vec3(0.0f, 0.0f, -1.0f);
        }
    };

    /// Structure representing a point light object.
    struct SceneRendererPointLight
    {
        /// The colour of the light.
        glm::vec3 colour;

        /// The position of the light.
        glm::vec3 position;

        /// The radius of the light.
        float radius;

        /// The falloff of the light.
        float falloff;
    };

    /// Structure representing a spot light object.
    struct SceneRendererSpotLight
    {
        /// The colour of the light.
        glm::vec3 colour;

        /// The position of the light.
        glm::vec3 position;

        /// The orientation of the light.
        glm::quat orientation;


        /// The length of the light.
        float length;

        /// The falloff.
        float falloff;


        /// The inner radius of the light.
        float innerAngle;

        /// The outer radius of the light.
        float outerAngle;




        /// Helper method for calculating the forward vector.
        glm::vec3 GetForwardVector() const
        {
            return this->orientation * glm::vec3(0.0f, 0.0f, -1.0f);
        }
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
        virtual void AddExternalMesh(const SceneRendererMesh &meshToAdd);

        /// Removes the given external mesh that was previously added with AddExternalMesh().
        ///
        /// @param meshToRemove [in] A reference to the mesh to remove.
        virtual void RemoveExternalMesh(const SceneRendererMesh &meshToRemove);


        




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
