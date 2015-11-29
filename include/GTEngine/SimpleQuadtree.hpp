// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_SimpleQuadtree
#define GT_SimpleQuadtree

#include <GTEngine/Core/Vector.hpp>
#include <cstdio>

namespace GT
{
    /// A simple, generic quadtree.
    ///
    /// This quadtree only stores items in the leaf nodes, and only stores a single item. When an item is set on a leaf node, the old one is overwritten.
    ///
    /// This quadtree uses integers for the bounds. Leaf nodes are 1x1.
    ///
    /// The right and bottom bounds are non-inclusive, meaning a node with left = 0 and right = 1 has a width of 1, not 2. The "position" of a node is the
    /// left/top coordinates.
    ///
    /// The quadtree should have a width and height that is a power of 2.
    template <typename T>
    class SimpleQuadtree
    {
    public:

        /// Structure representing a rectangle.
        struct Rect
        {
            int left;
            int top;
            int right;
            int bottom;
        };



    public:

        /// Constructor.
        SimpleQuadtree(int leftIn, int topIn, int rightIn, int bottomIn)
            : left(leftIn), top(topIn), right(rightIn), bottom(bottomIn),
              northWest(nullptr), northEast(nullptr), southWest(nullptr), southEast(nullptr),
              data(nullptr)
        {
            assert(left < right);
            assert(top  < bottom);
        }

        /// Destructor.
        ~SimpleQuadtree()
        {
            this->Clear();
        }

        
        /// Inserts a data pointer at the given location.
        ///
        /// @param dataIn [in] A pointer to the data to set at the given position.
        /// @param x      [in] The x position to place the item.
        /// @param y      [in] The y position to place the item.
        ///
        /// @return True if the item is placed successfully.
        bool Insert(T* dataIn, int x, int y)
        {
            if (this->Contains(x, y))
            {
                // We don't care about placing the node here if it's not a leaf node. Instead we want to recursively check children. If we don't
                // find anything, it should fall through and return false.
                if (!this->IsLeafNode())
                {
                    // We will need to subdivide if we don't have children.
                    if (!this->HasChildren())
                    {
                        this->Subdivide();
                    }

                    // Here we check if we can place the data in a child.
                    if (this->northWest != nullptr && this->northWest->Insert(dataIn, x, y)) return true;
                    if (this->northEast != nullptr && this->northEast->Insert(dataIn, x, y)) return true;
                    if (this->southWest != nullptr && this->southWest->Insert(dataIn, x, y)) return true;
                    if (this->southEast != nullptr && this->southEast->Insert(dataIn, x, y)) return true;
                }
                else
                {
                    this->data = dataIn;
                    return true;
                }
            }

            return false;
        }


        /// Subdivides the quadtree node by creating the four children.
        ///
        /// @return True if the quadtree is subdivided successfully; false otherwise.
        bool Subdivide()
        {
            if (!this->HasChildren())
            {
                int halfWidth  = (this->right  - this->left) >> 1;
                int halfHeight = (this->bottom - this->top)  >> 1;

                if (halfWidth > 0 && halfHeight > 0)
                {
                    this->northWest = new SimpleQuadtree(this->left,              this->top,                 this->left + halfWidth, this->top + halfHeight);
                    this->northEast = new SimpleQuadtree(this->right - halfWidth, this->top,                 this->right,            this->top + halfHeight);
                    this->southWest = new SimpleQuadtree(this->left,              this->bottom - halfHeight, this->left + halfWidth, this->bottom);
                    this->southEast = new SimpleQuadtree(this->right - halfWidth, this->bottom - halfHeight, this->right,            this->bottom);
                }
                else if (halfWidth > 0)
                {
                    this->northWest = new SimpleQuadtree(this->left,              this->top,                 this->left + halfWidth, this->top + halfHeight);
                    this->northEast = new SimpleQuadtree(this->right - halfWidth, this->top,                 this->right,            this->top + halfHeight);
                }
                else if (halfHeight > 0)
                {
                    this->northEast = new SimpleQuadtree(this->right - halfWidth, this->top,                 this->right,            this->top + halfHeight);
                    this->southEast = new SimpleQuadtree(this->right - halfWidth, this->bottom - halfHeight, this->right,            this->bottom);
                }
                else
                {
                    assert(false);          // <-- With everything factored correctly, should never hit this.
                    return false;
                }

                return true;
            }

            return false;
        }


