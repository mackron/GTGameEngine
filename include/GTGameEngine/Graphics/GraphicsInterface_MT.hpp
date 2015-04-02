// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsInterface_MT_hpp_
#define __GT_GraphicsInterface_MT_hpp_

#include "GraphicsInterface.hpp"

namespace GT
{
	/// Base class for modern, multi-threaded rendering APIs.
	///
	/// This is suited for Vulkan/D3D12/Metal style APIs.
	class GraphicsInterface_MT : public GraphicsInterface
	{
	public:

		/// Constructor.
		GraphicsInterface_MT(GraphicsInterfaceType type);

		/// Destructor.
		virtual ~GraphicsInterface_MT();


		/// GraphicsInterface::GetClass()
		GraphicsInterfaceClass GetClass() const;
	};
}

#endif

