// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIElementTree
#define GT_GUIElementTree

#include "GUIElement.hpp"

namespace GTGUI
{
    /// Class representing a tree data structure for storing elements.
    ///
    /// The requirements for this structure is fast searching by a string.
    class GUIElementTree
    {
    public:
    
        /// Constructor.
        GUIElementTree();
        
        /// Destructor.
        ~GUIElementTree();
        
        
        /// Inserts a style class into the tree.
        ///
        /// @param item [in] A reference to the item to add to the list.
        void Insert(GUIElement &item);
        void Insert(GUIElement* item) { assert(item != nullptr); this->Insert(*item); }
        
        /// Removes a style class from the tree.
        ///
        /// @param item [in] A reference to the item to remove from the list.
        void Remove(GUIElement &item);
        void Remove(GUIElement* item) { assert(item != nullptr); this->Remove(*item); }
        
        
        /// Finds the style class by id.
        ///
        /// @param id           [in] The id of the item to find.
        /// @param idLengthInTs [in] The length of the id, or -1 if the id is null terminated.
        ///
        /// @return A pointer to the item with the given id, or null if the item does not exist.
        GUIElement* FindByID(const char* id, ptrdiff_t idLengthInTs = -1) const;
    
    
        /// Retrieves a pointer to the root item.
        GUIElement* GetRoot();
        
        
        /// Retrieves the number of items in the tree.
        ///
        /// @remarks
        ///     This runs in linear time.
        size_t GetCount() const;
        
    
    private:

        /// Performs an in-order successor removal of the given node.
        ///
        /// @param item [in] A reference to the item to remove.
        ///
        /// @remarks
        ///     This method asserts that the left and right children are not null.
        void RemoveByInOrderSuccessor(GUIElement &item);

        /// Performs an in-order predecessor removal of the given node.
        ///
        /// @param item [in] A reference to the item to remove.
        ///
        /// @remarks
        ///     This method asserts that the left and right children are not null.
        void RemoveByInOrderPredecessor(GUIElement &item);
        
        /// Replaces the left or right node of the parent of the given node with a new value.
        ///
        /// @param item            [in] The item whose parent is having it's left or right child switched.
        /// @param replacementItem [in] The replacement item.
        ///
        /// @remarks
        ///     This is used by Remove(). 'item' will be the node being removed.
        void ReplaceItemInParent(GUIElement &item, GUIElement* replacementItem);

        /// Finds the in-order predecessor of the given node.
        ///
        /// @param item [in] The style class who's predecessor is being retrieved.
        GUIElement* FindInOrderPredecessor(GUIElement &item);
        
        /// Finds the in-order successor of the given node.
        ///
        /// @param item [in] The style class who's successor is being retrieved.
        GUIElement* FindInOrderSuccessor(GUIElement &item);
    
        
        /// Recursive function for counting the number of items in the tree.
        size_t GetCount(GUIElement* root) const;
    

        /// Verifies the tree for correctness. This is only used for debugging and testing.
        bool Verify() const;
        bool Verify(GUIElement* root) const;
    
    private:
        
        /// A pointer to the root item. Starts out as null.
        GUIElement* m_root;

        /// Whether or not the next removal should be an in-order successor or predecessor type. This is toggled with each removal. The
        /// idea with this system is to try and keep the tree a bit more balanced when removing items. Not sure how well it'll work, though.
        bool m_removeBySuccessor;
        
        
    private:    // No copying.
        GUIElementTree(const GUIElementTree &);
        GUIElementTree & operator=(const GUIElementTree &);
    };
}

#endif
