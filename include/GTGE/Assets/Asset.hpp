// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_Asset
#define GT_Asset

namespace GT
{
    /// Class representing an asset (texture, model, sound, etc.)
    class Asset : public ReferenceCountedObject
    {
    public:

        /// Constructor.
        Asset(const char* absolutePathOrIdentifier, AssetType type);

        /// Destructor.
        virtual ~Asset();


        /// Retrieves the absolute path or identifier of the asset.
        const char* GetAbsolutePathOrIdentifier() const;

        /// Retrieves the type of this asset.
        AssetType GetType() const;


        /// Adds a metadata chunk to the asset's metadata.
        ///
        /// @param name        [in] The name of the metadata chunk. Only the first 31 characters are used.
        /// @param sizeInBytes [in] The size in bytes of the metadata chunk.
        /// @param pData       [in] A pointer to the buffer that contains the chunk data.
        ///
        /// @remarks
        ///     If a chunk with the same name already exists it will be replaced.
        void AddMetadataChunk(const char* name, uint32_t sizeInBytes, const void* pData);

        /// Retrieves a pointer to the buffer containing the data of the chunk with the given name.
        ///
        /// @param name [in] The name of the metadata chunk whose data pointer is being retrieved.
        /// @param sizeInBytesOut [out] A reference to the variable that will receive the size in bytes of the chunk data.
        ///
        /// @return A pointer to the buffer containing the chunk's data.
        const void* GetMetadataChunkData(const char* name, uint32_t &sizeInBytesOut) const;

        /// @copydoc GetMetadataChunkData()
        const void* GetMetadataChunkData(const char* name) const;

        /// Loads the metadata from the file at the given path.
        ///
        /// @param absolutePath [in] The absolute path of the metadata.
        /// @param pVFS         [in] A pointer to the file system object to load the file from.
        ///
        /// @remarks
        ///     This is virtual so that inherited classes do custom implementations which may be required for things like procedural assets. It is
        ///     not required that inherited classes implement this, in which case the default implementation will simply try loading it from a file.
        virtual bool LoadMetadata(const char* absolutePath, drfs_context* pVFS);


        /////////////////////////////////////
        // Virtual Methods
        //
        // Inherited classes must implement these.

        /// Retrieves the class of this asset.
        virtual AssetClass GetClass() const = 0;

        /// Loads the given file into the asset.
        ///
        /// @param absolutePath [in] The absolute path of the file to load.
        /// @param pVFS         [in] A pointer to the file system object to load the file from.
        ///
        /// @remarks
        ///     This will replace the existing content of the asset.
        virtual bool Load(const char* absolutePath, drfs_context* pVFS) = 0;



    private:

        /// The absolute path or identifier of the asset.
        char m_absolutePathOrIdentifier[DRFS_MAX_PATH];

        /// The asset type.
        AssetType m_type;

        /// The asset's metadata.
        AssetMetadata m_metadata;
    };
}

#endif
