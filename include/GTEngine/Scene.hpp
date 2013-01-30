
#ifndef __GTEngine_Scene_hpp_
#define __GTEngine_Scene_hpp_

#include "SceneNode.hpp"
#include "SceneViewport.hpp"
#include "SceneEventHandler.hpp"
#include "Physics.hpp"
#include "DefaultSceneUpdateManager.hpp"
#include "DefaultScenePhysicsManager.hpp"
#include "DefaultSceneCullingManager.hpp"
#include "DefaultSceneRenderer.hpp"
#include "NavigationMesh.hpp"
#include "Serialization.hpp"
#include "SceneNodeMap.hpp"
#include "SceneStateStack.hpp"


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


        // Whether or not a hit was detected.
        bool hasCollision;


        /// Default constructor.
        RayTestCallback()
            : collisionGroup(static_cast<short>(-1)), collisionMask(static_cast<short>(-1)),
              rayStart(), rayEnd(),
              hasCollision(false)
        {
        }

        /// Constructor.
        RayTestCallback(short collisionGroup, short collisionMask)
            : collisionGroup(collisionGroup), collisionMask(collisionMask),
              rayStart(), rayEnd(),
              hasCollision(false)
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
        virtual bool NeedsCollision(short collisionGroupIn, short collisionMaskIn, SceneNode &object) const
        {
            (void)object;

            return (this->collisionMask  & collisionGroupIn) != 0 &&
                   (this->collisionGroup & collisionMaskIn)  != 0;
        }

        /// Called when a collision needs to be processed.
        virtual void ProcessResult(SceneNode &object, const glm::vec3 &worldPosition, const glm::vec3 &worldNormal)
        {
            (void)object;
            (void)worldPosition;
            (void)worldNormal;
        }
    };

    /// The ray test callback structure for keeping track of the closest object.
    struct ClosestRayTestCallback : public RayTestCallback
    {
        SceneNode* sceneNode;

        glm::vec3 worldHitPosition;
        glm::vec3 worldHitNormal;

        /// Default constructor.
        ClosestRayTestCallback()
            : RayTestCallback(), sceneNode(nullptr),
              worldHitPosition(), worldHitNormal()
        {
        }

        /// Constructor.
        ClosestRayTestCallback(short collisionGroup, short collisionMask)
            : RayTestCallback(collisionGroup, collisionMask), sceneNode(nullptr),
              worldHitPosition(), worldHitNormal()
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


    private:    // No copying.
        ClosestRayTestCallback(const ClosestRayTestCallback &);
        ClosestRayTestCallback & operator=(const ClosestRayTestCallback &);
    };

    /// The ray test callback structure for checking the closest object, not includeing the given scene node.
    struct ClosestRayExceptMeTestCallback : public ClosestRayTestCallback
    {
        SceneNode* excludedNode;

        ClosestRayExceptMeTestCallback()
            : ClosestRayTestCallback(), excludedNode(nullptr)
        {
        }

        ClosestRayExceptMeTestCallback(short collisionGroup, short collisionMask)
            : ClosestRayTestCallback(collisionGroup, collisionMask), excludedNode(nullptr)
        {
        }

        ClosestRayExceptMeTestCallback(short collisionGroup, short collisionMask, SceneNode &excluded)
            : ClosestRayTestCallback(collisionGroup, collisionMask), excludedNode(&excluded)
        {
        }

        /// RayTestCallback::NeedsCollision().
        virtual bool NeedsCollision(short collisionGroupIn, short collisionMaskIn, SceneNode &object) const
        {
            if (this->excludedNode == &object)
            {
                return false;
            }

            return ClosestRayTestCallback::NeedsCollision(collisionGroupIn, collisionMaskIn, object);
        }


    private:    // No copying.
        ClosestRayExceptMeTestCallback(const ClosestRayExceptMeTestCallback &);
        ClosestRayExceptMeTestCallback & operator=(const ClosestRayExceptMeTestCallback &);
    };
}


