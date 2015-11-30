// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_GarbageCollector
#define GT_GUI_GarbageCollector

#include <GTEngine/Core/List.hpp>

namespace GTGUI
{
    class Element;
    class GUIMesh;

    /// Structure representing an item in the garbage collector that's waiting to be collected.
    template <typename T>
    struct GCItem
    {
        GCItem(T &object, int counter)
            : object(&object), counter(counter)
        {
        }

        T*  object;
        int counter;
    };

    /// Class representing a simple garbage collector.
    ///
    /// Every Server object has it's own garbage collector. It's main use it to make it easier to manage dynamics resources
    /// between the stepping and rendering threads. For example, elements can not be deleted straight away. Instead they have
    /// to be marked for collection and then deleted once they have finished rendering.
    ///
    /// Every object marked for collection has a counter associated with it. Every time Collect() is called, the counter is
    /// decremented. When the counter reaches 0, the object is deleted. Every object has a counter of 1 by default, which
    /// means it will be collected after the SECOND call to Collect(). 
    ///
    /// Objects are delete with 'delete'.
    class GarbageCollector
    {
    public:
        
        /// Constructor.
        GarbageCollector();

        /// Destructor.
        ///
        /// @remarks
        ///     Anything marked for collection will the deleted regardless of their current counter value.
        ~GarbageCollector();


    public:

        /// Collects all garbage.
        ///
        /// @param force [in] Whether or not the collector should for the deletion of all marked objects regardless of their internal counters.
        ///
        /// @remarks
        ///     Objects are deleted with 'delete' nothing special happens when deleting. It is assumed element's will be orphaned
        ///     before they are marked for collection.
        void Collect(bool force = false);


        /// Marks an element for collection.
        ///
        /// @param element [in] A reference to the element being collected.
        void MarkForCollection(Element &elementm, int counter = 1);

        /// Marks a mesh for collection.
        ///
        /// @param mesh [in] A reference to the mesh being collected.
        void MarkForCollection(GUIMesh &mesh, int counter = 1);



    private:

        /// The list of garbage elements.
        GT::List<GCItem<Element>> garbageElements;

        /// The list of garbage meshes.
        GT::List<GCItem<GUIMesh>> garbageMeshes;
    };
}

#endif
