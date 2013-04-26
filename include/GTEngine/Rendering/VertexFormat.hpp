// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_VertexFormat_hpp_
#define __GTEngine_VertexFormat_hpp_

#include "VertexAttributes.hpp"
#include <GTCore/Serializer.hpp>
#include <GTCore/Deserializer.hpp>
#include <cstddef>

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


        /// Retrieves information about a vertex attribute.
        ///
        /// @param index          [in ] The index of the attribute whose information is being retrieved.
        /// @param componentCount [out] The number of components for the given attribute.
        /// @param offset         [out] The offset of the data for the given attribute in floats.
        ///
        /// @return True if the attribute exists; false otherwise.
        bool GetAttributeInfo(int index, size_t &componentCount, size_t &offset) const;


        
        /**
        *   \brief  Retrieves the number of floats making up a vertex. This will just be the sum of 'count' from AddAttribute().
        */
        size_t GetSize() const;
        size_t GetSizeInBytes() const;
        
        /**
        *   \brief  Retrieves a pointer to the array making up the format.
        */
        const int* ToArray() const;
        
        bool operator== (const VertexFormat &other);
        bool operator!= (const VertexFormat &other) { return !(*this == other); }
        
        const int & operator[] (const size_t index) const { return this->attributes[index]; }
        //    int & operator[] (const size_t index)       { return this->attributes[index]; }
              
        VertexFormat & operator= (const VertexFormat &other);



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the vertex array.
        ///
        /// @param serializer [in] A reference to the serializer to write to.
        void Serialize(GTCore::Serializer &serializer) const;

        /// Deserializes the vertex array.
        ///
        /// @param deserializer [in] A reference to the deserializer to read from.
        void Deserialize(GTCore::Deserializer &deserializer);
        

    private:

        /// Generic constructor for use internally.
        void ctor();


    private:

        /// The internal array containing the attribute information. We initialise every item in this array to -1 (VertexAttribs::End).
        int attributes[GTENGINE_VERTEX_FORMAT_SIZE];

        /// The number of valid integers in 'attributes'. This does not include the trailing -1 value.
        size_t count;


    // The functions below are used to retrieve common vertex formats.
    public:

        static VertexFormat P2;         // 2D position only. Good for fullscreen quads.
        static VertexFormat P3;         // 3D position only.
        static VertexFormat P2T2;       // 2D position, 2D texture coord.
        static VertexFormat P3T2;       // 3D position, 2D texture coord.
        static VertexFormat P3T2N3;     // 3D position, 2D texture coord, 3D normal.
        static VertexFormat P3T2N3T3B3; // 3D position, 2D texture coord, 3D normal, 3D tangent, 3D bitangent.
        static VertexFormat P2C4;       // 2D position, 4D colour.
        static VertexFormat P2T2C4;     // 2D position, 2D texture coord, 4D colour.
        static VertexFormat P3T2N3C4;   // 3D position, 2D texture coord, 3D normal, 4D colour.
        static VertexFormat P4T4N4C4;   // 4D position, 4D texture coord, 4D normal, 4D colour.
    };
}




#endif