        /// Retrieves all of the data pointer that are containing in the given boundary.
        ///
        /// @param otherLeft   [in ] The left position of the box to query.
        /// @param otherTop    [in ] The top position of the box to query.
        /// @param otherRight  [in ] The right position of the box to query.
        /// @param otherBottom [in ] The bottom position of the box to query.
        /// @param output      [out] A reference to the list to store the items in.
        void QueryRange(int otherLeft, int otherTop, int otherRight, int otherBottom, Vector<T*> &output) const
        {
            if (this->Intersects(otherLeft, otherTop, otherRight, otherBottom))
            {
                if (this->data != nullptr)
                {
                    output.PushBack(this->data);
                }

                // If we have children, we're going to check them, too.
                if (this->northWest != nullptr) this->northWest->QueryRange(otherLeft, otherTop, otherRight, otherBottom, output);
                if (this->northEast != nullptr) this->northEast->QueryRange(otherLeft, otherTop, otherRight, otherBottom, output);
                if (this->southWest != nullptr) this->southWest->QueryRange(otherLeft, otherTop, otherRight, otherBottom, output);
                if (this->southEast != nullptr) this->southEast->QueryRange(otherLeft, otherTop, otherRight, otherBottom, output);
            }
        }

        /// Retrieves every data pointer that is contained within the bounds of this node.
        ///
        /// @param output [out] A reference to the list to store the results.
        void QueryAll(Vector<T*> &output) const
        {
            this->QueryRange(this->left, this->top, this->right, this->bottom, output);
        }

        /// Retrieves the data point of the item at the given coordinates.
        ///
        /// @param x [in] The x coordinate.
        /// @param y [in] The y coordinate.
        ///
        /// @return A pointer to the data at the given position, or null if there is no data.
        T* QuerySingle(int x, int y) const
        {
            if (this->Contains(x, y))
            {
                if (this->data != nullptr)
                {
                    return this->data;
                }


                T* temp = nullptr;

                if (this->northWest != nullptr)
                {
                    temp = this->northWest->QuerySingle(x, y);
                    if (temp != nullptr)
                    {
                        return temp;
                    }
                }

                if (this->northEast != nullptr)
                {
                    temp = this->northEast->QuerySingle(x, y);
                    if (temp != nullptr)
                    {
                        return temp;
                    }
                }

                if (this->southWest != nullptr)
                {
                    temp = this->southWest->QuerySingle(x, y);
                    if (temp != nullptr)
                    {
                        return temp;
                    }
                }

                if (this->southEast != nullptr)
                {
                    temp = this->southEast->QuerySingle(x, y);
                    if (temp != nullptr)
                    {
                        return temp;
                    }
                }
            }

            return nullptr;
        }


        /// Determines whether or not the given bounds intersects with this node.
        bool Intersects(int otherLeft, int otherTop, int otherRight, int otherBottom) const
        {
            if (this->left   >= otherRight)  return false;
            if (this->top    >= otherBottom) return false;
            if (this->right  <= otherLeft)   return false;
            if (this->bottom <= otherTop)    return false;

            return true;
        }


        /// Determines whether or not the given rectangle is empty space (contains no data).
        ///
        /// @return True if there are no data pointers inside the given bounds; false otherwise.
        bool IsEmptySpace(int otherLeft, int otherTop, int otherRight, int otherBottom) const
        {
            if (this->Intersects(otherLeft, otherTop, otherRight, otherBottom))
            {
                if (this->data != nullptr)
                {
                    return false;
                }

                if (this->northWest != nullptr && !this->northWest->IsEmptySpace(otherLeft, otherTop, otherRight, otherBottom)) return false;
                if (this->northEast != nullptr && !this->northEast->IsEmptySpace(otherLeft, otherTop, otherRight, otherBottom)) return false;
                if (this->southWest != nullptr && !this->southWest->IsEmptySpace(otherLeft, otherTop, otherRight, otherBottom)) return false;
                if (this->southWest != nullptr && !this->southWest->IsEmptySpace(otherLeft, otherTop, otherRight, otherBottom)) return false;
            }

            return true;
        }

        /// Determines whether or not the leaf node at the given position is empty.
        ///
        /// @param x [in] The leaf node's x position.
        /// @param y [in] The leaf node's y position.
        ///
        /// @return True if the leaf node at the given position is empty; false otherwise.
        bool IsEmptySpace(int x, int y) const
        {
            return this->QuerySingle(x, y) == nullptr;
        }


