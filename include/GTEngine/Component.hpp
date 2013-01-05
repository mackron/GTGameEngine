
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
        virtual void Serialize(GTCore::Serializer &serializer);

        /// Deserializes the component.
        ///
        /// @param deserializer [in] A reference to the deserializer to read data from.
        virtual void Deserialize(GTCore::Deserializer &deserializer);


    protected:

        /// The node this component is attached to. This can not be changed after construction and can not be null. Thus,
        /// we can, and should, use a reference here.
        SceneNode &node;


    private:    // No copying.
        Component(const Component &);
        Component & operator=(const Component &);
    };
}

#endif
