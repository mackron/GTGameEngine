// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/StyleClassTree.hpp>

namespace GTGUI
{
    GUIStyleClassTree::GUIStyleClassTree()
        : m_root(nullptr), m_removeBySuccessor(false)
    {
    }
    
    GUIStyleClassTree::~GUIStyleClassTree()
    {
        while (m_root != nullptr)
        {
            this->Remove(*m_root);
        }
    }
    
    
    void GUIStyleClassTree::Insert(GUIStyleClass &styleClass)
    {
        // The style class should not already be in the list. If it exists, it's a higher level error.
        assert(styleClass.bst.left   == nullptr);
        assert(styleClass.bst.right  == nullptr);
        assert(styleClass.bst.parent == nullptr);
        
        
        if (m_root == nullptr)
        {
            m_root = &styleClass;
        }
        else
        {
            auto currentNode = m_root;
            while (currentNode != nullptr)
            {
                //auto compareResult = Strings::FastCompare(styleClass.name, currentNode->name);
                //if (compareResult < 0)
                if (styleClass.bst.hashedName < currentNode->bst.hashedName)
                {
                    if (currentNode->bst.left != nullptr)
                    {
                        currentNode = currentNode->bst.left;
                    }
                    else
                    {
                        currentNode->bst.left             = &styleClass;
                        currentNode->bst.left->bst.parent = currentNode;
                        
                        return;
                    }
                }
                //else if (compareResult > 0)
                else if (styleClass.bst.hashedName > currentNode->bst.hashedName)
                {
                    if (currentNode->bst.right != nullptr)
                    {
                        currentNode = currentNode->bst.right;
                    }
                    else
                    {
                        currentNode->bst.right             = &styleClass;
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

        //this->Verify();
    }
    
    
    void GUIStyleClassTree::Remove(GUIStyleClass &styleClass)
    {
        if (styleClass.bst.left != nullptr && styleClass.bst.right != nullptr)
        {
            if (m_removeBySuccessor)
            {
                this->RemoveByInOrderSuccessor(styleClass);
            }
            else
            {
                this->RemoveByInOrderPredecessor(styleClass);
            }

            m_removeBySuccessor = !m_removeBySuccessor;
        }
        else if (styleClass.bst.left != nullptr)
        {
            this->ReplaceStyleClassInParent(styleClass, styleClass.bst.left);
        }
        else if (styleClass.bst.right != nullptr)
        {
            this->ReplaceStyleClassInParent(styleClass, styleClass.bst.right);
        }
        else
        {
            this->ReplaceStyleClassInParent(styleClass, nullptr);
        }

        
        styleClass.bst.left   = nullptr;
        styleClass.bst.right  = nullptr;
        styleClass.bst.parent = nullptr;

        //this->Verify();
    }
    
    
    GUIStyleClass* GUIStyleClassTree::FindByName(const char* name, ptrdiff_t nameLengthInTs)
    {
        uint32_t hashedName = GT::Hash(name, nameLengthInTs);
        
        auto currentNode = m_root;
        while (currentNode != nullptr)
        {
            /*
            auto compareResult = Strings::FastCompare(name, nameLengthInTs, currentNode->name);
            if (compareResult < 0)
            {
                currentNode = currentNode->bst.left;
            }
            else if (compareResult > 0)
            {
                currentNode = currentNode->bst.right;
            }
            else
            {
                return currentNode;
            }
            */
            
            
            if (hashedName < currentNode->bst.hashedName)
            {
                currentNode = currentNode->bst.left;
            }
            else if (hashedName > currentNode->bst.hashedName)
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
    
    
    GUIStyleClass* GUIStyleClassTree::GetRoot()
    {
        return m_root;
    }
    
    
    size_t GUIStyleClassTree::GetCount() const
    {
        return this->GetCount(m_root);
    }
    
    
    
    /////////////////////////////////////////////////
    // Private

    void GUIStyleClassTree::RemoveByInOrderSuccessor(GUIStyleClass &styleClass)
    {
        assert(styleClass.bst.left != nullptr && styleClass.bst.right != nullptr);

        auto successor = this->FindInOrderSuccessor(*styleClass.bst.right);
        assert(successor != nullptr);
        {
            assert(successor->bst.parent != nullptr);
            assert(successor->bst.left   == nullptr);

            if (successor == styleClass.bst.right)
            {
                successor->bst.parent = styleClass.bst.parent;
                successor->bst.left   = styleClass.bst.left;

                if (successor->bst.left != nullptr)
                {
                    successor->bst.left->bst.parent = successor;
                }


                if (successor->bst.parent != nullptr)
                {
                    if (successor->bst.parent->bst.left == &styleClass)
                    {
                        successor->bst.parent->bst.left = successor;
                    }
                    else
                    {
                        assert(successor->bst.parent->bst.right == &styleClass);
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
                successor->bst.left   = styleClass.bst.left;
                successor->bst.right  = styleClass.bst.right;
                successor->bst.parent = styleClass.bst.parent;

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
                    if (successor->bst.parent->bst.left == &styleClass)
                    {
                        successor->bst.parent->bst.left = successor;
                    }
                    else
                    {
                        assert(successor->bst.parent->bst.right == &styleClass);
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

    void GUIStyleClassTree::RemoveByInOrderPredecessor(GUIStyleClass &styleClass)
    {
        assert(styleClass.bst.left != nullptr && styleClass.bst.right != nullptr);

        auto successor = this->FindInOrderPredecessor(*styleClass.bst.left);
        assert(successor != nullptr);
        {
            assert(successor->bst.parent != nullptr);
            assert(successor->bst.right  == nullptr);

            if (successor == styleClass.bst.left)
            {
                successor->bst.parent = styleClass.bst.parent;
                successor->bst.right  = styleClass.bst.right;

                if (successor->bst.right != nullptr)
                {
                    successor->bst.right->bst.parent = successor;
                }


                if (successor->bst.parent != nullptr)
                {
                    if (successor->bst.parent->bst.left == &styleClass)
                    {
                        successor->bst.parent->bst.left = successor;
                    }
                    else
                    {
                        assert(successor->bst.parent->bst.right == &styleClass);
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
                successor->bst.left   = styleClass.bst.left;
                successor->bst.right  = styleClass.bst.right;
                successor->bst.parent = styleClass.bst.parent;

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
                    if (successor->bst.parent->bst.left == &styleClass)
                    {
                        successor->bst.parent->bst.left = successor;
                    }
                    else
                    {
                        assert(successor->bst.parent->bst.right == &styleClass);
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

    
    void GUIStyleClassTree::ReplaceStyleClassInParent(GUIStyleClass &styleClass, GUIStyleClass* replacementStyleClass)
    {
        if (styleClass.bst.parent != nullptr)
        {
            if (styleClass.bst.parent->bst.left == &styleClass)
            {
                styleClass.bst.parent->bst.left = replacementStyleClass;
            }
            else
            {
                assert(styleClass.bst.parent->bst.right == &styleClass);
                styleClass.bst.parent->bst.right = replacementStyleClass;
            }
        }
        else
        {
            // If 'styleClass' has no parent, it means it's the root. And since 'styleClass' is being removed, the replacement style class becomes the new root.
            m_root = replacementStyleClass;
        }
        
        
        if (replacementStyleClass != nullptr)
        {
            replacementStyleClass->bst.parent = styleClass.bst.parent;
        }
    }

    
    GUIStyleClass* GUIStyleClassTree::FindInOrderPredecessor(GUIStyleClass &styleClass)
    {
        auto result = &styleClass;
        while (result->bst.right != nullptr)
        {
            result = result->bst.right;
        }

        return result;
    }

    GUIStyleClass* GUIStyleClassTree::FindInOrderSuccessor(GUIStyleClass &styleClass)
    {
        auto result = &styleClass;
        while (result->bst.left != nullptr)
        {
            result = result->bst.left;
        }

        return result;
    }
    
    
    size_t GUIStyleClassTree::GetCount(GUIStyleClass* root) const
    {
        if (root != nullptr)
        {
            return 1 + this->GetCount(root->bst.left) + this->GetCount(root->bst.right);
        }
        
        return 0;
    }


    bool GUIStyleClassTree::Verify() const
    {
        return this->Verify(m_root);
    }

    bool GUIStyleClassTree::Verify(GUIStyleClass* root) const
    {
        if (root != nullptr)
        {
            if (root->bst.left != nullptr && root->bst.right != nullptr && root->bst.left->bst.hashedName > root->bst.right->bst.hashedName)
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
