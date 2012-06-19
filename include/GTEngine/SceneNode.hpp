

#ifndef __GTEngine_SceneNode_hpp_
#define __GTEngine_SceneNode_hpp_

#include "Math.hpp"

// Though not required for compilation of this file, we will include the components here to make using the SceneNode class a bit easier. They're
// pretty light-weight, so they don't add too much time to compilation.
#include "Components/CameraComponent.hpp"
#include "Components/LightComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/SpriteComponent.hpp"
#include "Components/DynamicsComponent.hpp"
#include "Components/ProximityComponent.hpp"
#include "Components/OccluderComponent.hpp"

#include <GTCore/Dictionary.hpp>
#include <GTCore/List.hpp>
#include <GTCore/Map.hpp>
#include <GTCore/String.hpp>

namespace GTEngine
{
    class Scene;            // <-- Needed by SceneNode.
    class SceneNode;        // <-- Needed by SceneNodeEventHandler.

    /**
    *   \brief  Base class for handling events on scene nodes.
    *
    *   
    */
    class SceneNodeEventHandler
    {
    public:

        /**
        *   \brief                 Called after a child has been attached to the node.
        *   \param  node      [in] A pointer to the node whose just had 'childNode' attached to it.
        *   \param  childNode [in] A pointer to the child node that was just attached as a child to 'node'.
        */
        virtual void OnAttach(SceneNode &node, SceneNode &childNode);
        
        /**
        *   \brief                 Called after a child has been detached from the node.
        *   \param  node      [in] A pointer to the node whose child is being detached.
        *   \param  childNode [in] A pointer to the child node that is being detached from 'node'.
        *
        *   \remarks
        *       Note that when this function is called, childNode->GetParent() will actually return null since technically
        *       it is not attached to anything. Remember, this function is called AFTER the child has been detached from
        *       the parent.
        */
        virtual void OnDetach(SceneNode &node, SceneNode &childNode);
        
        /**
        *   \brief  Called just before the node is destroyed.
        *
        *   \remarks
        *       This event is called during the nodes destructor. You should only do cleaning-up in this event and should
        *       not create new nodes.
        *       \par
        *       The destructor of a node will also detach that node from the parent. This function is called after OnDetach().
        */
        virtual void OnDestroy(SceneNode& node);


        /// Called after a node has been moved, rotated or both.
        ///
        /// @param node [in] A reference to the node that has been transformed.
        ///
        /// @remarks
        ///     This is not called for scaling. For scaling, use OnScale() instead.
        virtual void OnTransform(SceneNode &node);


        /**
        *   \brief  Called after a node has been scaled.
        *
        *   \remarks
        *       Since this function is called after the node has changed scale, calling node->GetScale() will yield
        *       the current scale of the node. The value returned by node->GetScale() will always be different than
        *       prevScale.
        */
        virtual void OnScale(SceneNode &node);

        /**
        *   \brief  Called after the layer the node is sitting on has changed.
        */
        virtual void OnLayerChanged(SceneNode &node, unsigned int prevLayer);

        /**
        *   \brief  Called after a scene node has changed scenes.
        */
        virtual void OnSceneChanged(SceneNode &node, Scene *prevScene);

        /**
        *   \brief  Called after a scene node has changed from static to dynamic, or vice-versa.
        */
        virtual void OnStaticChanged(SceneNode &node);

        /**
        *   \brief  Called after a scene node has changed visibility.
        */
        virtual void OnVisibleChanged(SceneNode &node);

        /**
        *   \brief                 Called after a component is attached to the scene node.
        *   \param  node      [in] A pointer to the node that has had a component attached.
        *   \param  component [in] A pointer to the component that was just attached.
        */
        virtual void OnComponentAttached(SceneNode &node, Component &component);

        /**
        *   \brief                 Called after a component is detached from the scene node.
        *   \param  node      [in] A pointer to the node that has had a component detached.
        *   \param  component [in] A pointer to the component that was just detached.
        */
        virtual void OnComponentDetached(SceneNode &node, Component &component);


