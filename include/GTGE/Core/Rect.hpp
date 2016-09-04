// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Rect
#define GT_Rect

namespace GT
{
    /// A generic rectangle structure.
    template <typename T>
    struct Rect
    {
        Rect()
            : left(T(0)), top(T(0)), right(T(0)), bottom(T(0))
        {
        }

        template <typename U>
        Rect(const Rect<U> &other)
            : left(T(other.left)), top(T(other.top)), right(T(other.right)), bottom(T(other.bottom))
        {
        }

        Rect(T _left, T _top, T _right, T _bottom)
            : left(_left), top(_top), right(_right), bottom(_bottom)
        {
        }


        /// Clamps the rectangle so that it does not go outside the bounds of the input rectangle.
        void Clamp(const Rect &rect)
        {
            this->left   = GT::Clamp(this->left,   rect.left, rect.right);
            this->bottom = GT::Clamp(this->bottom, rect.top,  rect.bottom);
            this->right  = GT::Clamp(this->right,  rect.left, rect.right);
            this->top    = GT::Clamp(this->top,    rect.top,  rect.bottom);
        }
        
        
        /// Determines if a given point is inside the rectangle, including on the border.
        bool Contains(int x, int y) const
        {
            return x >= this->left && x < this->right &&
                   y >= this->top  && y < this->bottom;
        }
        

        /// Determines whether or not the given rectangle is inside this rectangle.
        bool Contains(const Rect &other) const
        {
            if (other.right  < this->left)   return false;
            if (other.left   > this->right)  return false;
            if (other.bottom < this->top)    return false;
            if (other.top    > this->bottom) return false;

            return true;
        }


        /// Retrieves the width of the rectangle.
        unsigned int GetWidth() const
        {
            return Max(this->left, this->right) - Min(this->left, this->right);
        }

        /// Retrieves the height of the rectangle.
        unsigned int GetHeight() const
        {
            return Max(this->bottom, this->top) - Min(this->bottom, this->top);
        }


        /// Static function for clamping the first rectangle against the second.
        ///
        /// @param rect1 [in] The rectangle to clamp.
        /// @param rect2 [in] The rectangle to clamp against.
        static Rect<T> Clamp(const Rect<T> &rect1, const Rect<T> &rect2)
        {
            return Rect<T>(
                Clamp(rect1.left,   rect2.left, rect2.right),
                Clamp(rect1.top,    rect2.top,  rect2.bottom),
                Clamp(rect1.right,  rect2.left, rect2.right),
                Clamp(rect1.bottom, rect2.top,  rect2.bottom)
            );
        }

        
        T left;
        T top;
        T right;
        T bottom;
    };
}


#endif
