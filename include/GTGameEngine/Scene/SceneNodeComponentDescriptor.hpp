// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneNodeComponentDescriptor_hpp_
#define __GT_SceneNodeComponentDescriptor_hpp_

#include "SceneNodeComponentTypes.hpp"

namespace GT
{
    class SceneNode;


    /// Base class for component descriptors.
    class SceneNodeComponentDescriptor
    {
    public:

        /// Retrieves the type ID of the component.
        static SceneNodeComponentTypeID TypeID();

        /// Retrieves the name of the component.
        static const char* Name();



    public:

        /// Constructor.
        SceneNodeComponentDescriptor();

        /// Destructor.
        virtual ~SceneNodeComponentDescriptor();


        /// Retrieves the unique ID of the component.
        virtual SceneNodeComponentTypeID GetTypeID() const;

        /// Retrieves the name of the component.
        virtual const char* GetName() const;
    };
}

#endif
