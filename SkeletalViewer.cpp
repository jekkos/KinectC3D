/************************************************************************
*                                                                       *
*   SkeletalViewer.cpp -- This module provides sample code used to      *
*                         demonstrate Kinect NUI processing             *
*                                                                       *
* Copyright (c) Microsoft Corp. All rights reserved.                    *
*                                                                       *
* This code is licensed under the terms of the                          *
* Microsoft Kinect for Windows SDK (Beta)                               *
* License Agreement: http://kinectforwindows.org/KinectSDK-ToU          *
*                                                                       *
************************************************************************/

// Note: 
//  1. DirectX SDK must be installed before compiling. 
//  2. DirectX SDK include path should be added after the VC include
//     path, because strsafe.h in DirectX SDK may be older.
//  3. platform SDK lib path should be added before the VC lib
//     path, because uuid.lib in VC lib path may be older

#include "stdafx.h"
#include "SkeletalViewer.h"
#include "resource.h"

static const std::string m_JointLabels[] = {"Hip Center", "Spine", "Shoulder Center", "Head", "Left Shoulder",  
	"Left Elbow", "Left Wrist", "Left Hand", "Right Shoulder", "Right Elbow", 
	"Right Wrist", "Right Hand", "Left Hip", "Left Knee", "Left Ankle", 
	"Left Foot", "Right Hip", "Right Knee", "Right Ankle", "Right Foot"};

// Global Variables:
CSkeletalViewerApp	g_CSkeletalViewerApp;	// Application class
HINSTANCE			g_hInst;				// current instance
HWND				g_hWndApp;				// Windows Handle to main application
TCHAR				g_szAppTitle[256];		// Application title

