// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_SceneNodeComponentDescriptor
#define GT_SceneNodeComponentDescriptor

#include "SceneNodeComponentTypes.hpp"
#include "SceneNodeComponent.hpp"

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


        /// Creates an instance of the component this descriptor is associated with.
        ///
        /// @remarks
        ///     The default implementation returns null. All descriptors should implement this.
        virtual SceneNodeComponent* CreateComponent() const;

        /// Deletes the given component that was created with a previous call to CreateComponent().
        ///
        /// @remarks
        ///     The default implementation will delete the component as "delete pComponent".
        virtual void DeleteComponent(SceneNodeComponent* pComponent) const;
    };
}

#endif
