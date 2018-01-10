#include "resource.h"

#include <windows.h>
#include <tchar.h>
#include <ocidl.h>
#include <olectl.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlguid.h>
#include <exdisp.h>
#include <shldisp.h>

#include "ubasewindow.h"
#include "uwinapp.h"
#include "ufile.h"
#include "aui/aui.h"

static WNDPROC g_pfnOldProc;
static HWND g_hwndButton=NULL;

LRESULT CALLBACK ButtonProc( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam );

class UShellWindow : public UBaseWindow
{
    enum {
        ID_BN_FIND    = 1111,
        ID_BN_TASKBAR = 1112
    };
public:
    UShellWindow()
    : UBaseWindow(NULL, ::GetModuleHandle(NULL))
    {
        this->setTitle(_T("Shell Test 0.0.1"));
        this->setPos(100, 100, 800, 600);

        ::CoInitialize(NULL);
        ::InitCommonControls();
    }

    ~UShellWindow()
    {
        ::CoUninitialize();
    }

    BOOL onCreate()
    {
        this->setIconBig(IDI_APP);

        bnFind = new UButton(this, ID_BN_FIND);
        bnFind->setPos(100, 100, 100, 50);
        bnFind->create();
        bnFind->setWindowText(_T("Find"));

        bnTaskBar = new UButton(this, ID_BN_TASKBAR);
        bnTaskBar->setPos(100, 200, 100, 50);
        bnTaskBar->create();
        bnTaskBar->setWindowText(_T("TaskBar"));

        return UBaseWindow::onCreate();
    }

    BOOL onChar(WPARAM wParam, LPARAM lParam)
    {
        switch (wParam)
        {
        case VK_ESCAPE:
            return UBaseWindow::onClose();
        default:
            return UBaseWindow::onChar(wParam, lParam);
        }
    }
    /* virtual */ BOOL onCommand(WPARAM wParam, LPARAM lParam)
    {
        switch (LOWORD(wParam))
        {
        case ID_BN_FIND:
            return OnFindFiles();
        case ID_BN_TASKBAR:
            return OnTaskbarProperties();
        default:
            return UBaseWindow::onCommand(wParam, lParam);
        }
    }
private:
    AUI::UButtonP bnFind;
    AUI::UButtonP bnTaskBar;
private:
    BOOL OnFindFiles()
    {
        HRESULT sc;
        IShellDispatch *pShellDisp = NULL;
        sc = CoCreateInstance( CLSID_Shell, NULL, CLSCTX_SERVER, IID_IDispatch, (LPVOID *) &pShellDisp );
         if (FAILED(sc)) return FALSE;
        pShellDisp->FindFiles();
        pShellDisp->Release();
        return FALSE;
    }

    BOOL OnTaskbarProperties()
    {
        HRESULT sc;
        IShellDispatch *pShellDisp = NULL;
        sc = CoCreateInstance( CLSID_Shell, NULL, CLSCTX_SERVER,IID_IDispatch, (LPVOID *) &pShellDisp );
        if( FAILED(sc) ) return FALSE;
        pShellDisp->TrayProperties();
        pShellDisp->Release();
        return FALSE;
    }

    VOID OnRunPrograms( VOID )
    {
        HRESULT sc;
        IShellDispatch *pShellDisp = NULL;
        sc = CoCreateInstance( CLSID_Shell, NULL, CLSCTX_SERVER, IID_IShellDispatch, (LPVOID *) &pShellDisp );
        if( FAILED(sc) )
            return;
        pShellDisp->FileRun();
        pShellDisp->Release();
        return;
    }

    VOID OnMinimizeAll( VOID )
    {
        HRESULT sc;
        IShellDispatch *pShellDisp = NULL;
        sc = CoCreateInstance( CLSID_Shell, NULL, CLSCTX_SERVER, IID_IDispatch, (LPVOID *) &pShellDisp );
        if( FAILED(sc) )
            return;
        pShellDisp->MinimizeAll();
        pShellDisp->Release();
        return;
    }