namespace GTEngine
{
    // TODO: Document the state stack.

    /// A class representing a scene.
    ///
    /// A Scene object is actually mostly just a container around managers. The managers are what perform most of the real functionality of the scene.
    ///
    /// Managers can only be set via the constructor and can not be changed afterwards. Below is a brief description of the roles of each manager.
    ///
    /// \section Update Manager
    /// The update manager is responsible for handling the updating/stepping of scene objects each frame.
    /// \endsection
    ///
    /// \section Physics Manager
    /// The physics manager is responsible for handling the physics of a scene.
    /// \endsection
    ///
    /// \section Culling Manager
    /// The culling manager is responsible for handling any culling required by the scene. This includes occlusion and frustum culling, and also things
    /// like culling objects that are outside the bounds of a light.
    /// \endsection
    class Scene
    {
    public:

        /// Default constructor.
        ///
        /// @remarks
        ///     This constructor will use default managers.
        Scene();

        /// Constructor.
        ///
        /// @param updateManager  [in] A reference to the update manager that will manage the updating of scene objects.
        /// @param physicsManager [in] A reference to the physics manager that will manage the physics of the scene.
        /// @param cullingManager [in] A reference to the culling manager that will manage the culling of objects in the scene.
        Scene(SceneUpdateManager &updateManager, ScenePhysicsManager &physicsManager, SceneCullingManager &cullingManager);

        /// Destructor.
        ///
        /// @remarks
        ///     The destructor of derived classes will <b>not</b> delete attached objects.
        virtual ~Scene();


        /// Adds the given object to the scene.
        ///
        /// @param object [in] The object to add to the scene.
        void AddObject(SceneObject &object);

        /// Removes the given object from the scene.
        ///
        /// @param object [in] A reference to the object to remove from the scene.
        void RemoveObject(SceneObject &object);

        /// Removes every object from the scene.
        void RemoveAllObjects();


        /// Adds a scene node to the scene.
        ///
        /// @param node [in] A reference to the scene node to add to the scene.
        void AddSceneNode(SceneNode &node);

        /// Removes a scene node from the scene.
        ///
        /// @param node [in] A reference to the node to remove.
        void RemoveSceneNode(SceneNode &node);

        /// Removes a scene node by it's ID.
        ///
        /// @param sceneNodeID [in] The ID of the scene node to remove.
        void RemoveSceneNodeByID(uint64_t sceneNodeID);

        /// Creates a new scene node from the given deserializer.
        ///
        /// @param deserializer [in] A reference to the deserializer to create the scene node from.
        ///
        /// @return A pointer to the new scene node, or null if an error occurs.
        ///
        /// @remarks
        ///     If a scene node of the same ID already exists, this will return nullptr.
        SceneNode* CreateNewSceneNode(GTCore::Deserializer &deserializer);
        


        /// Refreshes the given scene object.
        ///
        /// @param object [in] A reference to the object to refresh.
        ///
        /// @remarks
        ///     Internally, refreshing an object will simply remove and re-add the object from/to the scene, which will then force updates to
        ///     the internal structures (update manager, physics manager, etc).
        void RefreshObject(SceneObject &object);


        /// Retrieves a scene node by it's ID.
              SceneNode* GetSceneNodeByID(uint64_t sceneNodeID);
        const SceneNode* GetSceneNodeByID(uint64_t sceneNodeID) const;


        /// Pauses the scene.
        ///
        /// @remarks
        ///     Calling Update() while the scene is paused will still draw each scene node, but it will not step scene nodes or animations.
        void Pause();

        /// Resumes the scene.
        void Resume();

        /// Determines whether or not the scene is paused.
        ///
        /// @return True if the scene is paused; false otherwise.
        bool IsPaused() const { return this->paused; }


        /// Steps the scene.
        ///
        /// @param deltaTimeInSeconds [in] The time in seconds to step the scene by. This will usually be the time between frames, but it can be adjusted to speed up/slow down time.
        void Update(double deltaTimeInSeconds);


