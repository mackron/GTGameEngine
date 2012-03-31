
#ifndef __GTEngine_DefaultScene_hpp_
#define __GTEngine_DefaultScene_hpp_

#include "Scene.hpp"
#include "Physics.hpp"
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

        
        /// Scene::PickSceneNode()
        SceneNode* PickSceneNode(const glm::vec3 &rayStart, const glm::vec3 &rayEnd);


    public:

        void OnSceneNodeTransform(SceneNode &node);
        void OnSceneNodeScale(SceneNode &node, const glm::vec3 &prevPosition);

#if 0
        void OnSceneNodeStaticChanged(SceneNode& node);
        void OnSceneNodeComponentAttached(SceneNode& node, Component& component);
        void OnSceneNodeComponentDetached(SceneNode& node, Component& component);
#endif
    

    private:

        void UpdateNode(SceneNode &node, double deltaTimeInSeconds);

        /// Does a pre-update clean of dead nodes, caches, etc. This is the very first function called in Update().
        void DoPreUpdateClean();


    private:

        /// Structure containing metadata about a scene node.
        struct SceneNodeMetadata
        {
            /// The collision object used for picking.
            CollisionObject pickingObject;

            /// The collision shape to use with picking. This is a compound object.
            btCompoundShape pickingShape;



            SceneNodeMetadata()
                : pickingObject(), pickingShape()
            {
            }

            ~SceneNodeMetadata()
            {
                while (this->pickingShape.getNumChildShapes() > 0)
                {
                    auto child = this->pickingShape.getChildShape(0);
                    this->pickingShape.removeChildShapeByIndex(0);

                    delete child;
                }
            }

        private:    // No copying.
            SceneNodeMetadata(const SceneNodeMetadata &);
            SceneNodeMetadata & operator=(const SceneNodeMetadata &);
        };


    private:

        /// The list of viewports currently attached to this scene.
        GTCore::List<SceneViewport*> viewports;

        /// The list of nodes in the viewport. This is a flat list for now, but will be optimized later.
        GTCore::List<SceneNode*> nodes;


        /// The dynamics world for everything involving physics and collision detection.
        DynamicsWorld dynamicsWorld; 

        /// The collision world that will do occlusion culling and other collision detection functionality. We also use this
        /// world for picking.
        CollisionWorld occlusionCollisionWorld;



        /// The list of camera nodes in the scene.
        GTCore::List<SceneNode*> cameraNodes;

        /// The list of static nodes. These nodes are not updated.
        GTCore::List<SceneNode*> staticNodes;

        /// The list of dynamic nodes. These nodes are updated, unlike static nodes.
        GTCore::List<SceneNode*> dynamicNodes;


    private:    // No copying.
        DefaultScene(const DefaultScene &);
        DefaultScene & operator=(const DefaultScene &);
    };
}

#endif