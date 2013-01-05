
#include <GTEngine/Rendering/VertexFormat.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behavior
#endif

namespace GTEngine
{
    VertexFormat::VertexFormat()
        : attributes(), count(0)
    {
        this->ctor();
    }

    VertexFormat::VertexFormat(const VertexFormat &other)
        : attributes(), count(0)
    {
        *this = other;
    }

    VertexFormat::VertexFormat(int *other)
        : attributes(), count(0)
    {
        this->ctor();

        if (other != nullptr)
        {
            for (int i = 0; other[i] != VertexAttribs::End; i += 2)
            {
                this->AddAttribute(other[i], other[i + 1]);
            }
        }
    }

    void VertexFormat::AddAttribute(int index, int numComponents)
    {
        this->attributes[this->count]     = index;
        this->attributes[this->count + 1] = numComponents;

        this->count += 2;
    }

    int VertexFormat::GetAttributeComponentCount(int index) const
    {
        for (size_t i = 0; i < this->count; i += 2)
        {
            if (this->attributes[i] == index)
            {
                return this->attributes[i + 1];
            }
        }

        return -1;
    }

    int VertexFormat::GetAttributeOffset(int index) const
    {
        int stride = 0;
        
        for (size_t i = 0; i < this->count; i += 2)
        {
            if (this->attributes[i] == index)
            {
                return stride;
            }
            
            stride += this->attributes[i + 1];
        }
        
        return -1;
    }

    bool VertexFormat::GetAttributeInfo(int index, size_t &componentCount, size_t &offset) const
    {
        offset = 0;
        
        for (size_t i = 0; i < this->count; i += 2)
        {
            if (this->attributes[i] == index)
            {
                componentCount = static_cast<size_t>(this->attributes[i + 1]);
                return true;
            }
            
            offset += static_cast<size_t>(this->attributes[i + 1]);
        }


        componentCount = 0;
        offset         = 0;

        return false;
    }

    size_t VertexFormat::GetSize() const
    {
        // TODO: Optimize this so that the size is stored as a variable and changed dynamically with AddAttribute().

        int size = 0;
        
        for (size_t i = 0; i < this->count; i += 2)
        {
            size += this->attributes[i + 1];
        }
        
        return static_cast<size_t>(size);
    }

    size_t VertexFormat::GetSizeInBytes() const
    {
        return this->GetSize() * sizeof(float);
    }


    const int * VertexFormat::ToArray() const
    {
        return &this->attributes[0];
    }

    bool VertexFormat::operator== (const VertexFormat &other)
    {
        if (this->count != other.count)
        {
            return false;
        }

        // If any elements are different, the format is different.
        for (size_t i = 0; i < this->count; ++i)
        {
            if (this->attributes[i] != other[i])
            {
                return false;
            }
        }
        
        return true;
    }

    VertexFormat & VertexFormat::operator= (const VertexFormat &other)
    {
        for (auto i = 0; i < GTENGINE_VERTEX_FORMAT_SIZE; ++i)
        {
            this->attributes[i] = other.attributes[i];
        }

        this->count = other.count;
        
        return *this;
    }



    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void VertexFormat::Serialize(GTCore::Serializer &serializer) const
    {
        serializer.Write(this->attributes, sizeof(int) * GTENGINE_VERTEX_FORMAT_SIZE);
        serializer.Write(static_cast<uint32_t>(this->count));
    }

    void VertexFormat::Deserialize(GTCore::Deserializer &deserializer)
    {
        deserializer.Read(this->attributes, sizeof(int) * GTENGINE_VERTEX_FORMAT_SIZE);
        deserializer.Read(static_cast<uint32_t &>(this->count));
    }




    void VertexFormat::ctor()
    {
        for (auto i = 0; i < GTENGINE_VERTEX_FORMAT_SIZE; ++i)
        {
            this->attributes[i] = VertexAttribs::End;
        }
    }
}


namespace GTEngine
{
    bool GlobalVertexFormatsInitialised = false;

    int TempP2[]         = {VertexAttribs::Position, 2, VertexAttribs::End};
    int TempP3[]         = {VertexAttribs::Position, 3, VertexAttribs::End};
    int TempP2T2[]       = {VertexAttribs::Position, 2, VertexAttribs::TexCoord, 2, VertexAttribs::End};
    int TempP3T2[]       = {VertexAttribs::Position, 3, VertexAttribs::TexCoord, 2, VertexAttribs::End};
    int TempP3T2N3[]     = {VertexAttribs::Position, 3, VertexAttribs::TexCoord, 2, VertexAttribs::Normal, 3, VertexAttribs::End};
    int TempP3T2N3T3B3[] = {VertexAttribs::Position, 3, VertexAttribs::TexCoord, 2, VertexAttribs::Normal, 3, VertexAttribs::Tangent, 3, VertexAttribs::Bitangent, 3, VertexAttribs::End};
    int TempP2C4[]       = {VertexAttribs::Position, 2, VertexAttribs::Colour,   4, VertexAttribs::End};
    int TempP2T2C4[]     = {VertexAttribs::Position, 2, VertexAttribs::TexCoord, 2, VertexAttribs::Colour, 4, VertexAttribs::End};

    VertexFormat VertexFormat::P2(TempP2);
    VertexFormat VertexFormat::P3(TempP3);
    VertexFormat VertexFormat::P2T2(TempP2T2);
    VertexFormat VertexFormat::P3T2(TempP3T2);
    VertexFormat VertexFormat::P3T2N3(TempP3T2N3);
    VertexFormat VertexFormat::P3T2N3T3B3(TempP3T2N3T3B3);
    VertexFormat VertexFormat::P2C4(TempP2C4);
    VertexFormat VertexFormat::P2T2C4(TempP2T2C4);
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif


