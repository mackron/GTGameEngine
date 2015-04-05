// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsInterfaceAllocator_hpp_
#define __GT_GraphicsInterfaceAllocator_hpp_

#include "GraphicsTypes.hpp"

namespace GT
{
	class GraphicsInterface;


	/// Class used for allocating and deleting graphics interfaces.
	class GraphicsInterfaceAllocator
	{
	public:

		/// Constructor.
		GraphicsInterfaceAllocator();

		/// Destructor.
		virtual ~GraphicsInterfaceAllocator();



		/// Determines whether or not the given graphics interface type is supported by this allocator.
		///
		/// @param type [in] The graphics interface type that needs to be allocated.
		///
		/// @return True if the given graphics interface type is supported.
		virtual bool IsGraphicsInterfaceSupported(GraphicsInterfaceType type) const;

		/// Creates a graphics interface from the given type.
		///
		/// @param type [in] The graphics interface type to create.
		///
		/// @return A pointer to the new graphics interface object, or null if there was an error creating it.
		virtual GraphicsInterface* CreateGraphicsInterface(GraphicsInterfaceType type);

		/// Deletes the given graphics interface.
		///
		/// @param pGraphicsInterface [in] A pointer to the graphics interface to delete.
		virtual void DeleteGraphicsInterface(GraphicsInterface* pGraphicsInterface);
	};
}

#endif
