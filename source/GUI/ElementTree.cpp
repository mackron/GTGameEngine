// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/ElementTree.hpp>

namespace GTGUI
{
    ElementTree::ElementTree()
        : m_root(nullptr), m_removeBySuccessor(false)
    {
    }
    
    ElementTree::~ElementTree()
    {
        while (m_root != nullptr)
        {
            this->Remove(*m_root);
        }
    }
    
    
    void ElementTree::Insert(Element &item)
    {
        // The style class should not already be in the list. If it exists, it's a higher level error.
        assert(item.bst.left   == nullptr);
        assert(item.bst.right  == nullptr);
        assert(item.bst.parent == nullptr);
        
        
        if (m_root == nullptr)
        {
            m_root = &item;
        }
        else
        {
            auto currentNode = m_root;
            while (currentNode != nullptr)
            {
                if (item.bst.hashedID < currentNode->bst.hashedID)
                {
                    if (currentNode->bst.left != nullptr)
                    {
                        currentNode = currentNode->bst.left;
                    }
                    else
                    {
                        currentNode->bst.left             = &item;
                        currentNode->bst.left->bst.parent = currentNode;
                        
                        return;
                    }
                }
                else if (item.bst.hashedID > currentNode->bst.hashedID)
                {
                    if (currentNode->bst.right != nullptr)
                    {
                        currentNode = currentNode->bst.right;
                    }
                    else
                    {
                        currentNode->bst.right             = &item;
                        currentNode->bst.right->bst.parent = currentNode;
                        
                        return;
                    }
                }
                else
                {
                    // Already exists. We should never actually get here...
                    assert(false);
                }
            }
        }
    }
    
    
    void ElementTree::Remove(Element &item)
    {
        if (item.bst.left != nullptr && item.bst.right != nullptr)
        {
            if (m_removeBySuccessor)
            {
                this->RemoveByInOrderSuccessor(item);
            }
            else
            {
                this->RemoveByInOrderPredecessor(item);
            }

            m_removeBySuccessor = !m_removeBySuccessor;
        }
        else if (item.bst.left != nullptr)
        {
            this->ReplaceItemInParent(item, item.bst.left);
        }
        else if (item.bst.right != nullptr)
        {
            this->ReplaceItemInParent(item, item.bst.right);
        }
        else
        {
            this->ReplaceItemInParent(item, nullptr);
        }

        
        item.bst.left   = nullptr;
        item.bst.right  = nullptr;
        item.bst.parent = nullptr;
    }
    
    
    Element* ElementTree::FindByID(const char* id, ptrdiff_t idLengthInTs) const
    {
        uint32_t hashedID = GT::Hash(id, idLengthInTs);
        
        auto currentNode = m_root;
        while (currentNode != nullptr)
        {
            if (hashedID < currentNode->bst.hashedID)
            {
                currentNode = currentNode->bst.left;
            }
            else if (hashedID > currentNode->bst.hashedID)
            {
                currentNode = currentNode->bst.right;
            }
            else
            {
                return currentNode;
            }
        }
        
        
        // Not found.
        return nullptr;
    }
    
    
    Element* ElementTree::GetRoot()
    {
        return m_root;
    }
    
    
    size_t ElementTree::GetCount() const
    {
        return this->GetCount(m_root);
    }
    
    
    
    /////////////////////////////////////////////////
    // Private

    void ElementTree::RemoveByInOrderSuccessor(Element &item)
    {
        assert(item.bst.left != nullptr && item.bst.right != nullptr);

        auto successor = this->FindInOrderSuccessor(*item.bst.right);
        assert(successor != nullptr);
        {
            assert(successor->bst.parent != nullptr);
            assert(successor->bst.left   == nullptr);

            if (successor == item.bst.right)
            {
                successor->bst.parent = item.bst.parent;
                successor->bst.left   = item.bst.left;

                if (successor->bst.left != nullptr)
                {
                    successor->bst.left->bst.parent = successor;
                }


                if (successor->bst.parent != nullptr)
                {
                    if (successor->bst.parent->bst.left == &item)
                    {
                        successor->bst.parent->bst.left = successor;
                    }
                    else
                    {
                        assert(successor->bst.parent->bst.right == &item);
                        successor->bst.parent->bst.right = successor;
                    }
                }
                else
                {
                    m_root = successor;
                }
            }
            else
            {
                auto prevSuccessorRight  = successor->bst.right;
                auto prevSuccessorParent = successor->bst.parent;
                
                // 'successor' needs to replace the old one.
                successor->bst.left   = item.bst.left;
                successor->bst.right  = item.bst.right;
                successor->bst.parent = item.bst.parent;

                if (successor->bst.left != nullptr)
                {
                    successor->bst.left->bst.parent  = successor;
                }
                if (successor->bst.right != nullptr)
                {
                    successor->bst.right->bst.parent = successor;
                }
                
                if (successor->bst.parent != nullptr)
                {
                    if (successor->bst.parent->bst.left == &item)
                    {
                        successor->bst.parent->bst.left = successor;
                    }
                    else
                    {
                        assert(successor->bst.parent->bst.right == &item);
                        successor->bst.parent->bst.right = successor;
                    }
                }
                else
                {
                    m_root = successor;
                }
                


                if (prevSuccessorParent->bst.left == successor)
                {
                    prevSuccessorParent->bst.left = prevSuccessorRight;
                }
                else
                {
                    assert(prevSuccessorParent->bst.right == successor);
                    prevSuccessorParent->bst.right = prevSuccessorRight;
                }

                if (prevSuccessorRight != nullptr)
                {
                    prevSuccessorRight->bst.parent = prevSuccessorParent;
                }
            }
        }
    }

