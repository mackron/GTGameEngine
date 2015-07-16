// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsAPI_hpp_
#define __GT_GraphicsAPI_hpp_

#include "GraphicsTypes.hpp"

namespace GT
{
    /// Base class representing a graphics API.
    class GraphicsAPI
    {
    public:

        /// Constructor.
        GraphicsAPI();

        /// Destructor.
        virtual ~GraphicsAPI();


        /// Initializes the API.
        ///
        /// @return True if the API is supported and initialized successfully.
        virtual bool Startup() = 0;

        /// Shuts down the API.
        virtual void Shutdown() = 0;


        /// Retrieves the API type ID.
        virtual GraphicsAPIType GetType() const = 0;
    };
}

#endif