    VOID OnUndoMinimize( VOID )
    {
        HRESULT sc;
        IShellDispatch *pShellDisp = NULL;
        sc = CoCreateInstance( CLSID_Shell, NULL, CLSCTX_SERVER, IID_IDispatch, (LPVOID *) &pShellDisp );
        if( FAILED(sc) )
            return;
        pShellDisp->UndoMinimizeALL();
        pShellDisp->Release();
        return;
    }

    VOID OnAddTab( HWND hWnd )
    {
        static BOOL g_bFirstTime=TRUE;
        HRESULT sc;
        ITaskbarList *pDisp = NULL;
        sc = CoCreateInstance( CLSID_TaskbarList, NULL, CLSCTX_SERVER, IID_ITaskbarList, (LPVOID *) &pDisp );
        if( FAILED(sc) )
            return;
        // call the first time only
        if( g_bFirstTime )
        {
            g_bFirstTime = FALSE;
            pDisp->HrInit();
            // create a new button window
            g_hwndButton = CreateWindow( "button", "My Button", WS_CLIPSIBLINGS|BS_PUSHBUTTON,0, 0, 58, 14, hWnd, NULL,g_hInstance, NULL );
            g_pfnOldProc = (WNDPROC) SubclassWindow( g_hwndButton, ButtonProc );
        }
        pDisp->AddTab( g_hwndButton );
        pDisp->Release();
        return;
    }

    VOID OnDeleteTab( VOID )
    {
        HRESULT sc;
        ITaskbarList *pDisp = NULL;
        sc = CoCreateInstance( CLSID_TaskbarList, NULL, CLSCTX_SERVER, IID_ITaskbarList, (LPVOID *) &pDisp );
        if( FAILED(sc) )
            return;
        pDisp->DeleteTab( g_hwndButton );
        pDisp->Release();
        return;
    }

    static VOID OnButtonActivation( VOID )
    {
        HMENU hmenu;
        RECT r;
        LONG x, y;
        // get some window handles
        HWND h0=FindWindow("Shell_TrayWnd", NULL );
        HWND h1=FindWindowEx(h0,NULL,"RebarWindow32", NULL);
        HWND h2=FindWindowEx(h1,NULL,"MSTaskSwWClass", NULL);
        HWND h3=FindWindowEx(h2,NULL,"SysTabControl32", NULL);
        GetWindowRect( h3, &r );
        // get the currently selected button and
        // create a new popup menu
        hmenu = CreatePopupMenu();
        INT i=TabCtrl_GetCurSel( h3 );
        if( i==-1 )
        {
            AppendMenu( hmenu, MF_STRING, IDC_DELETETAB,"&Close" );
        }
        else
        {
            AppendMenu( hmenu, MF_STRING, IDC_MINIMIZE,"&Minimize All" );
            AppendMenu( hmenu, MF_STRING, IDC_UNDOMINIMIZE,"&Undo Minimize All" );
            AppendMenu( hmenu, MF_SEPARATOR, 0, NULL );
            AppendMenu( hmenu, MF_STRING, IDC_PROPERTIES,"&Taskbar Properties" );
        }
        // set and immediately reset its size to get
        // the current width and height
        LONG l = TabCtrl_SetItemSize( h3, 0, 0 );
        TabCtrl_SetItemSize( h3, LOWORD(l), HIWORD(l) );
        // have the menu to appear just above the button
        if( i==-1 )
        {
            POINT pt;
            GetCursorPos( &pt );
            x = pt.x;
            y = pt.y;
        }
        else
        {
            x = r.left + LOWORD(l)*i+3;
            y = GetSystemMetrics(SM_CYSCREEN)-(HIWORD(l)+1);
        }
        TrackPopupMenu( hmenu, TPM_BOTTOMALIGN, x, y, 0, g_hDlg, 0);
        DestroyMenu( hmenu );
        return;
    }
};

LRESULT CALLBACK ButtonProc( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uiMsg )
    {
    case WM_NCACTIVATE:
      if( wParam==TRUE )
        UShellWindow::OnButtonActivation();
        break;
    }
    return CallWindowProc( g_pfnOldProc, hwnd, uiMsg, wParam, lParam );
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpszCmdLine, int nCmdShow)
{
    UWinApp app;
    app.setMainWindow(new UShellWindow);
    app.init(hInstance);
    return app.run();
}