    void ElementTree::RemoveByInOrderPredecessor(Element &item)
    {
        assert(item.bst.left != nullptr && item.bst.right != nullptr);

        auto successor = this->FindInOrderPredecessor(*item.bst.left);
        assert(successor != nullptr);
        {
            assert(successor->bst.parent != nullptr);
            assert(successor->bst.right  == nullptr);

            if (successor == item.bst.left)
            {
                successor->bst.parent = item.bst.parent;
                successor->bst.right  = item.bst.right;

                if (successor->bst.right != nullptr)
                {
                    successor->bst.right->bst.parent = successor;
                }


                if (successor->bst.parent != nullptr)
                {
                    if (successor->bst.parent->bst.left == &item)
                    {
                        successor->bst.parent->bst.left = successor;
                    }
                    else
                    {
                        assert(successor->bst.parent->bst.right == &item);
                        successor->bst.parent->bst.right = successor;
                    }
                }
                else
                {
                    m_root = successor;
                }
            }
            else
            {
                auto prevSuccessorLeft   = successor->bst.left;
                auto prevSuccessorParent = successor->bst.parent;
                
                // 'successor' needs to replace the old one.
                successor->bst.left   = item.bst.left;
                successor->bst.right  = item.bst.right;
                successor->bst.parent = item.bst.parent;

                if (successor->bst.left != nullptr)
                {
                    successor->bst.left->bst.parent  = successor;
                }
                if (successor->bst.right != nullptr)
                {
                    successor->bst.right->bst.parent = successor;
                }
                
                if (successor->bst.parent != nullptr)
                {
                    if (successor->bst.parent->bst.left == &item)
                    {
                        successor->bst.parent->bst.left = successor;
                    }
                    else
                    {
                        assert(successor->bst.parent->bst.right == &item);
                        successor->bst.parent->bst.right = successor;
                    }
                }
                else
                {
                    m_root = successor;
                }
                


                if (prevSuccessorParent->bst.left == successor)
                {
                    prevSuccessorParent->bst.left = prevSuccessorLeft;
                }
                else
                {
                    assert(prevSuccessorParent->bst.right == successor);
                    prevSuccessorParent->bst.right = prevSuccessorLeft;
                }

                if (prevSuccessorLeft != nullptr)
                {
                    prevSuccessorLeft->bst.parent = prevSuccessorParent;
                }
            }
        }
    }

    
    void ElementTree::ReplaceItemInParent(Element &item, Element* replacementElement)
    {
        if (item.bst.parent != nullptr)
        {
            if (item.bst.parent->bst.left == &item)
            {
                item.bst.parent->bst.left = replacementElement;
            }
            else
            {
                assert(item.bst.parent->bst.right == &item);
                item.bst.parent->bst.right = replacementElement;
            }
        }
        else
        {
            // If 'item' has no parent, it means it's the root. And since 'item' is being removed, the replacement style class becomes the new root.
            m_root = replacementElement;
        }
        
        
        if (replacementElement != nullptr)
        {
            replacementElement->bst.parent = item.bst.parent;
        }
    }

    
    Element* ElementTree::FindInOrderPredecessor(Element &item)
    {
        auto result = &item;
        while (result->bst.right != nullptr)
        {
            result = result->bst.right;
        }

        return result;
    }

    Element* ElementTree::FindInOrderSuccessor(Element &item)
    {
        auto result = &item;
        while (result->bst.left != nullptr)
        {
            result = result->bst.left;
        }

        return result;
    }
    
    
    size_t ElementTree::GetCount(Element* root) const
    {
        if (root != nullptr)
        {
            return 1 + this->GetCount(root->bst.left) + this->GetCount(root->bst.right);
        }
        
        return 0;
    }


    bool ElementTree::Verify() const
    {
        return this->Verify(m_root);
    }

    bool ElementTree::Verify(Element* root) const
    {
        if (root != nullptr)
        {
            if (root->bst.left != nullptr && root->bst.right != nullptr && root->bst.left->bst.hashedID > root->bst.right->bst.hashedID)
            {
                // The left side is larger than the right side.
                return false;
            }

            if (root->bst.parent == root)
            {
                // The root's parent is itself.
                return false;
            }

            if (root->bst.left == root || root->bst.right == root)
            {
                // The root has a child equal to itself.
                return false;
            }

            if (root->bst.left == root->bst.right && root->bst.left != nullptr)
            {
                // The children are the same.
                return false;
            }


            if (root->bst.parent != nullptr)
            {
                if (root->bst.parent->bst.left != root && root->bst.parent->bst.right != root)
                {
                    // The roots parent does not have a child equal to the root.
                    return false;
                }

                if (root->bst.parent->bst.left == root && root->bst.parent->bst.right == root)
                {
                    // Both children of the parent are equal to the root.
                    return false;
                }

                if (root->bst.parent == root->bst.left || root->bst.parent == root->bst.right)
                {
                    // The parent is equal to a child.
                    return false;
                }
            }

            return this->Verify(root->bst.left) && this->Verify(root->bst.right);
        }

        // Being null is allowed.
        return true;
    }
}
