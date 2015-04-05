// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_D3D12Common_hpp_
#define __GT_D3D12Common_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_BUILD_D3D12)

#if defined(GT_PLATFORM_WINDOWS)
    #include <GTLib/windows.hpp>
    #include <dxgi1_4.h>
#endif

#include <d3d12.h>

namespace GT
{
    typedef HRESULT (WINAPI *PFN_CreateDXGIFactory2) (UINT flags, REFIID riid, _Out_ void **ppFactory);
}

#endif

#endif