        /// Determines whether or not the bounding box contains the given point.
        ///
        /// @param x [in] The x position of the point.
        /// @param y [in] The y position of the point.
        ///
        /// @return True if the point is contained on or inside the bounds of this node.
        bool Contains(int x, int y) const
        {
            return this->left <= x && this->right  > x &&
                   this->top  <= y && this->bottom > y;
        }

        /// Determines whether or not this node has children.
        ///
        /// @return Trus if the node has children; false otherwise.
        bool HasChildren() const
        {
            return this->northWest != nullptr || this->northEast != nullptr || this->southWest != nullptr || this->southEast != nullptr;
        }

        /// Determines whether or not this node is a leaf node.
        ///
        /// @return True if the node is a leaf node; false otherwise.
        ///
        /// @remarks
        ///     This method uses the size of the node in determining whether or not it is a leaf.
        bool IsLeafNode() const
        {
            return (this->right  == this->left + 1 || this->right  == this->left) &&
                   (this->bottom == this->top  + 1 || this->bottom == this->top);
        }


        /// Clears the quadtree.
        void Clear()
        {
            delete this->northWest;
            delete this->northEast;
            delete this->southWest;
            delete this->southEast;

            this->northWest = nullptr;
            this->northEast = nullptr;
            this->southWest = nullptr;
            this->southEast = nullptr;
        }


        /// Scans for empty spacing across the X axis, left to right.
        void ScanEmptySpacesX(int left, int top, int right, int bottom, int minWidth, Vector<Rect> &output) const
        {
            assert(left < right);
            assert(top  < bottom);

            Rect currentRect;
            currentRect.left   = 0;
            currentRect.right  = 0;
            currentRect.top    = top;
            currentRect.bottom = bottom;



            // We're doing to do a linear scan for now.
            for (int x = left; x < right; )
            {
                // At this point we need to find the first empty space.
                while (!this->IsEmptySpace(x, top, x + 1, bottom) && x < right)
                {
                    ++x;
                }

                // When we get here we will be at an empty space. We can now keep looping until we find a non-empty space, or the end of the rectangle.
                if (x < right)
                {
                    currentRect.left = x;
                    
                    while (this->IsEmptySpace(x, top, x + 1, bottom) && x < right)
                    {
                        ++x;
                        currentRect.right = x;
                    }

                    // At this point we will have an empty region which can now be added to the output list, but only if it's at least the size of <minWidth>.
                    if (currentRect.right - currentRect.left >= minWidth)
                    {
                        output.PushBack(currentRect);
                    }
                }
            }
        }

        /// Scans for empty spacing down the Y axis, top to bottom.
        void ScanEmptySpacesY(int left, int top, int right, int bottom, int minHeight, Vector<Rect> &output) const
        {
            assert(left < right);
            assert(top  < bottom);

            Rect currentRect;
            currentRect.left   = left;
            currentRect.right  = right;
            currentRect.top    = 0;
            currentRect.bottom = 0;


            // We're doing to do a linear scan for now.
            for (int y = top; y < bottom; )
            {
                // At this point we need to find the first empty space.
                while (!this->IsEmptySpace(left, y, right, y + 1) && y < bottom)
                {
                    ++y;
                }

                // When we get here we will be at an empty space. We can now keep looping until we find a non-empty space, or the end of the rectangle.
                if (y < bottom)
                {
                    currentRect.top = y;
                    
                    while (this->IsEmptySpace(left, y, right, y + 1) && y < bottom)
                    {
                        ++y;
                        currentRect.bottom = y;
                    }

                    // At this point we will have an empty region which can now be added to the output list, but only if it's at least the size of <minWidth>.
                    if (currentRect.bottom - currentRect.top >= minHeight)
                    {
                        output.PushBack(currentRect);
                    }
                }
            }
        }


    private:

        /// The four corners of the node.
        int left;
        int top;
        int right;
        int bottom;

        /// Pointers to the children.
        SimpleQuadtree* northWest;
        SimpleQuadtree* northEast;
        SimpleQuadtree* southWest;
        SimpleQuadtree* southEast;

        /// The data associated with this node. Should only be valid for leaf nodes.
        T* data;


    private:    // No copying.
        SimpleQuadtree(const SimpleQuadtree &);
        SimpleQuadtree & operator=(const SimpleQuadtree &);
    };
}

#endif