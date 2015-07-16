// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsAPI_Null_hpp_
#define __GT_GraphicsAPI_Null_hpp_

#include "GraphicsAPI.hpp"

namespace GT
{
    /// Base class representing a null graphics API.
    class GraphicsAPI_Null : public GraphicsAPI
    {
    public:

        /// Constructor.
        GraphicsAPI_Null();

        /// Destructor.
        ~GraphicsAPI_Null();


        /// @copydoc GraphicsAPI::Startup().
        bool Startup();

        /// @copydoc GraphicsAPI::Shutdown()
        void Shutdown();


        /// Retrieves the API type ID.
        GraphicsAPIType GetType() const;
    };
}

#endif
