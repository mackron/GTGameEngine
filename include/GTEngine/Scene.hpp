
#ifndef __GTEngine_Scene_hpp_
#define __GTEngine_Scene_hpp_

#include "SceneNode.hpp"
#include "SceneViewport.hpp"
#include "Physics.hpp"
#include "DefaultSceneUpdateManager.hpp"
#include "DefaultScenePhysicsManager.hpp"
#include "DefaultSceneCullingManager.hpp"
#include "NavigationMesh.hpp"


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
        virtual bool NeedsCollision(short collisionGroupIn, short collisionMaskIn, SceneNode &object) const
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
    };
}


namespace GTEngine
{
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


        /// Adds a scene node to the scene.
        ///
        /// @param node [in] A reference to the scene node to add to the scene.
        void AddSceneNode(SceneNode &node);
        
        /// Removes a scene node from the scene.
        ///
        /// @param node [in] A reference to the node to remove.
        void RemoveSceneNode(SceneNode &node);


        /// Refreshes the given scene object.
        ///
        /// @param object [in] A reference to the object to refresh.
        ///
        /// @remarks
        ///     Internally, refreshing an object will simply remove and re-add the object from/to the scene, which will then force updates to
        ///     the internal structures (update manager, physics manager, etc).
        virtual void RefreshObject(SceneObject &object);


        /// Pauses the scene.
        ///
        /// @remarks
        ///     Calling Update() while the scene is paused will still draw each scene node, but it will not step scene nodes or animations.
        virtual void Pause();

        /// Resumes the scene.
        virtual void Resume();

        /// Determines whether or not the scene is paused.
        ///
        /// @return True if the scene is paused; false otherwise.
        virtual bool IsPaused() const { return this->paused; }

        
        /// Steps the scene.
        ///
        /// @param deltaTimeInSeconds [in] The time in seconds to step the scene by. This will usually be the time between frames, but it can be adjusted to speed up/slow down time.
        virtual void Update(double deltaTimeInSeconds);


        /// Retrieves a pointer to the first occurance of the scene node with the given name. Returns nullptr if none exist.
        virtual SceneNode* FindFirstNode(const char *name);

        /// Retrieves a pointer to the first occurance of the scene node with the given component. Returns nullptr is none exist.
        virtual SceneNode* FindFirstNodeWithComponent(const char *componentName);

        template <typename T>
        SceneNode* FindFirstNodeWithComponent()
        {
            return this->FindFirstNodeWithComponent(T::Name);
        }


        /// Adds a viewport to the scene.
        /// @param viewport [in] A reference to the viewport to add to the scene.
        virtual void AddViewport(SceneViewport &viewport);

        /// Removes a viewport from the scene.
        /// @param viewport [in] A reference to the viewport to remove from the scene.
        virtual void RemoveViewport(SceneViewport &viewport);


        /// Retrieves the AABB of the scene.
        virtual void GetAABB(glm::vec3 &min, glm::vec3 &max) const;


    // Collision Tests.
    public:

        /// Performs a ray test on the scene nodes against their dynamics components.
        ///
        /// @param rayStart [in     ] The start position of the ray.
        /// @param rayEnd   [in     ] The end position of the ray.
        /// @param callback [in, out] A reference to the callback structure that will receive the results of the ray test.
        ///
        /// @return A pointer to the closest scene node to 'rayStart', or null if there were no contacts.
        virtual SceneNode* RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, RayTestCallback &callback);

        /// Performs a contact test against the proximity volume of the given scene node.
        ///
        /// @param node     [in]      The node to perform the contact test against.
        /// @param callback [in, out] A reference to the callback structure that will receive the results of the contact test.
        virtual void ContactTest(const SceneNode &node, ContactTestCallback &callback);



    // Occlusion
    public:

        /// Adds the applicable visible components using the given viewport's Add*Component() API.
        ///
        /// @param viewport [in] A reference to the viewport whose having visible components added.
        ///
        /// @remarks
        ///     You should not need to call this method directly. It is intended to be used internally by SceneViewport.
        virtual void AddVisibleComponents(SceneViewport &viewport);     // TODO: Rename this. Waaaay to confusing..



    // Physics.
    public:

        /// Sets the scene's gravity.
        virtual void SetGravity(float x, float y, float z);
                void SetGravity(const glm::vec3 &gravity) { this->SetGravity(gravity.x, gravity.y, gravity.z); }

        /// Retrieves the scene's gravity.
        virtual void GetGravity(float &x, float &y, float &z) const;

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
        virtual void SetWalkableHeight(float height);

        /// Sets the walkable radius of actors for navigation.
        ///
        /// @remarks
        ///     Default value is 0.5 (0.5 meters).
        virtual void SetWalkableRadius(float radius);

        /// Sets the walkable slope angle.
        ///
        /// @remarks
        ///     Default value is 10.0 degrees.
        virtual void SetWalkableSlopeAngle(float angle);

        /// Sets the walkable climb height (for stepping).
        ///
        /// @remarks
        ///     Default value is 0.25.
        virtual void SetWalkableClimbHeight(float height);


        /// Retrieves the walkable height of actors for navigation.
        ///
        /// @remarks
        ///     Default value is 2.0 (2 meters).
        virtual float GetWalkableHeight() const;

        /// Retrieves the walkable radius of actors for navigation.
        ///
        /// @remarks
        ///     Default value is 0.5 (0.5 meters).
        virtual float GetWalkableRadius() const;

        /// Retrieves the walkable slope angle.
        ///
        /// @remarks
        ///     Default value is 10.0 degrees.
        virtual float GetWalkableSlopeAngle() const;

        /// Retrieves the walkable climb height (for stepping).
        ///
        /// @remarks
        ///     Default value is 0.25.
        virtual float GetWalkableClimbHeight() const;


        /// Rebuilds the navigation mesh that will be used for doing navigation paths.
        void BuildNavigationMesh();

        /// Retrieves the points on a navigation path between the given start and end positions.
        ///
        /// @param start  [in ] The start position.
        /// @param end    [in ] The end position.
        /// @param output [out] A reference to the vector that will receive the navigation points.
        void FindNavigationPath(const glm::vec3 &start, const glm::vec3 &end, GTCore::Vector<glm::vec3> &output);

        /// A hacky temp method for retrieving a reference to the internal list of scene nodes. (Used with NavigationMesh. Will be replaced later.)
        const GTCore::List<SceneNode*> & GetSceneNodes() const { return this->nodes; }


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

        /// Sometimes when a component changes, the scene needs to update some information. An example is when the attenuation of a point light
        /// changes, the scene may need to update culling information.
        virtual void OnSceneNodeComponentChanged(SceneNode& node, Component& component);

        virtual void OnPause();
        virtual void OnResume();


    protected:

        /// A reference to the update manager.
        SceneUpdateManager &updateManager;

        /// A reference to the physics manager.
        ScenePhysicsManager &physicsManager;

        /// A reference to the culling manager.
        SceneCullingManager &cullingManager;

        /// Keeps track of whether or not the update manager needs to be deleted by the destructor.
        bool deleteUpdateManager;

        /// Keeps track of whether or not the physics manager needs to be deleted by the destructor.
        bool deletePhysicsManager;

        /// Keeps track of whether or not the culling manager needs to be deleted by the destructor.
        bool deleteCullingManager;

        /// Whether or not the scene is paused.
        bool paused;


        /// The list of viewports currently attached to this scene.
        GTCore::List<SceneViewport*> viewports;

        /// The list of nodes in the scene. This is a flat list for now, but will be optimized later.
        GTCore::List<SceneNode*> nodes;


        /// The list of ambient light components.
        GTCore::List<AmbientLightComponent*> ambientLightComponents;

        /// The list of directional light components.
        GTCore::List<DirectionalLightComponent*> directionalLightComponents;


        /// The navigation mesh for doing navigation paths.
        NavigationMesh navigationMesh;


    private:    // No copying.
        Scene(const Scene &);
        Scene & operator=(const Scene &);
    };

}

#endif