/************************************************************************
*                                                                       *
*   device.cpp -- Manages the Direct3D device                           *
*                                                                       *
* Copyright (c) Microsoft Corp. All rights reserved.                    *
*                                                                       *
* This code is licensed under the terms of the                          *
* Microsoft Kinect for Windows SDK (Beta)                               *
* License Agreement: http://kinectforwindows.org/KinectSDK-ToU          *
*                                                                       *
************************************************************************/

#include "stdafx.h"
#include "DrawDevice.h"

const DWORD NUM_BACK_BUFFERS = 2;

inline LONG Width(const RECT& r)
{
    return r.right - r.left;
}

inline LONG Height(const RECT& r)
{
    return r.bottom - r.top;
}


//-------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------

DrawDevice::DrawDevice() : 
    m_hwnd(NULL),
    m_pD3D(NULL),
    m_pDevice(NULL),
    m_pSwapChain(NULL),
    m_height(0),
    m_lDefaultStride(0)
{
}


//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------

DrawDevice::~DrawDevice()
{
    DestroyDevice();
}

//-------------------------------------------------------------------
// CreateDevice
//
// Create the Direct3D device.
//-------------------------------------------------------------------

HRESULT DrawDevice::CreateDevice(HWND hwnd)
{
    if (m_pDevice)
    {
        return S_OK;
    }

    // Create the Direct3D object.
    if (m_pD3D == NULL)
    {
        m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

        if (m_pD3D == NULL)
        {
            return E_FAIL;
        }
    }


    HRESULT hr = S_OK;
    D3DPRESENT_PARAMETERS pp = { 0 };
    D3DDISPLAYMODE mode = { 0 };

    hr = m_pD3D->GetAdapterDisplayMode(
        D3DADAPTER_DEFAULT, 
        &mode
        );

    if (FAILED(hr)) { goto done; }

    hr = m_pD3D->CheckDeviceType(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        mode.Format,
        D3DFMT_X8R8G8B8,
        TRUE    // windowed
        );

    if (FAILED(hr)) { goto done; }

    pp.BackBufferFormat = D3DFMT_X8R8G8B8;
    pp.SwapEffect = D3DSWAPEFFECT_COPY;
    pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;  
    pp.Windowed = TRUE;
    pp.hDeviceWindow = hwnd;

    hr = m_pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hwnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
        &pp,
        &m_pDevice
        );

    if (FAILED(hr)) { goto done; }

    m_hwnd = hwnd;

done:
    return hr;
}

//-------------------------------------------------------------------
// SetVideoType
//
// Set the video format.  
//-------------------------------------------------------------------

HRESULT DrawDevice::SetVideoType( int Width, int Height, int Stride )
{
    HRESULT hr = S_OK;
    D3DPRESENT_PARAMETERS pp = { 0 };

    // Get the frame size.
    m_lDefaultStride = Stride;

    // Create Direct3D swap chain.
	if(m_pSwapChain!=NULL)
	{
		m_pSwapChain->Release( );
		m_pSwapChain=NULL;
	}

    pp.BackBufferWidth  = Width;
    pp.BackBufferHeight = m_height = Height;
    pp.Windowed = TRUE;
    pp.SwapEffect = D3DSWAPEFFECT_FLIP;
    pp.hDeviceWindow = m_hwnd;
    pp.BackBufferFormat = D3DFMT_X8R8G8B8;
    pp.Flags = 
        D3DPRESENTFLAG_VIDEO | D3DPRESENTFLAG_DEVICECLIP |
        D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    pp.BackBufferCount = NUM_BACK_BUFFERS;

    hr = m_pDevice->CreateAdditionalSwapChain(&pp, &m_pSwapChain);
    if (FAILED(hr)) { goto done; }

    // Update the destination rectangle for the correct
    // aspect ratio.
    GetClientRect(m_hwnd, &m_rcDest);

done:
    return hr;
}

//-------------------------------------------------------------------
// DrawFrame
//
// Draw the video frame.
//-------------------------------------------------------------------

HRESULT DrawDevice::DrawFrame( BYTE * pBits )
{
    HRESULT hr = S_OK;
    D3DLOCKED_RECT lr;

    IDirect3DSurface9 *pSurf = NULL;
    IDirect3DSurface9 *pBB = NULL;

    if (m_pDevice == NULL || m_pSwapChain == NULL)
    {
        return S_OK;
    }

    // Get the swap-chain surface.
    hr = m_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pSurf);
    if (FAILED(hr)) { goto done; }

    // Lock the swap-chain surface.
    hr = pSurf->LockRect(&lr, NULL, D3DLOCK_NOSYSLOCK );
    if (FAILED(hr)) { goto done; }

    // Convert the frame. This also copies it to the Direct3D surface.

    BYTE * pDst = (BYTE*) lr.pBits;
    BYTE * pSrc = pBits;

    for( int y = 0 ; y < (int) m_height ; y++ )
    {
        memcpy( pDst, pSrc, m_lDefaultStride );
        pDst += lr.Pitch;
        pSrc += m_lDefaultStride;
    }

    hr = pSurf->UnlockRect();

    if (FAILED(hr)) { goto done; }


    // Color fill the back buffer.
    hr = m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBB);

    if (FAILED(hr)) { goto done; }

    hr = m_pDevice->ColorFill(pBB, NULL, D3DCOLOR_XRGB(0, 0, 0x80));

    if (FAILED(hr)) { goto done; }


    // Blit the frame.

    hr = m_pDevice->StretchRect(pSurf, NULL, pBB, &m_rcDest, D3DTEXF_LINEAR);
    
    if (FAILED(hr)) { goto done; }


    // Present the frame.
    
    hr = m_pDevice->Present(NULL, NULL, NULL, NULL);
    

done:

	if(pSurf != NULL)
		pSurf->Release();

    if(pBB!= NULL)
		pBB->Release();

	return hr;
}


//-------------------------------------------------------------------
// DestroyDevice 
//
// Release all Direct3D resources.
//-------------------------------------------------------------------

void DrawDevice::DestroyDevice()
{
	if(m_pSwapChain!=NULL)
	{
		m_pSwapChain->Release( );
		m_pSwapChain=NULL;
	}

    if(m_pDevice!=NULL)
	{
		m_pDevice->Release( );
		m_pDevice=NULL;
	}
    if(m_pD3D!=NULL)
	{
		m_pD3D->Release( );
		m_pD3D=NULL;
	}
}