        /**
        *   \brief                          Called when the scene node is being updated.
        *   \param  node               [in] A pointer to the node that is being updated.
        *   \param  deltaTimeInSeconds [in] The since the last update, in seconds.
        */
        virtual void OnUpdate(SceneNode &node, double deltaTimeInSeconds);


        /// Called when a physics object is touching another object.
        ///
        /// @param node  [in] A reference to the owner of the event.
        /// @param other [in] The other node the node is touching.
        /// @param pt    [in] Contact information.
        ///
        /// @remarks
        ///     This event is not only called when two objects go from a non-colliding to colliding state. Also, it will be called every frame
        ///     that the nodes are touching.
        ///     @par
        ///     This event will only be called for scene nodes with dynamics components whose collisions are detectable by the physics system. Kinematic/Kinematic,
        ///     Static/Static and Kinematic/Static collisions will not be handled. If these collisions are required, consider using a proximity component.
        virtual void OnContact(SceneNode &node, SceneNode &other, const btManifoldPoint &pt);



    public:

        // We need to define a default constructor.
        SceneNodeEventHandler() {}

        // This is just a warning silencer.
        virtual ~SceneNodeEventHandler() {}


    public:
        
        /**
        *   \brief  Retrieves the default scene node event handler.
        */
        static SceneNodeEventHandler & GetDefault();


    private:
        SceneNodeEventHandler(const SceneNodeEventHandler &);
        SceneNodeEventHandler & operator=(const SceneNodeEventHandler &);
    };


    /**
    *   \brief  Base class for scene nodes.
    *
    *   Scene nodes have a single parent and many children. The positions of scenes nodes are always relative to the parent
    *   node. Helper functions exist for retrieving and setting the world transformation of the node.
    *
    *   Nodes have a series of Components attached to them. The components essentially define what the node actually does. For
    *   example, if the node has a point light component attached, it means the node will emit a point light.
    *
    *   Nodes also have a list (or map, specifically) or data pointers. It is through these data pointers that the node can
    *   have custom data attached to it. There can be many data pointers attached to a node. Each different data pointer is
    *   distinguished with a key. When retrieving or setting a data pointer, you must also specify a key for that data pointer.
    *   The node itself does not use any predefined keys, but if using classes/objects which also use a scene node, you must
    *   ensure your data pointer keys do not conflict.
    */
    class SceneNode
    {
    public:

        /// Constructor.
        SceneNode();

        /**
        *   \brief  Destructor.
        *
        *   The destructor will orphan/detach any children that are still attached. Children must be deleted manually.
        *
        *   The destructor will detach the node from it's parent, if it has one. After being detached from
        *   the parent, it will call the OnDestroy() event. OnDestroy() is called before the constructor orphans
        *   the child nodes.
        *
        *   The destructor will delete any attached components. The destructor does not delete the attached event
        *   handler or data pointers.
        */
        virtual ~SceneNode();
        
        
        /**
        *   \brief  Retrieves the name of the node.
        */
        const char* GetName() const;

        /**
        *   \brief  Sets the name of the node.
        */
        void SetName(const char* name);

        
        /**
        *   \brief  Retrieves the parent node, or null if the node does not have a parent.
        */
        SceneNode* GetParent();


        /// Retrieves a pointer to the first child node.
        SceneNode* GetFirstChild();

        /// Retrieves a pointer to the last child node.
        SceneNode* GetLastChild();

        /// Retrieves a pointer to the previous sibling. Returns null if the node is the first of it's siblings, or does not have any.
        SceneNode* GetPrevSibling();

        /// Retrieves a pointer to the next sibling. Returns null if the node if the last of it's siblings, or does not have any.
        SceneNode* GetNextSibling();


        /// Retrieves the top-level ancestor.
        ///
        /// @remarks
        ///     Returns <this> if the node has not parents.
        SceneNode* GetTopAncestor();
        

