// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_SceneNodeComponentDescriptorLibrary
#define GT_SceneNodeComponentDescriptorLibrary

#include "SceneNodeComponentDescriptor.hpp"
#include <GTLib/Map.hpp>

namespace GT
{
    /// Class containing a list of component descriptors which are used to allocated component instances.
    class SceneNodeComponentDescriptorLibrary
    {
    public:

        /// Constructor.
        SceneNodeComponentDescriptorLibrary();

        /// Destructor.
        ~SceneNodeComponentDescriptorLibrary();


        /// Registers the given descriptor so that a component can be allocated based off it.
        ///
        /// @remarks
        ///     If a descriptor of the same type ID has already been registered it will be replaced.
        void RegisterDescriptor(const SceneNodeComponentDescriptor &descriptor);

        /// Retrieves a component descriptor by the type ID.
        ///
        /// @param type [in] The type ID of the component descriptor to retrieve.
        const SceneNodeComponentDescriptor* GetDescriptorByType(SceneNodeComponentTypeID type) const;


        /// Creates an instance of a component based on the given component type ID.
        ///
        /// @param type [in] The type ID of the component to create.
        ///
        /// @remarks
        ///     If a descriptor of the given type has not been created, this will fail and return null.
        SceneNodeComponent* CreateComponent(SceneNodeComponentTypeID type);

        /// Deletes the given component.
        void DeleteComponent(SceneNodeComponent* pComponent);


    private:

        /// The list of registered descriptors.
        GTLib::Map<SceneNodeComponentTypeID, const SceneNodeComponentDescriptor*> m_descriptors;
    };
}

#endif
