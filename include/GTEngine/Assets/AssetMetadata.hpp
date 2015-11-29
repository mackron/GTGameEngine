// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_AseetMetadata
#define GT_AseetMetadata

#include <GTEngine/Core/Vector.hpp>
#include <GTEngine/Core/Serializer.hpp>
#include <GTEngine/Core/Deserializer.hpp>

namespace GT
{
    /// Class representing a metadata chunk.
    class AssetMetadataChunk
    {
    public:

        /// Constructor.
        AssetMetadataChunk(const char* name, uint32_t sizeInBytes, const void* pData);

        /// Constructs the chunk from a deserializer.
        AssetMetadataChunk(Deserializer &deserializer);

        /// Destructor.
        ~AssetMetadataChunk();


        /// Sets the name of the metadata.
        ///
        /// @param name [in] The new name. This cannot be longer than 31 characters, and must be null-terminated.
        ///
        /// @remarks
        ///     If \c name is larger than 31 characters it will be clipped.
        void SetName(const char* name);

        /// Retrieves the name of the metadata.
        ///
        /// @return The name of the chunk.
        const char* GetName() const;


        /// Sets the data of the metadata chunk.
        ///
        /// @param sizeInBytes [in] The size in bytes of the data.
        /// @param pData       [in] A pointer to the buffer that contains the metadata data.
        void SetData(uint32_t sizeInBytes, const void* pData);

        /// Retrieves the size of the metadata, in bytes.
        uint32_t GetDataSizeInBytes() const;

        /// Retrieves a pointer to the buffer that contains the raw data of the metadata chunk.
        const void* GetData() const;


        /// Serializes the chunk.
        ///
        /// @param serializer [in] A reference to the serializer to write the data to.
        void Serialize(Serializer &serializer) const;



    private:

        /// A pointer to the buffer containing the raw data.
        void* m_pData;

        /// The name of the chunk. Maximum of 31 characters, with the 32nd character being the null-terminator.
        char m_name[32];

        /// The size of the metadata data.
        uint32_t m_dataSizeInBytes;
    };


    /// Class representing the metadata of an asset.
    class AssetMetadata
    {
    public:

        /// Constructor.
        AssetMetadata();

        /// Constructs the metadata from a deserializer.
        ///
        /// @param deserializer [in] A reference to the deserializer.
        AssetMetadata(Deserializer &deserializer);

        /// Destructor.
        ~AssetMetadata();


        /// Adds a metadata chunk.
        ///
        /// @param name        [in] The name of the metadata chunk. Only the first 31 characters are used.
        /// @param sizeInBytes [in] The size in bytes of the metadata chunk.
        /// @param pData       [in] A pointer to the buffer containing the data of the metadata chunk.
        ///
        /// @remarks
        ///     If a chunk of the same name already exists it will be replaced.
        void AddChunk(const char* name, uint32_t sizeInBytes, const void* pData);

        /// Removes a metadata chunk.
        ///
        /// @param name [in] The name of the metadata chunk to remove.
        void RemoveChunk(const char* name);

        /// Retrieves a pointer to the buffer containing the data of the metadata chunk with the given name.
        ///
        /// @param name           [in]  The name of the buffer whose data is being retrieved.
        /// @param sizeInBytesOut [out] A reference to the variable that will receive the size of the data.
        ///
        /// @return A pointer to the chunk data, or null if a chunk of the given name does not exist.
        ///
        /// @remarks
        ///     If a chunk of the given name does not exist, \c sizeInBytesOut will be left unchanged.
        const void* GetChunkData(const char* name, uint32_t &sizeInBytesOut) const;


        /// Deletes every metadata chunk.
        void Clear();


        /// Serializes the metadata.
        ///
        /// @param serializer [in] A reference to the serializer.
        ///
        /// @remarks
        ///     The format of the serializer is very simple. The first 4 bytes defines the number of chunks, which is
        ///     followed by the chunk data itself.
        void Serialize(Serializer &serializer) const;

        /// Deserializes the metadata.
        ///
        /// @param deserializer [in] A reference to the deserializer to read the data from.
        ///
        /// @remarks
        ///     This will clear the metadata before deserializing.
        void Deserialize(Deserializer &deserializer);


    private:

        /// The list of metadata chunks.
        Vector<AssetMetadataChunk*> m_chunks;
    };
}

#endif // !GT_AseetMetadata
