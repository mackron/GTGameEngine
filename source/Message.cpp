// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

#include <GTEngine/Message.hpp>

namespace GT
{
    Message::Message(MessageType type, const char* content)
        : m_type(type), m_content(content), m_associatedFiles()
    {
    }

    Message::Message(const Message &other)
        : m_type(other.m_type), m_content(other.m_content.c_str()), m_associatedFiles(other.m_associatedFiles)
    {
    }


    MessageType Message::GetType() const
    {
        return m_type;
    }

    void Message::SetType(MessageType type)
    {
        m_type = type;
    }


    const char* Message::GetContent() const
    {
        return m_content.c_str();
    }

    void Message::SetContent(const char* content)
    {
        m_content = content;
    }


    void Message::AddAssociatedFile(const char* filePath)
    {
        if (!m_associatedFiles.Exists(filePath))
        {
            m_associatedFiles.PushBack(filePath);
        }
    }

    void Message::RemoveAssociatedFile(const char* filePath)
    {
        m_associatedFiles.RemoveFirstOccuranceOf(filePath);
    }

    size_t Message::GetAssociatedFileCount() const
    {
        return m_associatedFiles.count;
    }

    GTLib::String Message::GetAssociatedFileByIndex(size_t index) const
    {
        return m_associatedFiles[index];
    }
}