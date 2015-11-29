// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_SceneViewport_hpp_
#define __GTEngine_SceneViewport_hpp_

#include <GTEngine/Core/Vector.hpp>
#include <GTEngine/Components/LightComponent.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include "Math.hpp"

namespace GTEngine
{
    class Scene;
    class SceneNode;
    class Framebuffer;
    class Texture2D;


    namespace ViewportLayer
    {
        static const int Background = 0;
        static const int Main       = 1;
    }

    static const int ViewportLayerCount = 2;


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

        /// Default constructor.
        ///
        /// @remarks
        ///     The viewport will default to 0,0.
        SceneViewport();

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
        void SetCameraNode(SceneNode* cameraNode, int layer = ViewportLayer::Main);
        void SetCameraNode(SceneNode &cameraNode, int layer = ViewportLayer::Main) { this->SetCameraNode(&cameraNode, layer); }

        /// Retrieves a pointer to the camera node. Can return null if a camera hasn't been set, or SetCameraNode() was set with an argument of null.
        ///
        /// @param layer [in] The index of the layer whose camera is being retrieved.
              SceneNode* GetCameraNode(int layer = ViewportLayer::Main);
        const SceneNode* GetCameraNode(int layer = ViewportLayer::Main) const;


        /// Retrieves the width of the viewport.
        /// @return The width of the viewport.
        unsigned int GetWidth() const;

        /// Retrieves the height of the viewport.
        /// @return The height of the viewport.
        unsigned int GetHeight() const;

        /// Resizes the viewport.
        void Resize(unsigned int newWidth, unsigned int newHeight);


        /// Retrieves the colour buffer of the viewport.
        Texture2D* GetColourBuffer() { return m_colourBuffer; }

        /// Sets the colour buffer of the viewport.
        ///
        /// @remarks
        ///     This should only need to be used by renderers. This is just a simple setter.
        void SetColourBuffer(Texture2D* colourBuffer) { m_colourBuffer = colourBuffer; }



    // Picking.
    public:

        /// Calculates a ray that can be used for picking.
        /// @param  x       [in]  The x position on the viewport the ray should start at.
        /// @param  y       [in]  The y position on the viewport the ray should start at.
        /// @param  rayNear [out] The end of the ray that is closest to the viewer.
        /// @param  rayFar  [out] The end of the ray that is farthest from the viewport.
        void CalculatePickingRay(int x, int y, glm::vec3 &rayNear, glm::vec3 &rayFar, int layer = ViewportLayer::Main);


    // Misc stuff.
    public:

        /// Projects a 3D point to window coordinates based on the viewport.
        ///
        /// @param position [in] The point in 3D space being projected.
        glm::vec3 Project(const glm::vec3 &position, int layer = ViewportLayer::Main);

        /// Unprojects a 2D point into 3D space.
        ///
        /// @param position [in] The point in 2D space being unprojected.
        glm::vec3 Unproject(const glm::vec3 &position, int layer = ViewportLayer::Main);

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
        glm::mat4 GetMVPMatrix(int layer = ViewportLayer::Main) const;




    private:

        /// The scene this viewport is attached to.
        Scene* scene;

        /// The camera nodes for each layer.
        SceneNode* cameraNodes[ViewportLayerCount];


        /// The width of the viewport.
        unsigned int width;

        /// The height of the viewport.
        unsigned int height;


        /// The texture that will hold the colour data of the viewport. Retrieve this with GetColourBuffer().
        Texture2D* m_colourBuffer;


    private: // No copying.
        SceneViewport(const SceneViewport &);
        SceneViewport & operator=(const SceneViewport &);
    };
}

#endif