        /**
        *   \brief               Attaches an event handler to the node.
        *   \param  handler [in] A reference to the event handler to attach to the node.
        *
        *   \remarks
        *       This function will not attach the handler if it is already attached.
        *       \par
        *       The event handler can not be deleted while it is in scope. A node will not make a copy of the event handler. Instead
        *       it just references the object directly. It is done this way so that a single event handler can be used across multiple
        *       nodes, thus saving a bit of memory.
        */
        void AttachEventHandler(SceneNodeEventHandler &handler);
        
        /**
        *   \brief               Detaches an event handler from the node.
        *   \param  handler [in] A reference to the event handler to detach.
        *
        *   \remarks
        *       Note that this function does not actually delete the event handler. Deletion must be managed by the caller.
        */
        void DetachEventHandler(SceneNodeEventHandler &handler);
        

        /**
        *   \brief                 Attaches a child to this node.
        *   \param  childNode [in] A pointer to the child node to attach to this node.
        *
        *   \remarks
        *       If the child is already attached to another parent, it is first detached from that parent. OnDetach() will be called
        *       like normal.
        *       \par
        *       If the child is already attached to a different scene as this node, it will be removed from that scene.
        */
        void AttachChild(SceneNode &childNode);

        /**
        *   \brief              Attaches this node to another node.
        *   \param  parent [in] A pointer to the parent node that this node should be attached to.
        *
        *   \remarks
        *       This function is the same as Attach(), only the other way around. This is more intuitive in some cases.
        */
        void AttachTo(SceneNode &parent);
        
        /**
        *   \brief                 Detaches/orphans a child from the node.
        *   \param  childNode [in] A pointer to the child node to detach.
        *
        *   \remarks
        *       This method does NOT delete the child.
        */
        void DetachChild(SceneNode& childNode);
        
        /**
        *   \brief  Safely detaches/orphans all children.
        */
        void DetachAllChildren();
        
        /**
        *   \brief  Helper function for detaching this node from it's parent.
        */
        void DetachFromParent();
        
        /**
        *   \brief            Retrieves the first child who has the given name, or nullptr if a child is not found.
        *   \param  name [in] The name of the child to return.
        *   \param  recursive [in] Whether or not to do a deep recursive search. False by default.
        */
        SceneNode* FindFirstChild(const char* name, bool recursive = false);

        /**
        *   \brief  Retrieves the first child node with the given component.
        */
        SceneNode* FindFirstChildWithComponent(const char* componentName, bool recursive = false);

        template <typename T>
        SceneNode* FindFirstChildWithComponent(bool recursive = false)
        {
            return this->FindFirstChildWithComponent(T::Name, recursive);
        }


        /// Determines whether or not the given node is an ancestor.
        bool IsAncestor(const SceneNode &other) const;

        /// Determines whether or not the given node is a descendant.
        bool IsDescendant(const SceneNode &other) const;

        /// Determines whether or not the given scene node is related to this node (is an ancestor or descendant).
        bool IsRelated(const SceneNode &other) const { return this->IsAncestor(other) || this->IsDescendant(other); } 


        /**
        *   \brief  Retrieves the position of the node relative to the parent.
        *   \return The position of the node relative to the parent.
        */
        const glm::vec3 & GetPosition() const;
        
        /**
        *   \brief                Sets the position of the node relative to the parent.
        *   \param  position [in] The new relative position of the node.
        */
        void SetPosition(const glm::vec3 &position);
        void SetPosition(float x, float y, float z) { this->SetPosition(glm::vec3(x, y, z)); }
        
        /**
        *   \brief  Retrieves the world/absolute position of node.
        */
        glm::vec3 GetWorldPosition() const;
        