int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)
{
    MSG			msg;
    WNDCLASS	wc;

    // Store the instance handle
    g_hInst=hInstance;

    LoadString(g_hInst,IDS_APPTITLE,g_szAppTitle,sizeof(g_szAppTitle)/sizeof(g_szAppTitle[0]));

    // Dialog custom window class
    ZeroMemory(&wc,sizeof(wc));
    wc.style=CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra=DLGWINDOWEXTRA;
    wc.hInstance=hInstance;
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_SKELETALVIEWER));
    wc.lpfnWndProc=DefDlgProc;
    wc.lpszClassName=SZ_APPDLG_WINDOW_CLASS;
    if(!RegisterClass(&wc))
        return (0);

    // Create main application window
    g_hWndApp=CreateDialogParam(g_hInst,MAKEINTRESOURCE(IDD_APP),NULL,(DLGPROC) CSkeletalViewerApp::MessageRouter, reinterpret_cast<LPARAM>(&g_CSkeletalViewerApp));

    MSR_NuiSetDeviceStatusCallback( &CSkeletalViewerApp::Nui_StatusProcThunk );

    // Show window
    ShowWindow(g_hWndApp,nCmdShow);
    UpdateWindow(g_hWndApp);

    // Main message loop:
    while(GetMessage(&msg,NULL,0,0)) 
    {
        // If a dialog message
        if(g_hWndApp!=NULL && IsDialogMessage(g_hWndApp,&msg))
            continue;

        // otherwise do default window processing
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

CSkeletalViewerApp::CSkeletalViewerApp()
{
    ::InitializeCriticalSection(&m_critSecUi);
    m_fUpdatingUi = false;
    Nui_Zero();
}

CSkeletalViewerApp::~CSkeletalViewerApp()
{
    ::DeleteCriticalSection(&m_critSecUi);
    Nui_Zero();
    ::SysFreeString(m_instanceId);
}


void CALLBACK CSkeletalViewerApp::Nui_StatusProcThunk(const NuiStatusData *pStatusData)
{
    g_CSkeletalViewerApp.Nui_StatusProc(pStatusData);
}

LRESULT CALLBACK CSkeletalViewerApp::MessageRouter(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    CSkeletalViewerApp *pThis = NULL;
    
    if (uMsg == WM_INITDIALOG)
    {
        pThis = reinterpret_cast<CSkeletalViewerApp*>(lParam);
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<CSkeletalViewerApp*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    if (pThis)
    {
        return pThis->WndProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void CSkeletalViewerApp::StartRecording() {
	// uses printf() format specifications for time
	CString t = CTime::GetCurrentTime().Format(_T("%Hh%Mm%S"));
	if (!m_pWriter) {
		m_RecordedFrames = 1;
		CString str = _T("C:/skeleton_3d");
		CString fileNameStr = str + _T("_") + t + _T(".c3d");
		// Convert a TCHAR string to a LPCSTR
		CT2CA pszConvertedAnsiString (fileNameStr);
		// construct a std::string using the LPCSTR input
		std::string strStd (pszConvertedAnsiString);
		m_pWriter = btk::AcquisitionFileWriter::New();
		m_pWriter->SetFilename(strStd);
		m_pAcquisition = btk::Acquisition::New();
		m_pAcquisition->Init(NUI_SKELETON_POSITION_COUNT + 4, 300);
		for (INT i = 0; i < NUI_SKELETON_POSITION_COUNT; i++) {
			btk::Point::Pointer point = m_pAcquisition->GetPoint(i);
			// set marker point labels
			point->SetLabel(m_JointLabels[i]);
		}

		for (INT i = NUI_SKELETON_POSITION_COUNT; i < NUI_SKELETON_POSITION_COUNT + 4; i++) {
			btk::Point::Pointer point = m_pAcquisition->GetPoint(i);
			// set marker point labels
			point->SetType(btk::Point::Type::Angle);
		}
		// set point unit to meters
		m_pAcquisition->SetPointUnit(btk::Point::Type::Marker, "m");
		m_pAcquisition->SetPointUnit(btk::Point::Type::Angle, "degrees");
		m_pWriter->SetInput(m_pAcquisition);
		m_pWriter->Update();
	}
	if (!m_pAviFile) {
		// capture avi file
		CString str = _T("C:/skeleton_capture");
		CString fileNameStr = str + _T("_") + t + _T(".avi");
		int sizeOfString = (fileNameStr.GetLength() + 1);
		LPTSTR  lpsz = new TCHAR[ sizeOfString ];
		_tcscpy_s(lpsz, sizeOfString, fileNameStr);
		// create a new recording named ouput.avi, without compression at 30fps
		m_pAviFile = new CAviFile(lpsz, 0, 30);
		m_pAviFile->GetLastErrorMessage();
	}
}

void CSkeletalViewerApp::StopRecording() {
	if (m_pAviFile) {
		delete m_pAviFile;
		m_pAviFile = NULL;
	}
	if (m_pWriter) {
		m_pAcquisition->ResizeFrameNumber(m_RecordedFrames);
		// flush pending changes to file
		m_pWriter->Update();
	}
}

void CSkeletalViewerApp::UpdateComboBox()
{
    m_fUpdatingUi = true;
    int numDevices = 0;
    MSR_NUIGetDeviceCount(&numDevices);

    ::SendDlgItemMessage(m_hWnd, IDC_CAMERAS, CB_RESETCONTENT , 0, 0);
    for(int i = 0; i < numDevices; i++)
    {
        WCHAR kinectName[MAX_PATH];
        StringCchPrintfW(kinectName, _countof(kinectName), L"Kinect %d", i);
        ::SendDlgItemMessage(m_hWnd, IDC_CAMERAS, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(kinectName));
    }

    if (m_pNuiInstance && m_pNuiInstance->InstanceIndex() >= 0)
    {
        SendDlgItemMessage(m_hWnd, IDC_CAMERAS, CB_SETCURSEL, m_pNuiInstance->InstanceIndex(), 0);
    }
    else
    {
        SendDlgItemMessage(m_hWnd, IDC_CAMERAS, CB_SETCURSEL, 0, 0);
    }
    m_fUpdatingUi = false;
}

LRESULT CALLBACK CSkeletalViewerApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_INITDIALOG:
        {
            LOGFONT lf;

            // Clean state the class
            Nui_Zero();

            // Bind application window handle
            m_hWnd=hWnd;

            // Initialize and start NUI processing
            Nui_Init();

            // Set the font for Frames Per Second
            GetObject((HFONT) GetStockObject(DEFAULT_GUI_FONT),sizeof(lf),&lf);
            lf.lfHeight*=4;
            m_hFontFPS=CreateFontIndirect(&lf);
            SendDlgItemMessage(hWnd,IDC_FPS,WM_SETFONT,(WPARAM) m_hFontFPS,0);

            UpdateComboBox();
            SendDlgItemMessage (m_hWnd, IDC_CAMERAS, CB_SETCURSEL, 0, 0);
        }
        break;
        case WM_USER_UPDATE_FPS:
        {
            ::SetDlgItemInt( m_hWnd, static_cast<int>(wParam), static_cast<int>(lParam), FALSE );
        }
        break;
        case WM_USER_UPDATE_COMBO:
        {
            UpdateComboBox();
        }
        break;
        case WM_COMMAND:
        if( HIWORD( wParam ) == CBN_SELCHANGE )
        {
            LRESULT index = ::SendDlgItemMessage(m_hWnd, IDC_CAMERAS, CB_GETCURSEL, 0, 0);
            switch (LOWORD(wParam))
            {
            case IDC_CAMERAS:
                {
                    if (!m_fUpdatingUi) // Don't reconnect as a result of updating the combo box.
                    {
                        Nui_UnInit();
                        Nui_Zero();
                        Nui_Init(static_cast<int>(index));
                    }
                }
            break;
            }
        } else if  ( wParam  == IDC_BUTTON1) {
			StartRecording();
			break;
		} else if ( wParam == IDC_BUTTON2) {
			StopRecording();
			break;
		}
        break;
        // If the titlebar X is clicked destroy app
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;

        case WM_DESTROY:
            // Uninitialize NUI
            Nui_UnInit();

            // Other cleanup
            DeleteObject(m_hFontFPS);

            // Quit the main message pump
            PostQuitMessage(0);
            break;

    }
    return (FALSE);
}



/*****************************************************************************/
/* int MessageBoxResourceV(HWND hwnd,UINT nID,UINT nType, ... )
/*
/* Superset of MessageBox functionality allowing a user to specify a String
/* table loaded string 
/*****************************************************************************/
int MessageBoxResource(HWND hwnd,UINT nID,UINT nType)
{
static TCHAR szRes[512];
int         nRet;

LoadString(g_hInst,nID,szRes,sizeof(szRes)/sizeof(szRes[0]));
nRet=::MessageBox(hwnd,szRes,g_szAppTitle,nType);
return (nRet);
}



