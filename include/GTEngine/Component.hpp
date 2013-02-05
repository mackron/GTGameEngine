// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Component_hpp_
#define __GTEngine_Component_hpp_

#include <GTCore/Serializer.hpp>
#include <GTCore/Deserializer.hpp>

#define GTENGINE_DECL_COMPONENT_START(className) \
    class className : public GTEngine::Component \
    { \
    public: \
        static const char* const Name; \
        const char* GetName() const \
        { \
            return className::Name; \
        } \
    private: \
        className(const className &);   \
        className & operator=(const className &);

#define GTENGINE_DECL_COMPONENT_END() \
    };

#define GTENGINE_IMPL_COMPONENT(className, name) \
    const char* const className::Name = name;

namespace GTEngine
{
    class SceneNode;

    /**
    *   \brief  Base class for components.
    *
    *   Components are attached to SceneNodes. They define what a scene node is, and what it does. For example, if a point
    *   light component is attached to a scene node, the node will emit a point light.
    */
    class Component
    {
    public:

        Component(SceneNode &node)
            : node(node)
        {
        }
        
        virtual ~Component()
        {
        }


        /// Retrieves the name of this component.
        virtual const char* GetName() const = 0;
        
        /// Retrieves a pointer to the node that this component is attached to.
              SceneNode & GetNode()       { return this->node; }
        const SceneNode & GetNode() const { return this->node; }


        /// Serializes the component.
        ///
        /// @param serializer [in] The serializer to write data to.
        ///
        /// @remarks
        ///     This does not need to preserve any information about the owner scene node or the component name.
        virtual void Serialize(GTCore::Serializer &serializer) const;

        /// Deserializes the component.
        ///
        /// @param deserializer [in] A reference to the deserializer to read data from.
        virtual void Deserialize(GTCore::Deserializer &deserializer);


        /// Called by inheritted classes when the component has changed so that the scene can be notified and events posted.
        virtual void OnChanged();


    protected:

        /// The node this component is attached to. This can not be changed after construction and can not be null. Thus,
        /// we can, and should, use a reference here.
        SceneNode &node;


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
    Component* CreateComponentByName(const char* componentName, SceneNode &hostSceneNode);
}

#endif