        /**
        *   \brief  Sets the world/absolute position of the node.
        */
        void SetWorldPosition(const glm::vec3 &worldPosition);
        void SetWorldPosition(float x, float y, float z) { this->SetWorldPosition(glm::vec3(x, y, z)); }
        
        
        /**
        *   \brief  Retrieves the orientation of the node relative to the parent, as a quaternion.
        *   \return A quaternion representing the orientation of the node relative to the parent.
        */
        const glm::quat & GetOrientation() const;
        
        /**
        *   \brief                   Sets the orientation of the node relative to the parent.
        *   \param  orientation [in] The new orientation of the node.
        */
        void SetOrientation(const glm::quat &orientation);
        
        /**
        *   \brief  Retrieves the world/absolute orientation of the node.
        */
        glm::quat GetWorldOrientation() const;
        
        /**
        *   \brief  Sets the world/absolute orientation of the node.
        */
        void SetWorldOrientation(const glm::quat &worldOrientation);
        
        
        /**
        *   \brief  Retrieves the scale of the node relative to the parent.
        *   \return The scale of the node relative to the parent.
        */
        const glm::vec3 & GetScale() const;
        
        /**
        *   \brief             Sets the scale of the node relative to the parent.
        *   \param  scale [in] The new scale of the node relative to the parent.
        */
        void SetScale(const glm::vec3 &scale);
        void SetScale(float x, float y, float z) { this->SetScale(glm::vec3(x, y, z)); }
        
        /**
        *   \brief  Retrieves the world/absolute scale of the node.
        *   \return The world/absolute scale of the node.
        */
        glm::vec3 GetWorldScale() const;
        
        /**
        *   \brief                  Sets the world/absolute scale of the node.
        *   \param  worldScale [in] The new world/absolute scale of the node.
        */
        void SetWorldScale(const glm::vec3 &worldScale);
        void SetWorldScale(float x, float y, float z) { this->SetWorldScale(glm::vec3(x, y, z)); }
        

        /**
        *   \brief              Looks at a point in the world.
        *   \param  target [in] The position in the world to look at.
        *   \param  up     [in] The up direction. Defaults to (0, 1, 0). Must be normalized.
        *
        *   \remarks
        *       For efficiency, the up vector is not normalized internally.
        */
        void LookAt(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));

