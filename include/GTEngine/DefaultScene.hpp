
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

        /// Scene::RayTest()
        SceneNode* RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd);


    // Occlusion.
    public:

        /// Scene::GetVisibleNodes(const SceneViewport &);
        void AddVisibleComponents(SceneViewport &viewport);


    public:

        void OnSceneNodeTransform(SceneNode &node);
        void OnSceneNodeScale(SceneNode &node);

        void OnSceneNodeComponentAttached(SceneNode& node, Component& component);
        void OnSceneNodeComponentDetached(SceneNode& node, Component& component);
        void OnSceneNodeComponentChanged(SceneNode& node, Component& component);


    private:

        void UpdateNode(SceneNode &node, double deltaTimeInSeconds);

        /// Does a pre-update clean of dead nodes, caches, etc. This is the very first function called in Update().
        void DoPreUpdateClean();


        /// Adds culling objects for the given model.
        void AddModelCullingObjects(ModelComponent &modelComponent);

        /// Removes culling objects for the given model.
        void RemoveModelCullingObjects(ModelComponent &modelComponent);

        /// Adds culling objects for the given point light.
        void AddPointLightCullingObjects(PointLightComponent &light);

        /// Removes culling objects for the given point light.
        void RemovePointLightCullingObjects(PointLightComponent &light);

        /// Adds culling objects for the given spot light.
        void AddSpotLightCullingObjects(SpotLightComponent &light);

        /// Removes culling objects for the given spot light.
        void RemoveSpotLightCullingObjects(SpotLightComponent &light);
        


    private:

        /// Structure containing metadata about a scene node.
        struct SceneNodeMetadata
        {
            /// A pointer to the collision object for the model component. Can be null.
            CollisionObject* modelCollisionObject;

            /// The collision shape to use with the model. Can be null only if <modelCollisionObject> is also null.
            btCompoundShape* modelCollisionShape;


            /// A pointer to the collision object for the point light component. Can be null.
            CollisionObject* pointLightCollisionObject;

            /// The collision shape to use with the point light collision object. Can be null only if <pointLightCollisionObject> is also null.
            btSphereShape* pointLightCollisionShape;


            /// A pointer to the collision object for the spot light component. Can be null.
            CollisionObject* spotLightCollisionObject;

            /// The collision shape to use with the spot light collision object. Can be null only if <spotLightCollisionObject> is also null. We
            /// need to use a compound shape here because the cone will need to be offset by half it's height.
            btCompoundShape* spotLightCollisionShape;



            SceneNodeMetadata()
                : modelCollisionObject(nullptr), modelCollisionShape(nullptr),
                  pointLightCollisionObject(nullptr), pointLightCollisionShape(),
                  spotLightCollisionObject(nullptr), spotLightCollisionShape()
            {
            }

            ~SceneNodeMetadata()
            {
                this->DeleteModelCollisionObject();
                this->DeletePointLightCollisionObject();
                this->DeleteSpotLightCollisionObject();
            }

            /// Allocates the model collision object and shape.
            void AllocateModelCollisionObject()
            {
                this->DeleteModelCollisionObject();

                modelCollisionObject = new CollisionObject;
                modelCollisionShape  = new btCompoundShape;
            }

            /// Deletes the model collision object and shape.
            void DeleteModelCollisionObject()
            {
                if (modelCollisionShape != nullptr)
                {
                    while (this->modelCollisionShape->getNumChildShapes() > 0)
                    {
                        auto child = this->modelCollisionShape->getChildShape(0);
                        this->modelCollisionShape->removeChildShapeByIndex(0);

                        delete child;
                    }
                }

                delete this->modelCollisionObject;
                delete this->modelCollisionShape;

                this->modelCollisionObject = nullptr;
                this->modelCollisionShape  = nullptr;
            }



            /// Allocates the point light culling object and shape.
            void AllocatePointLightCollisionObject(float radius)
            {
                this->DeletePointLightCollisionObject();

                pointLightCollisionObject = new CollisionObject;
                pointLightCollisionShape  = new btSphereShape(radius);
            }

            /// Deletes the point light culling object and shape.
            void DeletePointLightCollisionObject()
            {
                delete this->pointLightCollisionObject;
                delete this->pointLightCollisionShape;

                this->pointLightCollisionObject = nullptr;
                this->pointLightCollisionShape  = nullptr;
            }


            
            /// Allocates the spot light culling object and shape.
            void AllocateSpotLightCollisionObject(float outerAngle, float height)
            {
                this->DeleteSpotLightCollisionObject();

                spotLightCollisionObject = new CollisionObject;
                spotLightCollisionShape  = new btCompoundShape;

                // Here we create the cone shape. We need to offset by half the height because Bullet creates it's cones centered.
                btTransform coneTransform;
                coneTransform.setIdentity();
                coneTransform.setOrigin(btVector3(0.0f, 0.0f, -height * 0.5f));
                spotLightCollisionShape->addChildShape(coneTransform, new btConeShapeZ(glm::sin(glm::radians(outerAngle)) * height, height));
            }

            /// Deletes a spot light culling object and shape.
            void DeleteSpotLightCollisionObject()
            {
                if (modelCollisionShape != nullptr)
                {
                    while (this->modelCollisionShape->getNumChildShapes() > 0)
                    {
                        auto child = this->modelCollisionShape->getChildShape(0);
                        this->modelCollisionShape->removeChildShapeByIndex(0);

                        delete child;
                    }
                }

                delete this->spotLightCollisionObject;
                delete this->spotLightCollisionShape;

                this->spotLightCollisionObject = nullptr;
                this->spotLightCollisionShape  = nullptr;
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


        /// The list of ambient light components.
        GTCore::List<AmbientLightComponent*> ambientLightComponents;

        /// The list of directional light components.
        GTCore::List<DirectionalLightComponent*> directionalLightComponents;


        /// The dynamics world for everything involving physics and collision detection.
        DynamicsWorld dynamicsWorld; 

        /// The collision world that will do occlusion culling and other collision detection functionality. We also use this
        /// world for picking. This world contains objects for each relevant component of an entity. The scene distringuishes
        /// between components types by looking at the collision group.
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