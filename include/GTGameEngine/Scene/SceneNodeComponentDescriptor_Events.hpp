// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneNodeComponentDescriptor_Events_hpp_
#define __GT_SceneNodeComponentDescriptor_Events_hpp_

#include "SceneNodeComponentDescriptor.hpp"

namespace GT
{
    class SceneNodeComponentDescriptor_Events : public SceneNodeComponentDescriptor
    {
    public:

        /// Retrieves the type ID of the component.
        static SceneNodeComponentTypeID TypeID();

        /// Retrieves the name of the component.
        static const char* Name();



    public:

        /// Constructor.
        SceneNodeComponentDescriptor_Events();

        /// Destructor.
        virtual ~SceneNodeComponentDescriptor_Events();


        /// Retrieves the unique ID of the component.
        virtual SceneNodeComponentTypeID GetTypeID() const;

        /// Retrieves the name of the component.
        virtual const char* GetName() const;


        /// @copydoc SceneNodeComponentDescriptor::CreateComponent()
        virtual SceneNodeComponent* CreateComponent() const;

        /// @copydoc SceneNodeComponentDescriptor::DeleteComponent()
        virtual void DeleteComponent(SceneNodeComponent* pComponent) const;
    };
}

#endif
