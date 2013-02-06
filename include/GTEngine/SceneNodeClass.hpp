// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneNodeClass_hpp_
#define __GTEngine_SceneNodeClass_hpp_

#include "Serialization.hpp"
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



        ////////////////////////////////////////////
        // Serialization.

        /// Serializes the scene node class.
        bool Serialize(GTCore::Serializer &serializer);

        /// Deserializes the scene node class.
        bool Deserialize(GTCore::Deserializer &deserializer);



    private:
        
        /// Clears the prefab leaving it completely empty.
        void Clear();



    private:

        /// The serialized data of every scene node.
        GTCore::Vector<GTCore::BasicSerializer*> sceneNodeSerializers;

        /// The hierarchy. These point to indexes in sceneNodeSerializers.
        GTCore::Map<size_t, size_t> hierarchy;
    };
}

#endif