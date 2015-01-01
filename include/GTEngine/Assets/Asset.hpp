// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GameEngine_Asset_hpp_
#define __GT_GameEngine_Asset_hpp_

#include <GTLib/String.hpp>
#include "../AssetTypes.hpp"

namespace GT
{
    namespace Engine
    {
        /// Base class for assets.
        class Asset
        {
        public:

            /// Constructor.
            Asset(const GTLib::String &relativePath, const GTLib::String &absolutePath);

            /// Destructor.
            virtual ~Asset();


            /// Retrieves the relative path of the asset file.
            const char* GetRelativePath() const;

            /// Retrieves the absolute path of the asset file.
            const char* GetAbsolutePath() const;

            
            /// Retrieves the reference counter for the asset.
            unsigned int GetReferenceCounter() const;

            /// Increments the reference counter by 1 and returns the new value.
            unsigned int IncrementReferenceCounter();

            /// Decrements the reference counter by 1 and returns the new value. This will never go below zero.
            unsigned int DecrementReferenceCounter();



            ///////////////////////////////////////////////////////
            // Virtual Methods

            /// Retrieves the type of this asset.
            ///
            /// @remarks
            ///     If a sub-class does not implement this method, it will return AssetType_None.
            virtual GTEngine::AssetType GetType() const;



        private:

            /// The relative path of the asset.
            GTLib::String m_relativePath;

            /// The absolute path of the asset.
            GTLib::String m_absolutePath;

            /// The reference counter for controlling when an asset should be deleted from memory entirely. Defaults to 1.
            unsigned int m_referenceCounter;
        };
    }
}

#endif
