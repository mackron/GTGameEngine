// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Serialization.hpp>

namespace GT
{
    /// Virtual class for handling non-standard chunks when deserializing a scene.
    ///
    /// This class is what enables games to have their own data associated with a scene. The best example is the
    /// engines editor, which saves metadata with the scene.
    class SceneDeserializeCallback
    {
    public:

        /// Constructor.
        SceneDeserializeCallback();

        /// Destructor.
        virtual ~SceneDeserializeCallback();


        /// Determines whether or not the header of the given chunk is handled by callback.
        ///
        /// @param header [in] The header of the chunk.
        ///
        /// @remarks
        ///     This is used in determining whether or not to have this callback handle the chunk. This is only called
        ///     for non-standard chunks.
        ///     @par
        ///     The defualt implementation always returns false.
        virtual bool IsChunkHandled(const Serialization::ChunkHeader &header) const;

        /// Handles the given chunk.
        ///
        /// @param header       [in] The header of the chunk.
        /// @param deserializer [in] A reference to the deserializer to read the chunk from.
        ///
        /// @return True if the chunk is handled properly; false otherwise.
        ///
        /// @remarks
        ///     This deserializer will be sitting at the first byte of the main chunk data, just past the header. Implementations
        ///     of this method should not deserialize the header.
        virtual bool HandleChunk(const Serialization::ChunkHeader &header, Deserializer &deserializer);
    };
}