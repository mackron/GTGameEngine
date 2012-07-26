
#ifndef __GTEngine_SceneViewport_hpp_
#define __GTEngine_SceneViewport_hpp_

#include <GTCore/Vector.hpp>
#include <GTEngine/Components/LightComponent.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/Components/SpriteComponent.hpp>
#include "Math.hpp"

namespace GTEngine
{
    class Scene;
    class SceneNode;
    class ViewportRenderer;
    class Framebuffer;
    class Texture2D;

    /// Base class representing a viewport that a scene renders to.
    ///
    /// The scene the viewport is attached to can be changed dynamically. Do NOT use SceneViewport::SetScene(). Instead,
    /// attach/detach the viewport with Scene::AttachViewport()/Scene::DetachViewport().
    ///
    /// In order to render the viewport, a ViewportRenderer must be attached. Use SetRenderer() to set the renderer to
    /// use when rendering the viewport.
    class SceneViewport
    {
    public:

        /// Constructor.
        SceneViewport(unsigned int width, unsigned int height);

        /// Destructor.
        virtual ~SceneViewport();



    public:

        /// Sets the scene this viewport is attached to. Do not use this publically. Instead, use Scene::AttachViewport()/Scene::DetachViewport().
        ///
        /// @param scene [in] A pointer to the new scene. Can be null, it which case the viewport is not attached to any scene.
        void SetScene(Scene *scene);

        /// Retrieves a pointer to the scene this viewport is attached to. This can return null if it is not attached to any scene.
        Scene* GetScene();


        /// Sets the scene node to act as the camera.
        ///
        /// @param cameraNode [in] A pointer to the camera node. Can be null, in which case nothing is rendered.
        /// @param layer      [in] The index of the layer to attach this camera to.
        ///
        /// @remarks
        ///     Remove the camera of a layer by setting <cameraNode> to nullptr.
        void SetCameraNode(SceneNode* cameraNode, int layer = 0);

        /// Retrieves a pointer to the camera node. Can return null if a camera hasn't been set, or SetCameraNode() was set with an argument of null.
        ///
        /// @param layer [in] The index of the layer whose camera is being retrieved.
              SceneNode* GetCameraNode(int layer = 0);
        const SceneNode* GetCameraNode(int layer = 0) const;

        
        /// Sets the renderer.
        void SetRenderer(ViewportRenderer* renderer);

        /// Retrieves a pointer to the renderer.
        ViewportRenderer* GetRenderer();


        /// Retrieves the width of the viewport.
        /// @return The width of the viewport.
        unsigned int GetWidth() const;

        /// Retrieves the height of the viewport.
        /// @return The height of the viewport.
        unsigned int GetHeight() const;

        /// Resizes the viewport.
        void Resize(unsigned int newWidth, unsigned int newHeight);



        /// Retrieves a reference to the cache of model components.
        GTCore::Vector<ModelComponent*> & GetModelComponents() { return this->modelComponents; }

        /// Retrieves a reference to the cache of ambient light components.
        GTCore::Vector<AmbientLightComponent*> & GetAmbientLightComponents() { return this->ambientLightComponents; }

        /// Retrieves a reference to the cache of directional light components.
        GTCore::Vector<DirectionalLightComponent*> & GetDirectionalLightComponents() { return this->directionalLightComponents; }

        /// Retrieves a reference to the cache of point light components.
        GTCore::Vector<PointLightComponent*> & GetPointLightComponents() { return this->pointLightComponents; }

        /// Retrieves a reference to the cache of spot light components.
        GTCore::Vector<SpotLightComponent*> & GetSpotLightComponents() { return this->spotLightComponents; }

        


        /// Adds a model component.
        void AddModelComponent(ModelComponent &component);

        /// Adds an ambient light component.
        void AddAmbientLightComponent(AmbientLightComponent &component);

        /// Adds a directional light component.
        void AddDirectionalLightComponent(DirectionalLightComponent &component);

        /// Adds a point light component.
        void AddPointLightComponent(PointLightComponent &component);

        /// Adds a spot light component.
        void AddSpotLightComponent(SpotLightComponent &component);



        /// Draws the content of the viewport using the attached renderer. If no renderer is attached, this will do nothing.
        void Render();

        /// Retrieves the framebuffer that the renderer is drawing to. An application will need to retrieve this in order to
        /// show the rendering result. This will return null if there is no renderer.
        Framebuffer* GetFramebuffer();


        /// Retrieves the final colour output buffer. Can return null if there is no renderer.
        Texture2D* GetColourOutputBuffer();

        /// Retrieves the final depth/stencil output buffer. Can return null if there is no renderer.
        Texture2D* GetDepthStencilOutputBuffer();


    // Picking.
    public:

        /// Calculates a ray that can be used for picking.
        /// @param  x       [in]  The x position on the viewport the ray should start at.
        /// @param  y       [in]  The y position on the viewport the ray should start at.
        /// @param  rayNear [out] The end of the ray that is closest to the viewer.
        /// @param  rayFar  [out] The end of the ray that is farthest from the viewport.
        void CalculatePickingRay(int x, int y, glm::vec3 &rayNear, glm::vec3 &rayFar, int layer = 0);


    // Misc stuff.
    public:

        /// Projects a 3D point to window coordinates based on the viewport.
        /// @param position [in] The point in 3D space being projected.
        glm::vec3 Project(const glm::vec3 &position, int layer = 0);

        /// Unprojects a 2D point into 3D space.
        /// @param position [in] The point in 2D space being unprojected.
        glm::vec3 Unproject(const glm::vec3 &position, int layer = 0);

        /// Retrieves a 2D projection matrix for this viewport.
        ///
        /// @param yDown [in] Whether or not the +y axis should move from top to bottom. Defaults to false (+y up).
        ///
        /// @return A new 2D projection matrix for this viewport.
        ///
        /// @remarks
        ///     This method does a full construction of the matrix. Do not assume it does a direct access.
        glm::mat4 Get2DProjectionMatrix(bool yDown = false) const;

        /// Retrieves the model-view-projection matrix used by this viewport and it's current camera.
        glm::mat4 GetMVPMatrix(int layer = 0) const;


    private:

        /// Updates the internal rendering data in preparation for rendering.
        void UpdateRenderingData();

        /// Clears the internal rendering data.
        void ClearRenderingData();


    private:

        /// The scene this viewport is attached to.
        Scene* scene;

        /// The map for mapping a layer to a camera.
        GTCore::Map<int, SceneNode*> cameraNodes;


        /// The renderer to use when drawing this viewport.
        ViewportRenderer* renderer;

        /// The width of the viewport.
        unsigned int width;

        /// The height of the viewport.
        unsigned int height;


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



    private: // No copying.
        SceneViewport(const SceneViewport &);
        SceneViewport & operator=(const SceneViewport &);
    };
}

#endif