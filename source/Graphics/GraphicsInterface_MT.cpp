// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsInterface_MT.hpp>

namespace GT
{
	GraphicsInterface_MT::GraphicsInterface_MT(GraphicsInterfaceType type)
		: GraphicsInterface(type)
	{
	}

	GraphicsInterface_MT::~GraphicsInterface_MT()
	{
	}

	GraphicsInterfaceClass GraphicsInterface_MT::GetClass() const
	{
		return GraphicsInterfaceClass_MT;
	}
}