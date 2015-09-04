// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_SceneNodeComponent
#define GT_SceneNodeComponent

#include "SceneNodeComponentTypes.hpp"

namespace GT
{
    class SceneNode;
    class SceneNodeComponentDescriptor;

    /// Base class for scene node components.
    class SceneNodeComponent
    {
    public:

        /// Constructor.
        SceneNodeComponent(const SceneNodeComponentDescriptor &descriptor);

        /// Destructor.
        virtual ~SceneNodeComponent();


        /// Retrieves a reference to the descriptor that was passed to the constructor.
        const SceneNodeComponentDescriptor & GetDescriptor() const;


        /// Retrieves a pointer to the scene node this component is attached to.
        SceneNode* GetSceneNode() const;

        /// [Internal Use Only] Sets the scene node that owns this component.
        void _IUO_SetSceneNode(SceneNode* pSceneNode);


        /// Retrieves the component's type ID.
        ///
        /// @return The component's type ID.
        SceneNodeComponentTypeID GetTypeID() const;

        /// Retrieves the component's name.
        const char* GetName() const;


        /// [Internal Use Only] Retrieves the change flags.
        ///
        /// @return The current change flags.
        uint32_t _GetChangeFlags() const;

        /// [Internal Use Only] Sets the change flags.
        ///
        /// @param changeFlags [in] The new change flags.
        void _SetChangeFlags(uint32_t changeFlags);


    private:

        /// The descriptor that was passed to the constructor. This is used to describe the component such as describing it's name and unique ID.
        const SceneNodeComponentDescriptor &m_descriptor;

        /// A pointer to the scene node that owns this component. This is null by default and set with _SetSceneNode().
        SceneNode* m_pSceneNode;

        /// Flags that track the changes that have been made to the component. This is used when handling change events so that certain things can be optimized.
        uint32_t m_changeFlags;
    };
}

#endif


