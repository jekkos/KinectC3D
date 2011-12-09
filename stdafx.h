/************************************************************************
*                                                                       *
*   stdafx.h -- include file for standard system and project includes   *
*                                                                       *
*   Copyright (c) Microsoft Corp. All rights reserved.                  *
*                                                                       *
* This code is licensed under the terms of the                          *
* Microsoft Kinect for Windows SDK (Beta)                               *
* License Agreement: http://kinectforwindows.org/KinectSDK-ToU          *
*                                                                       *
************************************************************************/

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <afxwin.h>
#include <ole2.h>
#include <atlstr.h>
#include <atltime.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <strsafe.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <mmsystem.h>
#include <assert.h>


extern TCHAR g_szAppTitle[];

#pragma comment (lib, "d3d9.lib")
#pragma comment ( lib, "winmm.lib" )

