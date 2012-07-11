
#ifndef __GTEngine_DefaultScene_hpp_
#define __GTEngine_DefaultScene_hpp_

#include "Scene.hpp"
#include "DefaultSceneUpdateManager.hpp"
#include "DefaultScenePhysicsManager.hpp"
#include "DefaultSceneCullingManager.hpp"
#include "Physics.hpp"
#include "NavigationMesh.hpp"
#include <GTCore/List.hpp>
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    /**
    *   \brief  Class representing the default scene.
    */
    class DefaultScene : public Scene
    {
    public:

        /**
        *   \brief  Constructor.
        */
        DefaultScene();

        /**
        *   \brief  Destructor.
        */
        ~DefaultScene();

        /// Scene::AddSceneNode(SceneNode &)
        void AddSceneNode(SceneNode &node);

        /// Scene::RemoveSceneNode(SceneNode &)
        void RemoveSceneNode(SceneNode &node);

        /// Scene::Update(double)
        void Update(double deltaTimeInSeconds);

        /// Scene::OnSceneNodeAdded(SceneNode &)
        void OnSceneNodeAdded(SceneNode &node);

        /// Scene::OnSceneNodeRemoved(SceneNode &)
        void OnSceneNodeRemoved(SceneNode &node);

        /// Scene::FindFirstNode(const char*)
        SceneNode* FindFirstNode(const char* name);

        /// Scene::FindFirstNodeWithComponent(const char*)
        SceneNode* FindFirstNodeWithComponent(const char* componentName);

        template <typename T>
        SceneNode* FindFirstNodeWithComponent()
        {
            return this->FindFirstNodeWithComponent(T::Name);
        }


        /// Scene::AddViewport(SceneViewport &);
        void AddViewport(SceneViewport &viewport);

        /// Scene::RemoveViewport(SceneViewport &);
        void RemoveViewport(SceneViewport &viewport);


        /// Scene::GetAABB(glm::vec3 &, glm::vec3 &);
        void GetAABB(glm::vec3 &min, glm::vec3 &max) const;


        /// Scene::RayTest()
        SceneNode* RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, RayTestCallback &callback);

        /// Scene::ContactTest()
        void ContactTest(const SceneNode &node, ContactTestCallback &callback);



    // Occlusion.
    public:

        /// Scene::GetVisibleNodes(const SceneViewport &);
        void AddVisibleComponents(SceneViewport &viewport);


    // Physics.
    public:

        /// Scene::SetGravity(float, float, float)
        void SetGravity(float x, float y, float z);

        /// Scene::GetGravity(glm::vec3 &)
        void GetGravity(float &x, float &y, float &z) const;


    // AI.
    public:

        void SetWalkableHeight(float height);
        void SetWalkableRadius(float radius);
        void SetWalkableSlopeAngle(float angle);
        void SetWalkableClimbHeight(float height);

        float GetWalkableHeight() const;
        float GetWalkableRadius() const;
        float GetWalkableSlopeAngle() const;
        float GetWalkableClimbHeight() const;


        /// Rebuilds the navigation mesh that will be used for doing navigation paths.
        void BuildNavigationMesh();

        /// Retrieves the points on a navigation path between the given start and end positions.
        void FindNavigationPath(const glm::vec3 &start, const glm::vec3 &end, GTCore::Vector<glm::vec3> &output);


        /// Shows the navigation mesh.
        void ShowNavigationMesh();

        /// Hides the navigation mesh.
        void HideNavigationMesh();


    public:

        void OnSceneNodeTransform(SceneNode &node);
        void OnSceneNodeScale(SceneNode &node);
        void OnSceneNodeComponentChanged(SceneNode& node, Component& component);



    public:

        /// A hacky temp method for retrieving a reference to the internal list of scene nodes. (Used with NavigationMesh. Will be replaced later.)
        const GTCore::List<SceneNode*> & GetSceneNodes() const { return this->nodes; }


    private:

        /// Does a pre-update clean of dead nodes, caches, etc. This is the very first function called in Update().
        void DoPreUpdateClean();



    private:

        /// The list of viewports currently attached to this scene.
        GTCore::List<SceneViewport*> viewports;

        /// The list of nodes in the scene. This is a flat list for now, but will be optimized later.
        GTCore::List<SceneNode*> nodes;


        /// The update manager.
        DefaultSceneUpdateManager updateManager;

        /// The physics manager.
        DefaultScenePhysicsManager physicsManager;

        /// The culling manager.
        DefaultSceneCullingManager cullingManager;


        /// The list of ambient light components.
        GTCore::List<AmbientLightComponent*> ambientLightComponents;

        /// The list of directional light components.
        GTCore::List<DirectionalLightComponent*> directionalLightComponents;


        /// The navigation mesh for doing navigation paths.
        NavigationMesh navigationMesh;

        /// The scene node for the navigation mesh.
        //SceneNode navigationMeshNode;

        /// The model for drawing the navigation mesh.
        //Model navigationMeshModel;


    private:    // No copying.
        DefaultScene(const DefaultScene &);
        DefaultScene & operator=(const DefaultScene &);
    };
}

#endif