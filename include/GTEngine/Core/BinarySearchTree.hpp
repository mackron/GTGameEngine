// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_BinarySearchTree_hpp_
#define __GTLib_BinarySearchTree_hpp_

namespace GT
{
    template <typename T>
    class BinarySearchTree
    {
    public:

        /// Class representing a node in the binary search tree.
        struct Node
        {
            /// Constructor.
            Node(const T &value)
                : value(value), left(nullptr), right(nullptr), parent(nullptr)
            {
            }

            /// Destructor.
            ~Node()
            {
            }

            /// Determines whether or not the node is on the left of the parent.
            bool OnLeft() const
            {
                return this->parent != nullptr && this->parent->left == this;
            }

            /// Determines whether or not the node is on the right of the parent.
            bool OnRight() const
            {
                return this->parent != nullptr && this->parent->right == this;
            }


            ////////////////////////////////////////////////
            // Attributes.

            /// The value of the node.
            T value;

            /// A pointer to the low node.
            Node* left;

            /// A pointer to the high node.
            Node* right;

            /// A pointer to the parent node.
            Node* parent;



            ////////////////////////////////////////////////
            // Static Functions.

            /// Inserts a value into the given tree.
            ///
            /// @param root [in] The root node of the tree.
            static void Insert(Node* &root, const T &value)
            {
                if (value < root->value)
                {
                    if (root->left != nullptr)
                    {
                        Insert(root->left, value);
                    }
                    else
                    {
                        root->left = new Node(value);
                        root->left->parent = root;
                    }
                }
                else if (value > root->value)
                {
                    if (root->right != nullptr)
                    {
                        Insert(root->right, value);
                    }
                    else
                    {
                        root->right = new Node(value);
                        root->right->parent = root;
                    }
                }
                else
                {
                    root->value.~T();
                    new (&root->value) T(value);
                }
            }


            /// Replaces the given node with another.
            ///
            /// @param nodeToReplace [in] The node being replaced.
            /// @param newNode       [in] The replacement node.
            static void ReplaceInParent(Node* nodeToReplace, Node* newNode)
            {
                assert(nodeToReplace != nullptr);

                if (nodeToReplace->parent != nullptr)
                {
                    if (nodeToReplace->parent->left == nodeToReplace)
                    {
                        nodeToReplace->parent->left = newNode;
                    }
                    else
                    {
                        nodeToReplace->parent->right = newNode;
                    }
                }

                if (newNode != nullptr)
                {
                    newNode->parent = nodeToReplace->parent;
                }
            }

            /// Finds the smallest node that is a child of <parent>.
            ///
            /// @param parent [in] The parent node.
            ///
            /// @remarks
            ///     If <parent> is the lowest, that will be returned.
            static Node* FindMinChild(Node* &parent)
            {
                assert(parent != nullptr);

                Node* result = parent;
                while (result->left != nullptr)
                {
                    result = result->left;
                }

                return result;
            }

            /// Finds the largest node that is a child of <parent>.
            ///
            /// @param parent [in] The parent node.
            ///
            /// @remarks
            ///     If <parent> is the largest, that will be returned.
            static Node* FindMaxChild(Node* &parent)
            {
                assert(parent != nullptr);

                Node* result = parent;
                while (result->right != nullptr)
                {
                    result = result->right;
                }

                return result;
            }


            /// Finds the node containing the given value, if any, rooted by the given node.
            ///
            /// @param root  [in] The root node to search through.
            /// @param value [in] The value to search for.
            ///
            /// @return A pointer to the node containing the given value, or null if no nodes are found.
            static const Node* FindByValue(const Node* root, const T &value)
            {
                if (root != nullptr)
                {
                    if (root->value == value)
                    {
                        return root;
                    }
                    else
                    {
                        if (value < root->value)
                        {
                            return FindByValue(root->left, value);
                        }
                        else //if (value > root->value)
                        {
                            return FindByValue(root->right, value);
                        }
                    }
                }

                return nullptr;
            }
            
            static Node* FindByValue(Node* root, const T &value)
            {
                return const_cast<Node*>(FindByValue(const_cast<const Node*>(root), value));
            }
            
            
        private:    // No copying.
            Node(const Node &);
            Node & operator=(const Node &);
        };



    public:

        /// Constructor.
        BinarySearchTree()
            : root(nullptr)
        {
        }

        /// Destructor.
        ~BinarySearchTree()
        {
            this->Clear();
        }


        /// Inserts a value into the tree.
        ///
        /// @param value [in] The value to insert into the tree.
        void Insert(const T &value)
        {
            if (this->root != nullptr)
            {
                Node::Insert(this->root, value);
            }
            else
            {
                this->root = new Node(value);
            }
        }

        /// Removes a value from the tree.
        ///
        /// @param value [in] The value to remove from the tree.
        void Remove(const T &value)
        {
            this->Remove(this->FindByValue(value));
        }

        /// Removes a node from the tree.
        ///
        /// @param node [in] The node to remove.
        void Remove(Node* nodeToRemove)
        {
            if (nodeToRemove != nullptr)
            {
                if (nodeToRemove->left != nullptr && nodeToRemove->right != nullptr)
                {
                    auto replacement    = Node::FindMinChild(nodeToRemove->right);
                    nodeToRemove->value = replacement->value;

                    Node::ReplaceInParent(replacement, replacement->right);

                    delete replacement;
                }
                else
                {
                    if (nodeToRemove->left != nullptr)
                    {
                        Node::ReplaceInParent(nodeToRemove, nodeToRemove->left);

                        if (nodeToRemove == this->root)
                        {
                            this->root = nodeToRemove->left;
                        }
                    }
                    else if (nodeToRemove->right != nullptr)
                    {
                        Node::ReplaceInParent(nodeToRemove, nodeToRemove->right);

                        if (nodeToRemove == this->root)
                        {
                            this->root = nodeToRemove->right;
                        }
                    }
                    else
                    {
                        Node::ReplaceInParent(nodeToRemove, nullptr);

                        if (nodeToRemove == this->root)
                        {
                            this->root = nullptr;
                        }
                    }

                    delete nodeToRemove;
                }
            }
        }

        /// Removes the root node from the tree.
        void RemoveRoot()
        {
            this->Remove(this->root);
        }


        /// Finds a node by it's value.
        ///
        /// @param value [in] The value of the node to find.
        ///
        /// @return A pointer to the node containing the given value, or null if no such node exists.
        const Node* FindByValue(const T &value) const
        {
            return Node::FindByValue(this->root, value);
        }
        Node* FindByValue(const T &value)
        {
            return Node::FindByValue(this->root, value);
        }


        /// Determines whether or not the given value exists in the tree.
        ///
        /// @param value [in] The value to search for.
        bool Exists(const T &value) const
        {
            return this->FindByValue(value) != nullptr;
        }


        /// Clears the tree.
        void Clear()
        {
            while (this->root != nullptr)
            {
                this->RemoveRoot();
            }
        }


        ////////////////////////////////////////////
        // Attributes.

        /// The root node. Starts out as null.
        Node* root;
        
        
    private:    // No copying.
        BinarySearchTree(const BinarySearchTree &);
        BinarySearchTree & operator=(const BinarySearchTree &);
    };
}

#endif
