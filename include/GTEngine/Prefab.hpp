// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Prefab
#define GT_Prefab

#include "Serialization.hpp"
#include "SceneNode.hpp"
#include <GTEngine/Core/Vector.hpp>
#include <GTEngine/Core/Map.hpp>


namespace GT
{
    /// Class representing a prefab.
    class Prefab
    {
    public:

        /// Constructor.
        Prefab(const char* absolutePath, const char* relativePath);
        
        /// Destructor.
        ~Prefab();


        /// Retrieves the absolute path of the prefab.
        const char* GetAbsolutePath() const;

        /// Retrieves the relative path of the prefab.
        const char* GetRelativePath() const;


        /// Sets the class to that of the given node and it's children.
        ///
        /// @param sceneNode [in] The scene node that will act as the definition of the new prefab.
        void SetFromSceneNode(const SceneNode &sceneNode);


        /// Retrieves a reference to the internal list of serializers.
        const Map<uint64_t, BasicSerializer*> & GetSerializers() const { return this->serializers; }

        /// Retrieves a reference to the hierarchy.
        const Map<uint64_t, uint64_t> & GetHierarchy() const { return this->hierarchy; }


        /// Retrieves a serializer by it's ID.
        ///
        /// @param id [in] The ID of the serializer to retrieve.
              BasicSerializer* GetSerializerByID(uint64_t id);
        const BasicSerializer* GetSerializerByID(uint64_t id) const;

        /// Retrieves the ID of the root item.
        uint64_t GetRootID() const;


        /// Clears the prefab leaving it completely empty.
        ///
        /// @remarks
        ///     This does not clear the value for creating unique IDs.
        void Clear();

        /// Recursively adds the given scene node and it's children.
        ///
        /// @param sceneNode   [in] The scene node to add.
        /// @param parentIndex [in] The index of the parent in the hierarchy. Set to 0 if the node does not have a parent.
        void AddSceneNode(const SceneNode &sceneNode, uint64_t id, uint64_t parentID);

        /// Adds the given scene node, but does not add it's children.
        ///
        /// @param sceneNode [in] A reference to the scene node to add.
        /// @param id        [in] The local ID to associate the scene node with in the prefab.
        /// @param parentID  [in] The local ID of the parent scene node.
        ///
        /// @return The local ID of the new scene node.
        ///
        /// @remarks
        ///     If parentID is 0, it assumes that it is the root node.
        uint64_t AddSingleSceneNode(const SceneNode &sceneNode, uint64_t id, uint64_t parentID);


        /// Retrieves the IDs of the children of the given node.
        ///
        /// @param parentID [in ] The local ID of the parent.
        /// @param childIDs [out] A reference to the vector that will receive the child IDs.
        void GetChildIDs(uint64_t parentID, Vector<uint64_t> &childIDs) const;



        ////////////////////////////////////////////
        // Serialization.

        /// Serializes the scene node class.
        bool Serialize(Serializer &serializer);

        /// Deserializes the scene node class.
        bool Deserialize(Deserializer &deserializer);

        /// A helper for serializing the prefab to the file.
        bool WriteToFile();



    private:

        /// The absolute path of the prefab.
        String absolutePath;

        /// The relative path of the prefab.
        String relativePath;

        /// The serialized data of every scene node, mapped to a persistent ID.
        Map<uint64_t, BasicSerializer*> serializers;

        /// The hierarchy. These point to indexes in sceneNodeSerializers.
        Map<uint64_t, uint64_t> hierarchy;

        /// The next unique ID.
        uint64_t nextID;
    };
}

#endif