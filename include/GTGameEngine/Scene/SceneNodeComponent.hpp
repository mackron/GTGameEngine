// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneNodeComponent_hpp_
#define __GT_SceneNodeComponent_hpp_

#include "SceneNodeComponentTypes.hpp"

namespace GT
{
    /// Base class for scene node components.
    class SceneNodeComponent
    {
    public:

        /// Constructor.
        SceneNodeComponent(SceneNodeComponentTypeID typeID);


        /// Retrieves the type ID.
        ///
        /// @return The component's type ID.
        SceneNodeComponentTypeID GetTypeID() const;
        

        /// [Internal Use Only] Retrieves the change flags.
        ///
        /// @return The current change flags.
        uint32_t _GetChangeFlags() const;

        /// [Internal Use Only] Sets the change flags.
        ///
        /// @param changeFlags [in] The new change flags.
        void _SetChangeFlags(uint32_t changeFlags);


    private:

        /// The type ID.
        SceneNodeComponentTypeID m_typeID;

        /// Flags that track the changes that have been made to the component. This is used when handling change events so that certain things can be optimized.
        uint32_t m_changeFlags;
    };
}

#endif


