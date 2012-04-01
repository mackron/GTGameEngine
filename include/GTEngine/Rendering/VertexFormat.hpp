
#ifndef __GTEngine_VertexFormat_hpp_
#define __GTEngine_VertexFormat_hpp_

#include <cstddef>
#include "VertexAttributes.hpp"

#define GTENGINE_MAX_VERTEX_ATTRIBS       16
#define GTENGINE_VERTEX_FORMAT_SIZE       (GTENGINE_MAX_VERTEX_ATTRIBS * 2 + 1)

namespace GTEngine
{
    /**
    *   \brief  Class representing the format of vertex attributes.
    */
    class VertexFormat
    {
    public:

        /**
        *   \brief  Constructor.
        */
        VertexFormat();

        /**
        *   \brief  Copy constructor.
        */
        VertexFormat(const VertexFormat &format);
        
        /**
        *   \brief  Constructor for constructing from an array directly.
        */
        VertexFormat(int *other);

        /**
        *   \brief             Adds an attribute to the format.
        *   \param  index [in] The index of the vertex element. This should be same index as used by the vertex shader.
        *   \param  count [in] The number of float components making up the attribute. For example, this will be 3 for a position (x, y and z). Must be at least 1.
        *
        *   \remarks
        *       This order this function is called is important. It defines the structure of the vertex data.
        *       \par
        *       'count' must be positive and can not be zero. 'index' can be zero, but can not be negative.
        *       \par
        *       'index' can be one of the VertexAttributeIndex_* constants.
        */
        void AddAttribute(int index, int count);
        
        /**
        *   \brief             Retrieves the number of components for the given attribute index.
        *   \param  index [in] The index of the vertex attribute. This will be the same as the index specified in AddAttribute().
        *
        *   \remarks
        *       If an attribute at the given index does exist in the format, -1 is returned.
        */
        int GetAttributeComponentCount(int index) const;
        
        /**
        *   \brief             Retrieves the offset of the data for the given attribute. This is in floats, not bytes.
        *   \param  index [in] The index of the vertex attribute. This will be the same as the index specified in AddAttribute().
        *
        *   \remarks
        *       If an attribute at the given index does exist in the format, -1 is returned.
        */
        int GetAttributeOffset(int index) const;

        /**
        *   \brief  Retrieves the number of attributes in the format.
        */
        inline size_t GetAttributeCount() const { return this->count; }
        
        /**
        *   \brief  Retrieves the number of floats making up a vertex. This will just be the sum of 'count' from AddAttribute().
        */
        size_t GetSize() const;
        size_t GetSizeInBytes() const;
        
        
        /**
        *   \brief  Retrieves a pointer to the array making up the format.
        */
        const int * ToArray() const;
        
        bool operator== (const VertexFormat &other);
        bool operator!= (const VertexFormat &other) { return !(*this == other); }
        
        const int & operator[] (const size_t index) const { return this->attributes[index]; }
        //    int & operator[] (const size_t index)       { return this->attributes[index]; }
              
        VertexFormat & operator= (const VertexFormat &other);
        

    private:

        /// Generic constructor for use internally.
        void ctor();


    private:

        /// The internal array containing the attribute information. We initialise every item in this array to -1 (VertexAttribs::End).
        int attributes[GTENGINE_VERTEX_FORMAT_SIZE];

        /// The number of number of valid integers in 'attributes'. This does not include the trailing -1 value.
        size_t count;


    // The functions below are used to retrieve common vertex formats.
    public:

        static VertexFormat P2;         // 2D position only. Good for fullscreen quads.
        static VertexFormat P3;         // 3D position only.
        static VertexFormat P2T2;       // 2D position, 2D texture coord.
        static VertexFormat P3T2;       // 3D position, 2D texture coord.
        static VertexFormat P3T2N3;     // 3D position, 2D texture coord, 3D normal.
        static VertexFormat P3T2N3T3B3; // 3D position, 2D texture coord, 3D normal, 3D tangent, 3D binormal.
    };
}




#endif
