// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsInterfaceAllocator.hpp>

namespace GT
{
	GraphicsInterfaceAllocator::GraphicsInterfaceAllocator()
	{
	}

	GraphicsInterfaceAllocator::~GraphicsInterfaceAllocator()
	{
	}


	bool GraphicsInterfaceAllocator::IsGraphicsInterfaceSupported(GraphicsInterfaceType) const
	{
		return false;
	}

	GraphicsInterface* GraphicsInterfaceAllocator::CreateGraphicsInterface(GraphicsInterfaceType)
	{
		return nullptr;
	}

	void GraphicsInterfaceAllocator::DeleteGraphicsInterface(GraphicsInterface*)
	{
	}
}
