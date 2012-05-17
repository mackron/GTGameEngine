
#ifndef __GTEngine_Scene_hpp_
#define __GTEngine_Scene_hpp_

#include "SceneNode.hpp"
#include "SceneViewport.hpp"

namespace GTEngine
{
    /**
    *   \brief  Base class for scenes.
    *
    *   A scene is made up of a bunch of scene nodes organised in a particular way, depending on the implementation.
    */
    class Scene
    {
    public:

        Scene();

        /**
        *   \brief  Destructor.
        *
        *   \remarks
        *       The destructor of derived classes should NOT delete attached scene nodes. Instead, they should simply be removed
        *       from the scene.
        */
        virtual ~Scene();


        /**
        *   \brief  Adds a scene node to the scene. See remarks.
        *
        *   \remarks
        *       The scene node will not be attached to a logical parent. Some scenes may use a root node as the parent of all nodes, in which
        *       case it will be made a child of the root node.
        */
        virtual void AddSceneNode(SceneNode &node) = 0;
        
        /**
        *   \brief            Removes a scene node from the scene. See remarks.
        *   \param  node [in] The scene node to remove.
        *
        *   \remarks
        *       The scene node will be orphaned. You must re-set the parent if required.
        */
        virtual void RemoveSceneNode(SceneNode &node) = 0;

        
        /**
        *   \brief              Updates the scene.
        *   \param  camera [in] A pointer to the node that the scene will be drawn from. Must have a camera component attached.
        *   \param  dt     [in] The delta time in seconds. The time since the last frame update. Used for time-based movement.
        */
        virtual void Update(double deltaTimeInSeconds) = 0;

        /**
        *   \brief  Retrieves a pointer to the first occurance of the scene node with the given name. Returns nullptr if none exist.
        */
        virtual SceneNode * FindFirstNode(const char *name) = 0;

        /**
        *   \brief  Retrieves a pointer to the first occurance of the scene node with the given component. Returns nullptr is none exist.
        */
        virtual SceneNode * FindFirstNodeWithComponent(const char *componentName) = 0;

        template <typename T>
        SceneNode * FindFirstNodeWithComponent()
        {
            return this->FindFirstNodeWithComponent(T::Name);
        }


        /// Adds a viewport to the scene.
        /// @param viewport [in] A reference to the viewport to add to the scene.
        virtual void AddViewport(SceneViewport &viewport) = 0;

        /// Removes a viewport from the scene.
        /// @param viewport [in] A reference to the viewport to remove from the scene.
        virtual void RemoveViewport(SceneViewport &viewport) = 0;


    // Picking.
    public:

        /// Picks the closest scene node based on the given ray.
        /// @param rayStart [in] The start of the ray.
        /// @param rayEnd   [in] The end of the ray.
        virtual SceneNode* PickSceneNode(const glm::vec3 &rayStart, const glm::vec3 &rayEnd) = 0;


        /// Performs a ray test on the scene nodes against their dynamics components.
        virtual SceneNode* RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd) = 0;


    // Occlusion
    public:

        /// Adds the applicable visible components using the given viewport's Add*Component() API.
        ///
        /// @param viewport [in] A reference to the viewport whose having visible components added.
        ///
        /// @remarks
        ///     You should not need to call this method directly. It is intended to be used internally by SceneViewport.
        virtual void AddVisibleComponents(SceneViewport &viewport) = 0;



    // Non-virtual functions.
    public:

        /**
        *   \brief  Pauses the scene.
        *
        *   \remarks
        *       Calling Update() while the scene is paused will still draw each scene node, but it will not step scene nodes or animations.
        */
        void Pause();

        /**
        *   \brief  Resumes the scene.
        */
        void Resume();

        /**
        *   \brief  Determines whether or not the scene is paused.
        */
        bool IsPaused() const { return this->paused; }


    public: // Events.

        /**
        *   \brief  Called after a scene node is added to the scene. See remarks.
        *
        *   \remarks
        *       This function is called when a scene node that is already part of this scene has a child attached to it.
        */
        virtual void OnSceneNodeAdded(SceneNode& node);

        /**
        *   \brief  Called after a scene node is removed from the scene.
        */
        virtual void OnSceneNodeRemoved(SceneNode& node);

        /// Called when a scene node is moved, rotated or both. This is not called for scaling. Use OnSceneNodeScale() that.
        /// @param node [in] A reference to the node that has been transformed.
        virtual void OnSceneNodeTransform(SceneNode &node);

        virtual void OnSceneNodeScale(SceneNode& node);
        virtual void OnSceneNodeStaticChanged(SceneNode& node);
        virtual void OnSceneNodeVisibleChanged(SceneNode& node);
        virtual void OnSceneNodeComponentAttached(SceneNode& node, Component& component);
        virtual void OnSceneNodeComponentDetached(SceneNode& node, Component& component);

        /// Sometimes when a component changes, the scene needs to update some information. An example is when the attenuation of a point light
        /// changes, the scene may need to update culling information.
        virtual void OnSceneNodeComponentChanged(SceneNode& node, Component& component);

        virtual void OnPause();
        virtual void OnResume();


    protected:

        /// Whether or not the scene is paused.
        bool paused;


    private:    // No copying.
        Scene(const Scene &);
        Scene & operator=(const Scene &);
    };

}

#endif