// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneNodeClass_hpp_
#define __GTEngine_SceneNodeClass_hpp_

#include "Serialization.hpp"
#include "SceneNode.hpp"
#include <GTCore/Vector.hpp>
#include <GTCore/Map.hpp>


namespace GTEngine
{
    /// Class representing the definition of a hierarchy of scene nodes.
    ///
    /// Think of this class as a "prefab".
    class SceneNodeClass
    {
    public:

        /// Constructor.
        SceneNodeClass();
        
        /// Destructor.
        ~SceneNodeClass();


        /// Sets the class to that of the given node and it's children.
        ///
        /// @param sceneNode [in] The scene node that will act as the definition of the new prefab.
        void SetFromSceneNode(const SceneNode &sceneNode);


        /// Retrieves a reference to the internal list of serializers.
        const GTCore::Map<uint64_t, GTCore::BasicSerializer*> & GetSerializers() const { return this->serializers; }

        /// Retrieves a reference to the hierarchy.
        const GTCore::Map<uint64_t, uint64_t> & GetHierarchy() const { return this->hierarchy; }



        ////////////////////////////////////////////
        // Serialization.

        /// Serializes the scene node class.
        bool Serialize(GTCore::Serializer &serializer);

        /// Deserializes the scene node class.
        bool Deserialize(GTCore::Deserializer &deserializer);



    private:
        
        /// Clears the prefab leaving it completely empty.
        void Clear();

        /// Recursively adds the given scene node and it's children.
        ///
        /// @param sceneNode   [in] The scene node to add.
        /// @param parentIndex [in] The index of the parent in the hierarchy. Set to 0 if the node does not have a parent.
        void AddSceneNode(const SceneNode &sceneNode, uint64_t id, uint64_t parentID);



    private:

        /// The serialized data of every scene node.
        //GTCore::Vector<GTCore::BasicSerializer*> serializers;

        /// The serialized data of every scene node, mapped to a persistent ID.
        GTCore::Map<uint64_t, GTCore::BasicSerializer*> serializers;

        /// The hierarchy. These point to indexes in sceneNodeSerializers.
        GTCore::Map<uint64_t, uint64_t> hierarchy;

        /// The next unique ID.
        uint64_t nextID;
    };
}

#endif