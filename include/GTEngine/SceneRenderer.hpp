// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

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

        SceneRendererMesh(const SceneRendererMesh &other)
            : vertexArray(other.vertexArray), drawMode(other.drawMode), material(other.material), transform(other.transform), flags(other.flags)
        {
        }

        /// Destructor (warning silencer)
        virtual ~SceneRendererMesh()
        {
        }


        /// Assignment.
        SceneRendererMesh & operator=(const SceneRendererMesh &other)
        {
            if (this != &other)
            {
                this->vertexArray = other.vertexArray;
                this->drawMode    = other.drawMode;
                this->material    = other.material;
                this->transform   = other.transform;
                this->flags       = other.flags;
            }

            return *this;
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
            DrawLast        = (1 << 1),
            NoDepthTest     = (1 << 2),
            NoDepthWrite    = (1 << 3),
            NoNormalMapping = (1 << 4),

            DrawHighlight = (1 << 31)           // <-- This is temporary and is only used for the editor.
        };
    };


    /// Structure representing an ambient light object.
    struct SceneRendererAmbientLight
    {
        /// The colour of the light.
        glm::vec3 colour;


        /// Constructor.
        SceneRendererAmbientLight()
            : colour()
        {
        }

        /// Destructor (warning silencer)
        virtual ~SceneRendererAmbientLight()
        {
        }
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



        /// Constructor.
        SceneRendererDirectionalLight()
            : colour(), position(), orientation()
        {
        }

        /// Destructor (warning silencer)
        virtual ~SceneRendererDirectionalLight()
        {
        }


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


        /// Constructor.
        SceneRendererPointLight()
            : colour(), position(), radius(), falloff()
        {
        }

        /// Destructor (warning silencer)
        virtual ~SceneRendererPointLight()
        {
        }
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





        /// Constructor.
        SceneRendererSpotLight()
            : colour(), position(), orientation(), length(), falloff(), innerAngle(), outerAngle()
        {
        }

        /// Destructor (warning silencer)
        virtual ~SceneRendererSpotLight()
        {
        }


        /// Helper method for calculating the forward vector.
        glm::vec3 GetForwardVector() const
        {
            return this->orientation * glm::vec3(0.0f, 0.0f, -1.0f);
        }
    };




    /// Base class for rendering a scene.
    ///
    /// The engine supports the idea of implementing completely custom renderers. Because some renderers have properties that don't make
    /// any sense to other types of renderers, there exists a way of generically settings properties where the names of those properties
    /// are just strings. A property is set with the SetProperty() method and retrieved with the Get*Property() methods.
    ///
    /// There are some standard properties that will be used by most renderers:
    ///     - IsBackgroundClearEnabled (boolean)
    ///     - BackgroundClearColour (vec3)
    ///     - IsHDREnabled (boolean)
    ///     - IsBloomEnabled (boolean)
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



        /// Sets a rendering property.
        ///
        /// @param name  [in] The name of the property to set.
        /// @param value [in] The value of the property.
        ///
        /// @remarks
        ///     Because the engine is designed to allow custom renderers, sometimes some properties will be unique to that particular
        ///     renderer. To make it possible to set properties arbitrarily, we use a key/value type system, where the key is just a
        ///     string and the value is any basic type (string, number, boolean, vector2/3/4).
        virtual void SetProperty(const char* name, const char* value);
        virtual void SetProperty(const char* name, int value);
        virtual void SetProperty(const char* name, float value);
        virtual void SetProperty(const char* name, bool value);
        virtual void SetProperty(const char* name, const glm::vec2 &value);
        virtual void SetProperty(const char* name, const glm::vec3 &value);
        virtual void SetProperty(const char* name, const glm::vec4 &value);

        /// Retrieves the value of a generic property.
        ///
        /// @param name [in] The name of the property to retrieve.
        virtual GTLib::String GetStringProperty(const char* name) const;
        virtual int            GetIntegerProperty(const char* name) const;
        virtual float          GetFloatProperty(const char* name) const;
        virtual bool           GetBooleanProperty(const char* name) const;
        virtual glm::vec2      GetVector2Property(const char* name) const;
        virtual glm::vec3      GetVector3Property(const char* name) const;
        virtual glm::vec4      GetVector4Property(const char* name) const;


        /// Enables background colour clearing.
        ///
        /// @param r [in] The red component of the clear colour.
        /// @param g [in] The green component of the clear colour.
        /// @param b [in] The blue component of the clear colour.
        virtual void EnableBackgroundColourClearing(float r, float g, float b);
                void EnableBackgroundColourClearing(const glm::vec3 &colour) { this->EnableBackgroundColourClearing(colour.x, colour.y, colour.z); }

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
