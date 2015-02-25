// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Rendering/GPUStateObjects.hpp>

namespace GT
{
    namespace GE
    {
        GPURasterizerState::GPURasterizerState(const GPURasterizerStateDesc &desc)
            : m_desc(desc)
        {
        }

        GPURasterizerState::~GPURasterizerState()
        {
        }

        const GPURasterizerStateDesc & GPURasterizerState::GetDesc() const
        {
            return m_desc;
        }
    }
}