        /// Retrieves a pointer to the first occurance of the scene node with the given name. Returns nullptr if none exist.
        SceneNode* FindFirstNode(const char *name);

        /// Retrieves a pointer to the first occurance of the scene node with the given component. Returns nullptr is none exist.
        SceneNode* FindFirstNodeWithComponent(const char *componentName);

        template <typename T>
        SceneNode* FindFirstNodeWithComponent()
        {
            return this->FindFirstNodeWithComponent(T::Name);
        }


        /// Adds a viewport to the scene.
        ///
        /// @param viewport [in] A reference to the viewport to add to the scene.
        void AddViewport(SceneViewport &viewport);

        /// Removes a viewport from the scene.
        ///
        /// @param viewport [in] A reference to the viewport to remove from the scene.
        void RemoveViewport(SceneViewport &viewport);

        /// Removes every viewport.
        void RemoveAllViewports();


        /// Retrieves the AABB of the scene.
        void GetAABB(glm::vec3 &min, glm::vec3 &max) const;


        /// Retrieves a reference to the renderer.
              SceneRenderer & GetRenderer()       { assert(this->renderer != nullptr); return *this->renderer; }
        const SceneRenderer & GetRenderer() const { assert(this->renderer != nullptr); return *this->renderer; }

        /// Sets the renderer.
        void SetRenderer(SceneRenderer &newRenderer);




        /// Retrieves a reference to the update manager.
              SceneUpdateManager & GetUpdateManager()       { return this->updateManager; }
        const SceneUpdateManager & GetUpdateManager() const { return this->updateManager; }

        /// Retrieves a reference to the physics manager.
              ScenePhysicsManager & GetPhysicsManager()       { return this->physicsManager; }
        const ScenePhysicsManager & GetPhysicsManager() const { return this->physicsManager; }

        /// Retrieves a reference to the culling manager.
              SceneCullingManager & GetCullingManager()       { return this->cullingManager; }
        const SceneCullingManager & GetCullingManager() const { return this->cullingManager; }



        /// Attaches an event handler.
        ///
        /// @param eventHandler [in] A refernece to the event handler to attach.
        ///
        /// @remarks
        ///     If the event handler is already attached, this will do nothing.
        ///     @par
        ///     Detach the event handler with DetachEventHandler() before deleting it.
        void AttachEventHandler(SceneEventHandler &eventHandler);

        /// Detaches an event handler.
        ///
        /// @param eventHandler [in] A reference to the event handler to detach.
        void DetachEventHandler(SceneEventHandler &eventHandler);

        


        ////////////////////////////////////////////
        // State Stack

        /// Creates a new branch on the state stack at the current frame, with the current branch as it's parent.
        ///
        /// @return The ID of the new branch.
        uint32_t CreateStateStackBranch();

        /// Switches to the branch of the given ID.
        ///
        /// @param branchID [in] The ID of the branch to switch to.
        ///
        /// @return True if the branch was switched; false otherwise. This will only fail if the branch does not exist.
        bool SwitchStateStackBranch(uint32_t branchID);

        /// Determines whether or not staging of the state stack is enabled.
        ///
        /// @remarks
        ///     This will return true if there is at least one commit on the master branch. Thus, it is assumed that a scene
        ///     will want to use the state stack if there is an initial commit. If there is no initial commit, it is assumed
        ///     that the state stack is not being used and so staging is not performed.
        bool IsStateStackStagingEnabled() const;

        /// Enable state stack staging.
        ///
        /// @remarks
        ///     Note that an initial commit must also have been performed for state stack staging to be enabled.
        void EnableStateStackStaging();

        /// Disables state stack staging.
        void DisableStateStackStaging();

        /// Performs a commit on the state stack.
        void CommitStateStackFrame();

        /// Retrieves the number of stack frames on the current branch.
        ///
        /// @remarks
        ///     The number of stack frames on the current branch.
        size_t GetStateStackFrameCount() const;

