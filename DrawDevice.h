/************************************************************************
*                                                                       *
*   device.h -- Manages the Direct3D device                             *
*                                                                       *
* Copyright (c) Microsoft Corp. All rights reserved.                    *
*                                                                       *
* This code is licensed under the terms of the                          *
* Microsoft Kinect for Windows SDK (Beta)                               *
* License Agreement: http://kinectforwindows.org/KinectSDK-ToU          *
*                                                                       *
************************************************************************/

#pragma once

#include <d3d9.h>

// DrawDevice class

class DrawDevice
{
private:
    HWND                    m_hwnd;

    IDirect3D9              *m_pD3D;
    IDirect3DDevice9        *m_pDevice;
    IDirect3DSwapChain9     *m_pSwapChain;

    // Format information
    UINT                    m_height;
    LONG                    m_lDefaultStride;
    RECT                    m_rcDest;       // Destination rectangle

public:
    DrawDevice();
    virtual ~DrawDevice();

    // Implied bits per pixel is 32
    HRESULT SetVideoType( int Width, int Height, int Stride );
    HRESULT DrawFrame( BYTE * pBits );

    HRESULT CreateDevice(HWND hwnd);
    void    DestroyDevice();
};