        /**
        *   \brief  Looks at another scene node.
        *   \param  target [in] The other scene node to look at.
        *   \param  up     [in] The up direction. Defaults to (0, 1, 0). Must be normalized.
        */
        void LookAt(const SceneNode &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));

        
        /**
        *   \brief  Retrieves a normalised vector pointing in the forward direction of the node.
        */
        glm::vec3 GetForwardVector() const;
        
        /**
        *   \brief  Retrieves a normalised vector pointing in the right direction of the node.
        */
        glm::vec3 GetRightVector() const;
        
        /**
        *   \brief  Retrieves a normalised vector pointing in the up direction of the node.
        */
        glm::vec3 GetUpVector() const;


        /**
        *   \brief  Retrieves the normalised vector pointing in the forward direction of the node, in world coordinates.
        */
        glm::vec3 GetWorldForwardVector() const;

        /**
        *   \brief  Retrieves the normalised vector pointing in the right direction of the node, in world coordinates.
        */
        glm::vec3 GetWorldRightVector() const;

        /**
        *   \brief  Retrieves the normalised vector pointing in the up direction of the node, in world coordinates.
        */
        glm::vec3 GetWorldUpVector() const;



    
        /**
        *   \brief  Calculates the transformation matrix for this object.
        */
        glm::mat4 GetTransformMatrix() const;
        
        /**
        *   \brief  Calculates a transformation matrix for this object, in world space.
        */
        glm::mat4 GetWorldTransform() const;

        /**
        *   \brief                       Retrieves the Bullet transform for use with physics.
        *   \param  worldTransform [out] A reference to the Bullet btTransform object that will receive the world transformation.
        */
        void GetWorldTransform(btTransform &worldTransform) const;

        /**
        *   \brief                      Sets the world transform of the scene node based on the given btTransform object.
        *   \param  worldTransform [in] A reference to the btTransform object containing the new world transformation.
        *   \param  offset         [in] Physics objects can be given a center of mass offset. This represents that offset.
        */
        void SetWorldTransform(const btTransform &worldTransform);
        
        
        /**
        *   \brief  Translates the node.
        */
        void Translate(const glm::vec3 &offset);
        
        /**
        *   \brief  Rotates the node.
        */
        void Rotate(float angleDegrees, const glm::vec3 &axis);
        
        /**
        *   \brief  Scales the node.
        */
        void Scale(const glm::vec3 &scale);


        /**
        *   \brief  Translates the node based along the world coordinates.
        */
        void TranslateWorld(const glm::vec3 &offset);


        /**
        *   \brief  Moves the entity in the direction of its forward vector.
        *   \param  distance [in] The distance to move.
        */
        void MoveForward(float distance) { this->Translate(glm::vec3(0.0f, 0.0f, -distance)); }

        /**
        *   \brief  Moves the entity in the direction of its right vector.
        *   \param  distance [in] The distance to move.
        */
        void MoveRight(float distance) { this->Translate(glm::vec3(distance, 0.0f, 0.0f)); }

        /**
        *   \brief  Moves the entity in the direction of its up vector.
        *   \param  distance [in] The distance to move.
        */
        void MoveUp(float distance) { this->Translate(glm::vec3(0.0f, distance, 0.0f)); }
        

        /**
        *   \brief  Rotates the entity around it's local x axis.
        *   \param  angleDegress [in] The angle in degrees to rotate.
        */
        void RotateX(float angleDegrees) { this->Rotate(angleDegrees, glm::vec3(1.0f, 0.0f, 0.0f)); }

        /**
        *   \brief  Rotates the entity around it's local y axis.
        *   \param  angleDegress [in] The angle in degrees to rotate.
        */
        void RotateY(float angleDegrees) { this->Rotate(angleDegrees, glm::vec3(0.0f, 1.0f, 0.0f)); }

        /**
        *   \brief  Rotates the entity around it's local z axis.
        *   \param  angleDegress [in] The angle in degrees to rotate.
        */
        void RotateZ(float angleDegrees) { this->Rotate(angleDegrees, glm::vec3(0.0f, 0.0f, 1.0f)); }



        /// Performs a linear interpolation of the node's position.
        void InterpolatePosition(const glm::vec3 &dest, float a);



        /**
        *   \brief  Retrieves a pointer to the component as specified by 'T'.
        *   \return A pointer to the component as specified by 'T", or null if the component does not exist.
        */
        template <typename T>
        T * GetComponent()
        {
            return static_cast<T*>(this->GetComponentByName(T::Name));
        }

        template <typename T>
        const T* GetComponent() const
        {
            return static_cast<const T*>(this->GetComponentByName(T::Name));
        }

        
        /**
        *   \brief            Retrievse a component based on it's name.
        *   \param  name [in] The name of the component to retrieve.
        *   \return           A pointer to the component whose name is that of 'name'. Returns null if no such component exists.
        */
        Component* GetComponentByName(const char *name)
        {
            auto item = this->components.Find(name);
            if (item != nullptr)
            {
                return item->value;
            }

            return nullptr;
        }

        const Component* GetComponentByName(const char* name) const
        {
            return const_cast<SceneNode*>(this)->GetComponentByName(name);
        }

        
        /**
        *   \brief  Adds a component of the type given by 'T'.
        *   \return A pointer to the new component.
        *
        *   \remarks
        *       If a component of the same type already exists, the existing one is returned and is NOT overwritten.
        */
        template <typename T>
        T * AddComponent()
        {
            // A component of the same type can't already exist.
            auto component = this->GetComponent<T>();
            if (component == nullptr)
            {
                component = new T(*this);
                this->components.Add(T::Name, component);

                this->OnComponentAttached(*component);
            }

            return component;
        }
        
        /**
        *   \brief  Removes the component of the type given by 'T'.
        */
        template <typename T>
        void RemoveComponent()
        {
            auto component = this->GetComponent<T>();
            if (component != nullptr)
            {
                // The OnComponentDetached() event needs to be called before destruction, but after it's been removed.
                this->components.Remove(T::Name);
                this->OnComponentDetached(*component);

                delete component;
            }
        }
        
        /**
        *   \brief  Removes every component.
        */
        void RemoveAllComponents()
        {
            // The quickest way to do this would be to delete everything in one go. However, we need to fire events in a
            // consistent manner. The way it needs to be done if to first remove the event from the list, then call the
            // event handler. Batching everything in one go won't allow this. Thus, we do it using a slightly slower, but 
            // consistent method.
            while (this->components.count > 0)
            {
                auto component = this->components.buffer[0]->value;
                assert(component != nullptr);

                this->components.RemoveByIndex(0);
                this->OnComponentDetached(*component);

                delete component;
            }
        }

        /**
        *   \brief  Determines whether or not the node has the component as specified by 'T'.
        *   \return True if the scene node has the component; false otherwise.
        */
        inline bool HasComponent(const char *componentName)
        {
            return this->GetComponentByName(componentName) != nullptr;
        }

        template <typename T>
        bool HasComponent()
        {
            return this->HasComponent(T::Name);
        }
        

        /**
        *   \brief  Retrieves the data pointer for the given key.
        *
        *   If a data pointer at the given key can not be found, null is returned, casted to T.
        *
        *   Ideally, this should not be called too often since it needs to access a map.
        */
        template <typename T>
        T* GetDataPointer(size_t key)
        {
            auto item = this->dataPointers.Find(key);
            if (item != nullptr)
            {
                return static_cast<T*>(item->value);
            }

            return nullptr;
        }
        
        /**
        *   \brief  Sets the data pointer for the given key.
        */
        void SetDataPointer(size_t key, void *data)
        {
            this->dataPointers.Add(key, data);
        }
        
        /**
        *   \brief  Removes a data pointer from the scene node for the given key.
        *
        *   Note that this does not delete the data at the given pointer. This is the responsibility of the
        *   caller.
        */
        void RemoveDataPointer(size_t key)
        {
            this->dataPointers.RemoveByValue(key);
        }


        /**
        *   \brief  Sets the layer this scene node is sitting on. Will call the OnLayerChanged() event.
        */
        void SetLayer(unsigned int layer);

        /**
        *   \brief  Retrieves the layer this scene node is sittign on.
        */
        unsigned int GetLayer() const { return this->layer; }


        /**
        *   \brief  Sets the scene of this node.
        *
        *   \remarks
        *       This will also set the scene for any children. Be aware of this in the event that the children are
        *       already attached to a different scene.
        */
        void SetScene(Scene *scene);

        /**
        *   \brief  Retrieves the scene this scene node is attached to, if any.
        *
        *   \remarks
        *       This will transcend the parent nodes if required.
        */
        Scene * GetScene();

        /**
        *   \brief  Removes the scene node from it's current scene.
        */
        void RemoveFromScene();


        /**
        *   \brief  Sets whether or not the scene node is static.
        */
        void SetStatic(bool isStatic);

        /**
        *   \brief  Determines whether or not the node is static.
        */
        bool IsStatic() const;

        /**
        *   \brief                 Determines if the scene node is visible.
        *   \param  recursive [in] Whether or not the visibility should be check recursively. If a parent is invisible, so is it's children.
        */
        bool IsVisible(bool recursive = true) const;

        /**
        *   \brief  Sets whether or not the scene node is visible.
        */
        void SetVisible(bool isVisible);

        /**
        *   \brief  Makes the scene node visible.
        */
        inline void Show() { this->SetVisible(true); }

        /**
        *   \brief  Makes the scene node invisible.
        */
        inline void Hide() { this->SetVisible(false); }


        /// Disables position inheritance.
        void DisablePositionInheritance();

        /// Enables position inheritance.
        void EnablePositionInheritance();

        /// Determines whether or not position inheritance is enabled.
        bool IsPositionInheritanceEnabled() const { return this->inheritPosition; }


        /// Disables orientation inheritance.
        void DisableOrientationInheritance();

        /// Enables orientation inheritance.
        void EnableOrientationInheritance();

        /// Determines whether or not orientation inheritance is enabled.
        bool IsOrientationInheritanceEnabled() const { return this->inheritOrientation; }


        /// Disables scale inheritance.
        void DisableScaleInheritance();

        /// Enables scale inheritance.
        void EnableScaleInheritance();

        /// Determines whether or not scale inheritance is enabled.
        bool IsScaleInheritanceEnabled() const { return this->inheritScale; }


        /// Sets the application-defined type ID of this scene node.
        ///
        /// @param newTypeID [in] The new type ID.
        ///
        /// @remarks
        ///     Do SetTypeID(GetTypeID() | SomeFlag), etc to do bitwise modifications.
        void SetTypeID(unsigned int newTypeID) { this->typeID = newTypeID; }

        /// Retrieves the application-defined type ID of this scene node.
        unsigned int GetTypeID() const { return this->typeID; }



    // Component helpers. Use these to make attaching specific components a little easier...
    public:

        /**
        *   \brief             Helper for adding a model component.
        *   \param  model [in] A pointer to the model to attach to the scene node.
        *   \return            A pointer to the model component.
        *
        *   \remarks
        *       If a model component is already attached, the existing model is replaced.
        */
        ModelComponent* AddModelComponent(Model* model);

        /// Helper for adding a perspective projection camera component.
        /// @param fov    [in] The field-of-view of the camera.
        /// @param aspect [in] The aspect ratio of the camera.
        /// @param zNear  [in] The position of the near plane.
        /// @param zFar   [in] The position of the far plane.
        CameraComponent* Add3DCameraComponent(float fov, float aspect, float zNear, float zFar);

        /// Helper for adding an ambient light component.
        /// @param colour [in] The colour of the light.
        /// @param r      [in] The red component.
        /// @param g      [in] The green component.
        /// @param b      [in] The blue component.
        AmbientLightComponent* AddAmbientLightComponent(float r, float g, float b);
        AmbientLightComponent* AddAmbientLightComponent(const glm::vec3 &colour);

        /// Helper for adding a directional light component.
        /// @param colour [in] The colour of the light.
        /// @param r      [in] The red component.
        /// @param g      [in] The green component.
        /// @param b      [in] The blue component.
        DirectionalLightComponent* AddDirectionalLightComponent(float r, float g, float b);
        DirectionalLightComponent* AddDirectionalLightComponent(const glm::vec3 &colour);

        /// Helper for adding a point light component.
        /// @param colour [in] The colour of the light.
        /// @param r      [in] The red component.
        /// @param g      [in] The green component.
        /// @param b      [in] The blue component.
        PointLightComponent* AddPointLightComponent(float r, float g, float b);
        PointLightComponent* AddPointLightComponent(const glm::vec3 &colour);

        
    // Events. Use these to make posting events a bit easier.
    public:

        void OnAttach(SceneNode& childNode);
        void OnDetach(SceneNode& childNode);
        void OnDestroy();
        void OnTransform();
        void OnScale();
        void OnLayerChanged(unsigned int prevLayer);
        void OnSceneChanged(Scene *prevScene);
        void OnStaticChanged();
        void OnVisibleChanged();
        void OnComponentAttached(Component& component);
        void OnComponentDetached(Component& component);
        void OnUpdate(double deltaTimeInSeconds);
        void OnContact(SceneNode &other, const btManifoldPoint &pt);


    public:

        /**
        *   \brief              Helper for setting the parent. Do not call this function directly - it is only used internally.
        *   \param  parent [in] A pointer to the new parent node. Can be null.
        *
        *   \remarks
        *       Do not call this function directly. Instead use Attach()/Detach() to set the parent.
        */
        void _SetParent(SceneNode *parent);

        void _SetPrevSibling(SceneNode* newPrevSibling);
        void _SetNextSibling(SceneNode* newNextSibling);



    private:

        /// Locks event posting. Any events that are posted while events are locked will be lost.
        void LockEvents();

        /// Unlocks event posting.
        void UnlockEvents();

        /// Determines whether or not events are locked.
        bool EventsLocked() const;
        


    private:

        /// The name of this node. Should usually be unique, but doesn't need to be. This can be modified, so we'll use a String object
        /// to make things easier.
        GTCore::String name;

        /// The parent of the scene node. If this is null, it is a root object.
        SceneNode* parent;

        /// A pointer to the first child node.
        SceneNode* firstChild;

        /// A pointer to the last child node.
        SceneNode* lastChild;

        /// A pointer to the previous sibling.
        SceneNode* prevSibling;

        /// A pointer to the next sibling.
        SceneNode* nextSibling;


        /// The list of pointers of the event handlers that are attached to this node. This should usually always have at
        /// least a single entry, but doesn't have to.
        GTCore::List<SceneNodeEventHandler*> eventHandlers;

        /// A map of components. More than one type of component is not allowed. We index this map by the component name.
        GTCore::Dictionary<Component*> components;

        /// A map of data pointers. A SceneNode should not be inheritted. Instead, additional data can be attached to the scene
        /// node with a size_t as it's key. We use a size_t so we can use a pointer as a key if needed. This could be useful for
        /// component-specific data (use a key equal to the pointer to the component).
        GTCore::Map<size_t, void*> dataPointers;

        /// The index of the layer this scene node is sitting on. Nodes with a larger layer index will be rendered on top of
        /// those with a lower index.
        unsigned int layer;

        /// The scene this node is attached to, if any. Usually, a scene node is part of a scene. We need to keep track of the
        /// scene so we can notify it of certain events, such as when a component is added or removed. This can be null, in
        /// which case it's assumed that the node is not part of a scene. If a scene determines that it's nodes need scene awareness,
        /// it is the responsibility of the scene to call SceneNode::SetScene() manually. If left unset, the scene node will go
        /// with the assumption that it is not part of a scene.
        Scene* scene;

        /// Keeps track of whether or not the scene node is static. The scene node itself doesn't actually use this attribute.
        /// Instead, it is used so that scene managers can do optimizations for static nodes. When this is set to true, a scene
        /// manager will not need to perform an update step on the scene node. This will offer some great performance benefits.
        /// By default, a scene node is NOT static. Therefore, resource/level loaders will need to ensure they set this appropriately.
        bool isStatic;

        /// Keeps track of whether or not the node is visible. True by default.
        bool isVisible;

        /// Keeps track of whether or not position inheritance is enabled. True by default.
        bool inheritPosition;

        /// Keeps track of whether or not orientation inheritance is enabled. True by default.
        bool inheritOrientation;

        /// Keeps track of whether or not scale inheritance is enabled. True by default.
        bool inheritScale;


        /// The position of the node, relative to the parent node.
        glm::vec3 position;

        /// The rotation of the node.
        glm::quat orientation;

        /// The scale of the node, relative to the parent node.
        glm::vec3 scale;


        /// The counter used for event locks. If it is > 0, the events are locked. Otherwise they are unlocked. Defaults to 0. LockEvents()
        /// will increment, whereas UnlockEvents() will decrement.
        int eventLockCounter;


        /// Sometimes it will be required to able to generically identity scene nodes. This member is an integer that can be set by client
        /// applications for identification purposes. It is controlled with SetTypeID() and GetTypeID(). Defaults to 0.
        unsigned int typeID;


    
    private:    // No copying.
        SceneNode(const SceneNode &);
        SceneNode & operator=(const SceneNode &);
    };
}

#endif