        /// Moves the current frame by the given amount.
        ///
        /// @param step [in] The amount to seek by. Can be positive or negative.
        ///
        /// @remarks
        ///     This will also update the scene to that of the new state frame.
        void SeekStateStack(int step);

        /// Applies the current state as defined in the state stack to the scene.
        void ApplyStateStack();




    // Collision Tests.
    public:

        /// Performs a ray test on the scene nodes against their dynamics components.
        ///
        /// @param rayStart [in     ] The start position of the ray.
        /// @param rayEnd   [in     ] The end position of the ray.
        /// @param callback [in, out] A reference to the callback structure that will receive the results of the ray test.
        ///
        /// @return A pointer to the closest scene node to 'rayStart', or null if there were no contacts.
        SceneNode* RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, RayTestCallback &callback);
        SceneNode* RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, short collisionGroup = -1, short collisionMask = -1);

        /// Performs a contact test against the proximity volume of the given scene node.
        ///
        /// @param node     [in]      The node to perform the contact test against.
        /// @param callback [in, out] A reference to the callback structure that will receive the results of the contact test.
        void ContactTest(const SceneNode &node, ContactTestCallback &callback);



    // Occlusion
    public:

        /// Queries the visible objects based on the given projection matrix.
        ///
        /// @param mvp      [in] The projection matrix whose visible objects are being retrieved.
        /// @param callback [in] The callback that will receive the visible objects.
        void QueryVisibleObjects(const glm::mat4 &mvp, SceneCullingManager::VisibilityCallback &callback);




    // Physics.
    public:

        /// Sets the scene's gravity.
        void SetGravity(float x, float y, float z);
        void SetGravity(const glm::vec3 &gravity) { this->SetGravity(gravity.x, gravity.y, gravity.z); }

        /// Retrieves the scene's gravity.
        void GetGravity(float &x, float &y, float &z) const;

        glm::vec3 GetGravity() const
        {
            glm::vec3 gravity;
            this->GetGravity(gravity.x, gravity.y, gravity.z);

            return gravity;
        }

        void GetGravity(glm::vec3 &gravity) const
        {
            this->GetGravity(gravity.x, gravity.y, gravity.z);
        }


    // A.I.
    public:

        /// Sets the walkable height of actors for navigation.
        ///
        /// @remarks
        ///     Default value is 2.0 (2 meters).
        void SetWalkableHeight(float height);

        /// Sets the walkable radius of actors for navigation.
        ///
        /// @remarks
        ///     Default value is 0.5 (0.5 meters).
        void SetWalkableRadius(float radius);

        /// Sets the walkable slope angle.
        ///
        /// @remarks
        ///     Default value is 10.0 degrees.
        void SetWalkableSlopeAngle(float angle);

        /// Sets the walkable climb height (for stepping).
        ///
        /// @remarks
        ///     Default value is 0.25.
        void SetWalkableClimbHeight(float height);


        /// Retrieves the walkable height of actors for navigation.
        ///
        /// @remarks
        ///     Default value is 2.0 (2 meters).
        float GetWalkableHeight() const;

        /// Retrieves the walkable radius of actors for navigation.
        ///
        /// @remarks
        ///     Default value is 0.5 (0.5 meters).
        float GetWalkableRadius() const;

        /// Retrieves the walkable slope angle.
        ///
        /// @remarks
        ///     Default value is 10.0 degrees.
        float GetWalkableSlopeAngle() const;

        /// Retrieves the walkable climb height (for stepping).
        ///
        /// @remarks
        ///     Default value is 0.25.
        float GetWalkableClimbHeight() const;


        /// Rebuilds the navigation mesh that will be used for doing navigation paths.
        void BuildNavigationMesh();

        /// Retrieves the points on a navigation path between the given start and end positions.
        ///
        /// @param start  [in]  The start position.
        /// @param end    [in]  The end position.
        /// @param output [out] A reference to the vector that will receive the navigation points.
        void FindNavigationPath(const glm::vec3 &start, const glm::vec3 &end, GTCore::Vector<glm::vec3> &output);

        /// A hacky temp method for retrieving a reference to the internal list of scene nodes. (Used with NavigationMesh. Will be replaced later.)
        const SceneNodeMap & GetSceneNodes() const { return this->sceneNodes; }



    // Serialization/Deserialization.
    public:

        /// Serializes the scene using the given serializer.
        ///
        /// @param serializer [in] The serializer to copy the scene data in to.
        bool Serialize(GTCore::Serializer &serializer) const;

        /// Deserializes the scene using the given deserializer.
        ///
        /// @param deserializer [in] The deserializer to copy the scene data from.
        ///
        /// @return True if the scene is deserialized successfully; false otherwise.
        ///
        /// @remarks
        ///     If deserialization fails, the scene will be left completely unmodified.
        bool Deserialize(GTCore::Deserializer &deserializer);



    public: // Events.

        /// Called when the scene is updated.
        ///
        /// @remarks
        ///     This is called at the beginning of the update.
        virtual void OnUpdate(double deltaTimeInSeconds);




        /// Called after a scene node is added to the scene. See remarks.
        ///
        /// @remarks
        ///     This function is also called when a scene node that is already part of this scene has a child attached to it.
        void OnSceneNodeAdded(SceneNode &node);

        /// Called after a scene node is removed from the scene.
        void OnSceneNodeRemoved(SceneNode &node);

        /// Called when a scene node is moved, rotated or both. This is not called for scaling. Use OnSceneNodeScale() that.
        ///
        /// @param node                 [in] A reference to the node that has been transformed.
        /// @param updateDynamicsObject [in] Controls whether or not the dynamics object, if any, should be updated also.
        void OnSceneNodeTransform(SceneNode &node, bool updateDynamicsObject);

        void OnSceneNodeScale(SceneNode &node);
        void OnSceneNodeStaticChanged(SceneNode &node);
        void OnSceneNodeVisibleChanged(SceneNode &node);

        /// Called after a scene node has a component attached.
        ///
        /// @param node      [in] A reference to the node whose component is being added.
        /// @param component [in] A reference to the component being added.
        void OnSceneNodeComponentAdded(SceneNode &node, Component &component, bool postEvents = true);

        /// Called just before a node has a component removed.
        ///
        /// @param node      [in] A reference to the node whose component is being added.
        /// @param component [in] A reference to the component being added.
        void OnSceneNodeComponentRemoved(SceneNode &node, Component &component, bool postEvents = true);

        /// Sometimes when a component changes, the scene needs to update some information.
        ///
        /// @param node      [in] A reference to the node whose component has been modified.
        /// @param component [in] A reference to the component has been modified.
        ///
        /// @remarks
        ///     An example is when the attenuation of a point light changes, the scene may need to update culling information.
        void OnSceneNodeComponentChanged(SceneNode &node, Component &component);



    private:

        /////////////////////////////////////////////////
        // Event Posting

        /// Helper for posting an OnObjectAdded event to every attached event handler.
        ///
        /// @param object [in] A reference to the object that was just added.
        void PostEvent_OnObjectAdded(SceneObject &object);

        /// Helper for posting an OnObjectRemoved event to every attached event handler.
        ///
        /// @param object [in] A reference to the object that is being removed.
        void PostEvent_OnObjectRemoved(SceneObject &object);

        /// Helper for posting an OnObjectRefreshed event to every attached event handler.
        ///
        /// @param object [in] A reference to the object that has just been refreshed.
        void PostEvent_OnObjectRefreshed(SceneObject &object);

        /// Helper for posting an OnSceneNodeTransform event.
        ///
        /// @param node [in] A reference to the scene node that has just had it's tranformation changed.
        void PostEvent_OnSceneNodeTransform(SceneNode &node);

        /// Helper for posting an OnSceneNodeScale event.
        ///
        /// @param node [in] A reference to the scene node that has just had it's scale changed.
        void PostEvent_OnSceneNodeScale(SceneNode &node);

        /// Helper for posting an OnSceneNodeHide event.
        ///
        /// @param node [in] A reference to the scene node that has just been made invisible.
        void PostEvent_OnSceneNodeHide(SceneNode &node);

        /// Helper for posting an OnSceneNodeShow event.
        ///
        /// @param node [in] A reference to the scene node that has just been made visible.
        void PostEvent_OnSceneNodeShow(SceneNode &node);

        /// Helper for posting an OnSceneNodeComponentAdded event.
        ///
        /// @param node      [in] A reference to the scene node whose component has just been added.
        /// @param component [in] A reference to the newly added component.
        void PostEvent_OnSceneNodeComponentAdded(SceneNode &node, Component &component);

        /// Helper for posting an OnSceneNodeComponentRemoved event.
        ///
        /// @param node      [in] A reference to the scene node whose component has just been removed.
        /// @param component [in] A reference to the component being removed.
        void PostEvent_OnSceneNodeComponentRemoved(SceneNode &node, Component &component);

        /// Helper for posting an OnSceneNodeComponentChanged event.
        ///
        /// @param node      [in] A reference to the scene node whose component has just been modified.
        /// @param component [in] A reference to the component that was changed.
        void PostEvent_OnSceneNodeComponentChanged(SceneNode &node, Component &component);

        /// Helper for posting an OnStateStackFrameCommitted event.
        void PostEvent_OnStateStackFrameCommitted();


    private:

        /// A pointer to the scene's renderer. This will never actually be null, but it can be changed dynamically. Thus, it needs to be a pointer instead
        /// of a reference. This will default to a heap-allocated DefaultSceneRenderer.
        SceneRenderer* renderer;

        /// A reference to the update manager.
        SceneUpdateManager &updateManager;

        /// A reference to the physics manager.
        ScenePhysicsManager &physicsManager;

        /// A reference to the culling manager.
        SceneCullingManager &cullingManager;


        /// Keeps track of whether or not the renderer needs to be deleted when it changes or the scene is destructed.
        bool deleteRenderer;

        /// Keeps track of whether or not the update manager needs to be deleted by the destructor.
        bool deleteUpdateManager;

        /// Keeps track of whether or not the physics manager needs to be deleted by the destructor.
        bool deletePhysicsManager;

        /// Keeps track of whether or not the culling manager needs to be deleted by the destructor.
        bool deleteCullingManager;

        /// Whether or not the scene is paused.
        bool paused;

        /// Keeps track of whether or not we are refreshing an object. If we are, we don't want to post OnObjectAdded and OnObjectRemoved events to event handlers. Refreshing an
        /// object just removes and re-adds it.
        bool isRefreshingObject;


        /// The list of viewports currently attached to this scene.
        GTCore::List<SceneViewport*> viewports;


        /// The list of scene nodes in the scene. This contains every scene node, including those connected to a parent. This is indexed by the unique ID of the scene node.
        SceneNodeMap sceneNodes;

        /// The next unique ID to apply to new scene nodes.
        uint64_t nextSceneNodeID;



        /// The list of ambient light components.
        GTCore::List<AmbientLightComponent*> ambientLightComponents;

        /// The list of directional light components.
        GTCore::List<DirectionalLightComponent*> directionalLightComponents;


        /// The navigation mesh for doing navigation paths.
        NavigationMesh navigationMesh;


        /// The list of event handlers current attached to the scene.
        GTCore::Vector<SceneEventHandler*> eventHandlers;


        /// The state stack.
        SceneStateStack stateStack;

        /// Keeps track of whether or not staging should be enabled on the state stack.
        bool isStateStackStagingEnabled;



    private:    // No copying.
        Scene(const Scene &);
        Scene & operator=(const Scene &);
    };

}

#endif
