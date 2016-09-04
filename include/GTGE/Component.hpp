// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Component
#define GT_Component

#undef GetClassName

#define GTENGINE_IMPL_COMPONENT_ATTRIBS(className, name) \
    const char* const className::Name      = name; \
    const char* const className::ClassName = #className; \

#define GTENGINE_DECL_COMPONENT_ATTRIBS(className) \
    public: \
        static const char* const Name; \
        static const char* const ClassName; \
        const char* GetName() const \
        { \
            return className::Name; \
        } \
        const char* GetClassName() const \
        { \
            return className::ClassName; \
        } \
    private: \
        className(const className &);   \
        className & operator=(const className &);



namespace GT
{
    class Context;
    class SceneNode;
    class Serializer;
    class Deserializer;

    /**
    *   \brief  Base class for components.
    *
    *   Components are attached to SceneNodes. They define what a scene node is, and what it does. For example, if a point
    *   light component is attached to a scene node, the node will emit a point light.
    */
    class Component
    {
    public:

        /// Constructor.
        Component(SceneNode &node);
        
        /// Destructor.
        virtual ~Component();


        /// Retrieves the name of this component.
        virtual const char* GetName() const = 0;

        /// Retrieves the class name of this component.
        virtual const char* GetClassName() const = 0;

        
        /// Retrieves a pointer to the node that this component is attached to.
              SceneNode & GetNode()       { return this->node; }
        const SceneNode & GetNode() const { return this->node; }


        /// Retrieves a pointer to the game that owns this component.
        Context* GetContext();


        /// Serializes the component.
        ///
        /// @param serializer [in] The serializer to write data to.
        ///
        /// @remarks
        ///     This does not need to preserve any information about the owner scene node or the component name.
        virtual void Serialize(Serializer &serializer) const;

        /// Deserializes the component.
        ///
        /// @param deserializer [in] A reference to the deserializer to read data from.
        virtual void Deserialize(Deserializer &deserializer);

        /// Called after the owner scene node has finished deserializing.
        ///
        /// @remarks
        ///     This is needed so that components that depend on other components can do a post-processing step.
        virtual void OnPostSceneNodeDeserialized();


        /// Called by inheritted classes when the component has changed so that the scene can be notified and events posted.
        ///
        /// @param flags [in] A set of flags that specify what has actually changed.
        void OnChanged(unsigned int flags = 0);


        /// Increments the OnChanged() lock counter by 1.
        ///
        /// @remarks
        ///     OnChanged() calls will be locked so long as the lock counter is > 1. Make sure each call to LockOnChanged() has
        ///     a corresponding call to UnlockOnChanged().
        void LockOnChanged();

        /// Decrements the OnChanged() lock counter by 1.
        ///
        /// @remarks
        ///     This will never bring the counter down below 1. An assertion will fail if this is called while the lock counter
        ///     is equal to 0.
        void UnlockOnChanged();


    protected:

        /// The node this component is attached to. This can not be changed after construction and can not be null. Thus,
        /// we can, and should, use a reference here.
        SceneNode &node;

        /// A counter that is used to cancel calls to OnChanged(). When this is larger than 0, all calls to OnChanged() should
        /// do nothing. The purpose of this system is to allow components to essentially batch calls to OnChanged() instead of
        /// willy nilly posting it for every tiny change.
        int onChangedLockCounter;


    private:    // No copying.
        Component(const Component &);
        Component & operator=(const Component &);
    };



    /// Creates an instantiation of a component based on it's name.
    ///
    /// @param componentName [in] The name of the comonent to instantiate.
    /// @param hostSceneNode [in] The scene node to associate with the component.
    ///
    /// @return A pointer to the new component if successful, null otherwise.
    ///
    /// @remarks
    ///     The component will be created with 'new'. Delete it with 'delete'
    ///     @par
    ///     If 'name' does not correspond to a component defined by the engine, this will call Game::CreateCustomComponent().
    Component* CreateComponentByName(Context* pContext, const char* componentName, SceneNode &hostSceneNode);
}

#endif
