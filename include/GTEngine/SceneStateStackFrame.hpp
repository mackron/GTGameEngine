
#ifndef __GTEngine_SceneStateStackFrame_hpp_
#define __GTEngine_SceneStateStackFrame_hpp_

#include <GTCore/Vector.hpp>
#include <GTCore/Map.hpp>
#include <GTCore/Serializer.hpp>
#include <GTCore/Deserializer.hpp>

namespace GTEngine
{
    class Scene;
    class SceneStateStackBranch;

    class SceneStateStackFrame
    {
    public:

        /// Constructor.
        SceneStateStackFrame(SceneStateStackBranch &branch, const GTCore::Vector<uint64_t> &insertIDs, const GTCore::Vector<uint64_t> &deleteIDs, const GTCore::Vector<uint64_t> &updateIDs);

        /// Destructor.
        ~SceneStateStackFrame();


        /// Retrieves a reference to the scene that owns the state stack that subsequently owns this frame.
              Scene & GetScene();
        const Scene & GetScene() const;



    private:

        /// Serializes the given scene node.
        bool SerializeSceneNode(uint64_t sceneNodeID, GTCore::Serializer &serializer) const;


    private:

        /// A reference to the branch that owns this frame.
        SceneStateStackBranch &branch;

        /// The map containing the serialized data of inserted scene nodes. Indexed by the scene node ID.
        GTCore::Map<uint64_t, GTCore::BasicSerializer*> serializedInserts;

        /// The map containing the serialized data of deleted scene nodes. Indexed by the scene node ID.
        GTCore::Map<uint64_t, GTCore::BasicSerializer*> serializedDeletes;

        /// The map containing the serialized data of updated scene nodes. Indexed by the scene node ID.
        GTCore::Map<uint64_t, GTCore::BasicSerializer*> serializedUpdates;



    private:    // No copying.
        SceneStateStackFrame(const SceneStateStackFrame &);
        SceneStateStackFrame & operator=(const SceneStateStackFrame &);
    };
}

#endif
