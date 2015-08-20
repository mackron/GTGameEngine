// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneNodeComponent_hpp_
#define __GT_SceneNodeComponent_hpp_

#include "SceneNodeComponentTypes.hpp"

namespace GT
{
    class SceneNodeComponentDescriptor;

    /// Base class for scene node components.
    class SceneNodeComponent
    {
    public:

        /// Constructor.
        SceneNodeComponent(const SceneNodeComponentDescriptor &descriptor);


        /// Retrieves a reference to the descriptor that was passed to the constructor.
        const SceneNodeComponentDescriptor & GetDescriptor() const;


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

        /// Flags that track the changes that have been made to the component. This is used when handling change events so that certain things can be optimized.
        uint32_t m_changeFlags;
    };
}

#endif


