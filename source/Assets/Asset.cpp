// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Assets/Asset.hpp>

namespace GT
{
    namespace Engine
    {
        Asset::Asset(const GTLib::String &relativePath, const GTLib::String &absolutePath)
            : m_relativePath(relativePath),
              m_absolutePath(absolutePath),
              m_referenceCounter(1)
        {
        }

        Asset::~Asset()
        {
        }



        const char* Asset::GetRelativePath() const
        {
            return m_relativePath.c_str();
        }

        const char* Asset::GetAbsolutePath() const
        {
            return m_absolutePath.c_str();
        }



        unsigned int Asset::GetReferenceCounter() const
        {
            return m_referenceCounter;
        }

        unsigned int Asset::IncrementReferenceCounter()
        {
            m_referenceCounter += 1;
            
            return m_referenceCounter;
        }

        unsigned int Asset::DecrementReferenceCounter()
        {
            if (m_referenceCounter > 0)
            {
                m_referenceCounter -= 1;
            }

            return m_referenceCounter;
        }



        ///////////////////////////////////////////////////////
        // Virtual Methods

        GTEngine::AssetType Asset::GetType() const
        {
            return GTEngine::AssetType_None;
        }
    }
}