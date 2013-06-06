// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_PrefabComponent_hpp_
#define __GTEngine_PrefabComponent_hpp_

#include "../Component.hpp"

namespace GTEngine
{
    /// A special component used for helping synchronize with prefabs.
    class PrefabComponent : public Component
    {
    public:

        /// Constructor.
        PrefabComponent(SceneNode &node);

        /// Destructor.
        ~PrefabComponent();


        /// Retrieves the relative path of the prefab the scene node is linked to.
        ///
        /// @return The relative path of the prefab the scene node is linked to.
        const char* GetPrefabRelativePath() const;

        /// Sets the relative path the scene node is linked to.
        ///
        /// @param relativePath [in] The relative path of the prefab the scene node is linked to.
        void SetPrefabRelativePath(const char* relativePath);


        /// Retrieves the local hierarchy ID of the scene node.
        uint64_t GetLocalHierarchyID() const;

        /// Sets the local hirerachy ID of the scene node.
        ///
        /// @param id [in] The new local hierarchy ID.
        void SetLocalHierarchyID(uint64_t id);



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTCore::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTCore::Deserializer &deserializer);



    private:

        /// The relative path of the prefab the scene node is linked to.
        GTCore::String prefabRelativePath;

        /// The local hierarchy ID of the scene node. This maps to an ID from the prefab.
        uint64_t localHierarchyID;



        GTENGINE_DECL_COMPONENT_ATTRIBS(PrefabComponent)
    };
}

#endif
