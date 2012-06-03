
#ifndef __GTEngine_GameObjectGC_hpp_
#define __GTEngine_GameObjectGC_hpp_

#include "GameObject.hpp"
#include <GTCore/List.hpp>

namespace GTEngine
{
    /// Class representing a garbage collector for GameObjects.
    ///
    /// The garbage collector uses a simple counter system. Each GameObject has a gcCounter attribute, which is set to 0 by default. The
    /// object will only be collected when the counter is 0. Objects can increment and decrement the counter where appropriate.
    ///
    /// The destructor will delete any marked objects regardless of the GC counters.
    ///
    /// Ensure a game object is not collected by more than one garbage collector.
    class GameObjectGC
    {
    public:

        /// Default constructor.
        GameObjectGC();

        /// Destructor.
        virtual ~GameObjectGC();


        /// Marks an object for collection.
        ///
        /// @param object [in] A reference to the game object to mark for collection.
        ///
        /// @remarks
        ///     The object will be deleted during Collect(), but only if the counter is 0. It will also be deleted during the garbage
        ///     collector's destructor regardless of it's counter.
        void MarkForCollection(GameObject &object);

        /// Collects any marked objects whose counters are 0.
        void Collect();


    private:

        /// The list of game objects that are marked for deletion.
        GTCore::List<GameObject*> objects;
    };
}

#endif
