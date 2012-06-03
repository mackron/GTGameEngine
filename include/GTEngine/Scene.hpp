
#ifndef __GTEngine_Scene_hpp_
#define __GTEngine_Scene_hpp_

#include "SceneNode.hpp"
#include "SceneViewport.hpp"
#include "Physics.hpp"


/// Contact test callbacks.
namespace GTEngine
{
    /// The base callback structure for doing contact tests.
    struct ContactTestCallback
    {
        short collisionGroup;
        short collisionMask;

        /// Constructor.
        ContactTestCallback(short collisionGroup = 1, short collisionMask = static_cast<short>(-1))
            : collisionGroup(collisionGroup), collisionMask(collisionMask)
        {
        }

        /// Destructor.
        virtual ~ContactTestCallback()
        {
        }

        /// Called when determining whether or not a collision needs to be made.
        ///
        /// @remarks
        ///     By default this function will do a standard group/mask check.
        virtual bool NeedsCollision(short collisionGroupIn, short collisionMaskIn, SceneNode &object)
        {
            (void)object;

            return (this->collisionMask  & collisionGroupIn) != 0 &&
                   (this->collisionGroup & collisionMaskIn)  != 0;
        }

        /// Called when two objects are touching.
        virtual void ProcessCollision(SceneNode &objectA, SceneNode &objectB, btManifoldPoint &cp) = 0;
    };


    /// The base callback structure for ray tests.
    struct RayTestCallback
    {
        short collisionGroup;
        short collisionMask;

        // rayStart and rayEnd should be set by whatever is doing the ray test.
        glm::vec3 rayStart;
        glm::vec3 rayEnd;


        /// Default constructor.
        RayTestCallback()
            : collisionGroup(static_cast<short>(-1)), collisionMask(static_cast<short>(-1)),
              rayStart(), rayEnd()
        {
        }

        /// Constructor.
        RayTestCallback(short collisionGroup, short collisionMask)
            : collisionGroup(collisionGroup), collisionMask(collisionMask),
              rayStart(), rayEnd()
        {
        }

        /// Destructor.
        virtual ~RayTestCallback()
        {
        }


        /// Called when determining whether or not a collision needs to be made.
        ///
        /// @remarks
        ///     By default this function will do a standard group/mask check.
        virtual bool NeedsCollision(short collisionGroupIn, short collisionMaskIn, SceneNode &object)
        {
            (void)object;

            return (this->collisionMask  & collisionGroupIn) != 0 &&
                   (this->collisionGroup & collisionMaskIn)  != 0;
        }

        /// Called when a collision needs to be processed.
        virtual void ProcessResult(SceneNode &object, const glm::vec3 &worldPosition, const glm::vec3 &worldNormal) = 0;
    };

    /// The ray test callback structure for keeping track of the closest object.
    struct ClosestRayTestCallback : public RayTestCallback
    {
        SceneNode* sceneNode;

        glm::vec3 worldHitPosition;
        glm::vec3 worldHitNormal;

        /// Default constructor.
        ClosestRayTestCallback()
            : RayTestCallback(), sceneNode(nullptr)
        {
        }

        /// Constructor.
        ClosestRayTestCallback(short collisionGroup, short collisionMask)
            : RayTestCallback(collisionGroup, collisionMask), sceneNode(nullptr)
        {
        }

        /// Destructor.
        virtual ~ClosestRayTestCallback()
        {
        }


        /// RayTestCallback::ProcessResult()
        virtual void ProcessResult(SceneNode& object, const glm::vec3 &worldHitPositionIn, const glm::vec3 &worldHitNormalIn)
        {
            this->sceneNode        = &object;
            this->worldHitPosition = worldHitPositionIn;
            this->worldHitNormal   = worldHitNormalIn;
        }
    };
}


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
        //virtual SceneNode* RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd) = 0;
        //virtual SceneNode* RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, RayTestResult &result) = 0;
        virtual SceneNode* RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, RayTestCallback &callback) = 0;

        /// Performs a contact test against the proximity volume of the given scene node.
        virtual void ContactTest(const SceneNode &node, ContactTestCallback &callback) = 0;



    // Occlusion
    public:

        /// Adds the applicable visible components using the given viewport's Add*Component() API.
        ///
        /// @param viewport [in] A reference to the viewport whose having visible components added.
        ///
        /// @remarks
        ///     You should not need to call this method directly. It is intended to be used internally by SceneViewport.
        virtual void AddVisibleComponents(SceneViewport &viewport) = 0;



    // Physics.
    public:

        /// Sets the scene's gravity.
        ///
        virtual void SetGravity(float x, float y, float z) = 0;
                void SetGravity(const glm::vec3 &gravity) { this->SetGravity(gravity.x, gravity.y, gravity.z); }



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