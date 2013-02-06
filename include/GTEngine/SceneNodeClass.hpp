
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