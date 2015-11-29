// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_GameObject
#define GT_GameObject

#include "SceneNode.hpp"

namespace GT
{
    /// Base class for game objects.
    ///
    /// Sometimes using inheritance is the best way to get game objects up and running. This class makes it easier for games to use an
    /// inheritance based design to represent their game objects.
    ///
    /// Games do not ever need to use this class if they don't want to. It simply exists for convenience.
    ///
    /// The main property of a GameObject is the scene node. A reference to the scene node can be retrieved with GetSceneNode().
    ///
    /// This class also contains virtual methods for handling events from the main scene node. This can make it a lot easier to handle
    /// things like updating and movements.
    class GameObject
    {
    public:

        /// Default constructor.
        GameObject();

        /// Destructor.
        virtual ~GameObject();


        /// Retrieves a reference to the scene node.
              SceneNode & GetSceneNode()       { return this->sceneNode; }
        const SceneNode & GetSceneNode() const { return this->sceneNode; }


        /// Sets the name of the object.
        void SetName(const char* name) { this->sceneNode.SetName(name); }

        /// Retrieves the name of the object.
        const char* GetName() const { return this->sceneNode.GetName(); }



        /// Sets the position of the object relative to the parent scene node.
        void SetPosition(float x, float y, float z) { this->sceneNode.SetPosition(x, y, z); }
        void SetPosition(const glm::vec3 &position) { this->sceneNode.SetPosition(position); }

        /// Retrieves the position of the object relative to the parent node.
        const glm::vec3 & GetPosition() const { return this->sceneNode.GetPosition(); }



        /// Sets the world position of the object.
        void SetWorldPosition(float x, float y, float z) { this->sceneNode.SetWorldPosition(x, y, z); }
        void SetWorldPosition(const glm::vec3 &position) { this->sceneNode.SetWorldPosition(position); }

        /// Retrieves the world position of the object.
        glm::vec3 GetWorldPosition() const { return this->sceneNode.GetWorldPosition(); }



        /// Shows the game object.
        void Show() { this->sceneNode.Show(); }

        /// Hides the game object.
        void Hide() { this->sceneNode.Hide(); }

        /// Determines whether or not the game object is visible.
        bool IsVisible() const { return this->sceneNode.IsVisible(); }



        /////////////////////////////////////////////////////////////////////////
        // Garbage collection

        /// Retrieves the GC counter. Should only need to be used by the garbage collector.
        size_t GetGCCounter() const { return this->gcCounter; }

        /// Increments the GC counter.
        void IncrementGCCounter() { ++this->gcCounter; }

        /// Decrements the GC counter.
        void DecrementGCCounter() { --this->gcCounter; }



    // Events from the main scene node.
    public:

        /// Called after the main scene node is transformed.
        virtual void OnTransform();

        /// Called after the main scene node is scaled.
        virtual void OnScale();

        /// Called after the main scene node changes scenes.
        ///
        /// @remarks
        ///     This will also be called when the scene node is removed from a scene. Check the return value of sceneNode.GetScene() to
        ///     determine whether or not the scene node is no longer in the scene.
        virtual void OnSceneChanged();

        /// Called after the scene nodes changes between static/dynamic states.
        virtual void OnStaticChanged();

        /// Called after the scene node changes visibility states.
        virtual void OnVisibleChanged();

        /// Called after a component has been attached to the scene node.
        virtual void OnComponentAttached(Component &component);

        /// Called after a component has been removed from the scene node.
        virtual void OnComponentDetached(Component &component);

        /// Called when the game object is updated.
        ///
        /// @param deltaTimeInSeconds [in] The time in seconds between frames for time-based operations.
        virtual void OnUpdate(double deltaTimeInSeconds);



    protected:

        /// The scene node of the game object.
        SceneNode sceneNode;


    private:

        /// The event handler for the main scene node. This simply passes the events back to the main game object.
        class GameObjectEventHandler : public SceneNodeEventHandler
        {
        public:

            /// Constructor.
            GameObjectEventHandler(GameObject &object)
                : object(object)
            {
            }


            void OnTransform(SceneNode &)
            {
                this->object.OnTransform();
            }

            void OnScale(SceneNode &)
            {
                this->object.OnScale();
            }

            void OnSceneChanged(SceneNode &, Scene*)
            {
                this->object.OnSceneChanged();
            }

            void OnStaticChanged(SceneNode &)
            {
                this->object.OnStaticChanged();
            }

            void OnVisibleChanged(SceneNode &)
            {
                this->object.OnVisibleChanged();
            }

            void OnComponentAttached(SceneNode &, Component &component)
            {
                this->object.OnComponentAttached(component);
            }

            void OnComponentDetached(SceneNode &, Component &component)
            {
                this->object.OnComponentDetached(component);
            }

            void OnUpdate(SceneNode &, double dtSeconds)
            {
                this->object.OnUpdate(dtSeconds);
            }


        private:

            /// A reference to the game object that we'll be passing events to.
            GameObject &object;


        private:
            GameObjectEventHandler(const GameObjectEventHandler &);
            GameObjectEventHandler & operator=(const GameObjectEventHandler &);

        } sceneNodeEventHandler;


        /// The counter to use for garbage collection.
        size_t gcCounter;


    private:    // No copying.
        GameObject(const GameObject &);
        GameObject & operator=(const GameObject &);
    };
}

#endif
