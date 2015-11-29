// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_Message
#define __GT_Message

#include <GTEngine/Core/String.hpp>
#include <GTEngine/Core/Vector.hpp>

namespace GT
{
    /// Enumerator representing the different message types.
    enum MessageType
    {
        MessageType_Log,
        MessageType_Warning,
        MessageType_Error,

        MessageType_Other
    };

    /// Class representing an error, warning or log message.
    class Message
    {
    public:

        /// Constructor.
        ///
        /// @param type    [in] The type of the message (log, warning or error).
        /// @param content [in] The actual content of the message.
        Message(MessageType type, const char* content);

        /// Copy constructor.
        ///
        /// @param other [in] A reference to the other message to copy.
        Message(const Message &other);


        /// Retrieve the message type (log, warning or error).
        MessageType GetType() const;

        /// Sets the message type.
        void SetType(MessageType type);

        
        /// Retrieves the message content.
        const char* GetContent() const;

        /// Sets the message content.
        void SetContent(const char* content);


        /// Adds an associated file.
        ///
        /// @param filePath [in] The file path of the file associated with the message.
        void AddAssociatedFile(const char* filePath);

        /// Removes an associated file.
        ///
        /// @param filePath [in] The file path of the file that is no longer associated with the message.
        ///
        /// @remarks
        ///     Note that this will change the associated file count and their indexes inside the internal list. Be careful when calling
        ///     this while in the middle of iterating over the associated files.
        void RemoveAssociatedFile(const char* filePath);

        /// Retrieves a count of the associated files.
        ///
        /// @return The number of associated files.
        size_t GetAssociatedFileCount() const;

        /// Retrieves the file path of the associated file at the given index.
        ///
        /// @param index [in] The index of the associated file to retrieve.
        ///
        /// @return The file path of the associated file at the given index.
        GTLib::String GetAssociatedFileByIndex(const size_t index) const;


    private:

        /// The message type (log, warning or error).
        MessageType m_type;

        /// The content of the message.
        GTLib::String m_content;

        /// The list of associated files, as file paths. This will usually be asset file paths.
        GTLib::Vector<GTLib::String> m_associatedFiles;
    };
}

#endif
