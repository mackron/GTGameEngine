// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsInterface.hpp>

namespace GT
{
	GraphicsInterface::GraphicsInterface(GraphicsInterfaceType type)
		: m_type(type)
	{
	}

	GraphicsInterface::~GraphicsInterface()
	{
	}


	GraphicsInterfaceClass GraphicsInterface::GetClass() const
	{
		return GraphicsInterfaceClass_Unknown;
	}

	GraphicsInterfaceType GraphicsInterface::GetType() const
	{
		return m_type;
	}
}