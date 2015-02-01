// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GameDisplay.hpp>

namespace GT
{
    namespace GE
    {
        GameDisplay::GameDisplay(GPURenderingDevice &renderingDevice)
            : m_renderingDevice(renderingDevice)
        {
        }

        GameDisplay::~GameDisplay()
        {
        }


        GPURenderingDevice & GameDisplay::GetRenderingDevice()
        {
            return m_renderingDevice;
        }
    }
}