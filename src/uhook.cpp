#include <windows.h>
#include <tchar.h>
#include <commctrl.h>

#include "uhook.h"

#ifdef _MSC_VER
#pragma data_seg(".HUYS")
HHOOK g_hHook = NULL;
HMODULE g_hModule = NULL;
int aa = 0;
BOOL g_bSubclassed = FALSE;
WNDPROC s_OldWndProc = NULL;
WNDPROC s_OldWndProcMain = NULL;
WNDPROC s_OldWndProcMainMenu = NULL;
WNDPROC s_OldWndProcOpen = NULL;
WNDPROC s_OldWndProcImport = NULL;
WNDPROC s_NewWndProc = NULL;
HWND g_hMsgWnd = NULL;
HWND g_hWnd = NULL;
HWND g_hWndMain = NULL;
HWND g_hWndMainMenuBar = NULL;
HWND g_hWndMainToolBar = NULL;
HWND g_hWndLogin = NULL;
HWND g_hWndOpen = NULL;
HWND g_hWndImport = NULL;
int g_TrySubclassWndMode = -1;
#pragma data_seg()
#pragma comment(linker, "/section:.HUYS,RWS")
#endif

#ifdef __GNUC__
HHOOK g_hHook __attribute__((section ("Shared"), shared)) = NULL;
HMODULE g_hModule __attribute__((section ("Shared"), shared)) = NULL;
int aa __attribute__((section ("Shared"), shared)) = 0;
BOOL g_bSubclassed __attribute__((section ("Shared"), shared)) = FALSE;
WNDPROC s_OldWndProc __attribute__((section ("Shared"), shared))  = NULL;
WNDPROC s_NewWndProc __attribute__((section ("Shared"), shared)) = NULL;
HWND g_hMsgWnd __attribute__((section ("Shared"), shared)) = NULL;
HWND g_hWnd __attribute__((section ("Shared"), shared)) = NULL;
#endif


extern "C"
    BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hModule = hModule;
        LOG_STRING("UHook DLL_PROCESS_ATTACH!");
        break;
    case DLL_THREAD_ATTACH:
        g_hModule = hModule;
        LOG_STRING("UHook DLL_THREAD_ATTACH!");
        {
            // Increase DLL reference count
            // Without this, target programs may crash
            // because the hook DLL is unloaded from memory 
            char lib_name[MAX_PATH]; 
            ::GetModuleFileName( hModule, lib_name, MAX_PATH );
            LOG_STRING("LoadLibrary %s", lib_name);
            ::LoadLibrary( lib_name );
        }
        break;
    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        LOG_STRING("UHook DLL_THREAD_DETACH!");
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        LOG_STRING("UHook DLL_PROCESS_DETACH!");
        break;
    }
    return TRUE;

    UNREFERENCED_PARAMETER(lpReserved);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
    {
        return CallNextHookEx(g_hHook, nCode, wParam, lParam);
    }

    if (HC_ACTION == nCode)
    {
        //::MessageBox(NULL, "xx", "xx", MB_OK);
        ::PostMessage(g_hWnd, UWM_MOUSEMOVE, 0, 0);
    }

    return 1;
}

LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam,LPARAM lParam)
{
    if (nCode >= 0)
    {
        if (WM_RBUTTONDBLCLK == wParam)
        {
            ::PostMessage(g_hWnd, UWM_MOUSEMOVE, 0, 0);
            //::SetWindowText(g_hWnd, "GGGGGG");
        }
    }
    return ::CallNextHookEx(g_hHook, nCode, wParam,lParam);
}

enum WMCOPYDATA_MSGID
{
    WM_COPYDATA_MESSAGE_ID_NORMAL = 54321,
    WM_COPYDATA_MESSAGE_ID_CLOSE
};

enum TRYSUBCLASS_MODE
{
    TRYSUBCLASS_NONE        = 0x00,
    TRYSUBCLASS_LOGIN       = 0x01,
    TRYSUBCLASS_MAIN        = 0x01<<1,
    TRYSUBCLASS_OPEN        = 0x01<<2,
    TRYSUBCLASS_IMPORT      = 0x01<<3
};

void SendCopyDataMessage(HWND hWnd, const char * data, UINT MsgId = WM_COPYDATA_MESSAGE_ID_NORMAL)
{
    LRESULT copyDataResult;
    if (hWnd)
    {
        COPYDATASTRUCT cpd;
        cpd.dwData = 0;
        cpd.cbData = strlen(data)+1;
        cpd.lpData = (void*)data;
        copyDataResult = ::SendMessage(hWnd, WM_COPYDATA,
            (WPARAM)MsgId,
            (LPARAM)&cpd);
        // copyDataResult has value returned by other app
    } 
    else 
    {
        LOG_STRING("Unable to send WM_COPYDATA to HWND=null.");
    }
}

const UINT WM_BUSOBJ_EVENT = WM_USER + 105;
const UINT WM_MAINWINDOW_MENU_COMMAND = WM_USER + 106;

static void CheckAndReplaceWindowProcedure(HWND hWnd, WNDPROC & oldWndProc)
{
    if (hWnd != NULL && s_NewWndProc != (WNDPROC)::GetWindowLong(hWnd, GWL_WNDPROC))
    {
        oldWndProc = (WNDPROC)::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)s_NewWndProc);
        //g_bSubclassed = TRUE;    // Set state as subclassed
        //::PostMessage(g_hMsgWnd, 51111, 111, 111);
        LOG_STRING("Subclass HWND=%d", hWnd);
    }

}

//The CBT hook Procedure (Computer Based Training Hook)
static LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    //if (g_hMsgWnd)
    //{
    //    char buffer[128];
    //    wsprintf(buffer, "CBTProc %d (%d) %d\r\n", wParam, GetParent((HWND)wParam), nCode);
    //    SendCopyDataMessage(g_hMsgWnd, buffer);
    //}

    if (nCode==HCBT_ACTIVATE || nCode == HCBT_MINMAX || nCode == HCBT_MOVESIZE 
        || nCode == HCBT_SETFOCUS || nCode == HCBT_CLICKSKIPPED || nCode == HCBT_KEYSKIPPED)  //Called when the application window is activated
    {
        //::PostMessage(g_hMsgWnd, 51111, 111, 111);
        //if((HWND)(wParam) == g_hWnd || g_hWnd == g_hWndLogin)  //check if the window activated is Our Target App
        {
            if (g_bSubclassed == FALSE)
            {
                //if (s_NewWndProc != (WNDPROC)::GetWindowLong(g_hWnd, GWL_WNDPROC))
                //{
                //    s_OldWndProc = (WNDPROC)::SetWindowLong(g_hWnd, GWL_WNDPROC, (LONG)s_NewWndProc);
                //    g_bSubclassed = TRUE;    // Set state as subclassed
                //    //::PostMessage(g_hMsgWnd, 51111, 111, 111);
                //    LOG_STRING("Subclass HWND=%d", g_hWnd);
                //}
                switch (g_TrySubclassWndMode)
                {
                case TRYSUBCLASS_LOGIN:
                    CheckAndReplaceWindowProcedure(g_hWndLogin, s_OldWndProc);
                    break;
                case TRYSUBCLASS_MAIN:
                    CheckAndReplaceWindowProcedure(g_hWndMain, s_OldWndProcMain);
                    CheckAndReplaceWindowProcedure(g_hWndMainMenuBar, s_OldWndProcMainMenu);
                    //CheckAndReplaceWindowProcedure(g_hWndMainToolBar);
                    break;
                case TRYSUBCLASS_OPEN:
                    CheckAndReplaceWindowProcedure(g_hWndOpen, s_OldWndProcOpen);
                    break;
                case TRYSUBCLASS_IMPORT:
                    CheckAndReplaceWindowProcedure(g_hWndImport, s_OldWndProcImport);
                    break;
                default:
                    break;
                }
                g_TrySubclassWndMode = TRYSUBCLASS_NONE;
                g_bSubclassed = TRUE;
            }
        }



        return 0;
    }

    if (nCode==HCBT_DESTROYWND) //Called when the application window is destroyed
    {

        if((HWND)wParam == g_hWnd)
        {
            //SendNotifyMessage(hApp,WM_APP +1024,(WPARAM)wParam,(LPARAM)lParam);// Send the message  to the vb app
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}//End of the hook procedure


static LRESULT CALLBACK CallWndProc(int nCode, WPARAM w, LPARAM l)
{
    char cBuffer[0x80];
    DWORD bytesWritten;
    //Beep(440, 0.2);
    if (nCode < 0)
    {
        PCWPSTRUCT pcwps = (PCWPSTRUCT)l;
        if (pcwps->hwnd == g_hWnd && pcwps->message == WM_COMMAND)
        {
            LOG_STRING("nCode - %08x", nCode);
            LOG_STRING("wParam - %08x \n", w);
            LOG_STRING("PCWPSTRUCT->lParam - %08x \n", pcwps->lParam);
            LOG_STRING("PCWPSTRUCT->wParam - %08x \n", pcwps->wParam);
            LOG_STRING("PCWPSTRUCT->message - %08x \n", pcwps->message);
            LOG_STRING("PCWPSTRUCT->hwnd - %08x \n", pcwps->hwnd);
        }
        return CallNextHookEx(g_hHook, nCode, w, l);
    }
    else
    {
        PCWPSTRUCT pcwps = (PCWPSTRUCT)l;
        if (nCode == HC_ACTION && pcwps->hwnd == g_hWnd && pcwps->message == WM_COMMAND)
        {
            LOG_STRING("nCode - %08x", nCode);
            LOG_STRING("wParam - %08x \n", w);
            LOG_STRING("PCWPSTRUCT->lParam - %08x \n", pcwps->lParam);
            LOG_STRING("PCWPSTRUCT->wParam - %08x \n", pcwps->wParam);
            LOG_STRING("PCWPSTRUCT->message - %08x \n", pcwps->message);
            LOG_STRING("PCWPSTRUCT->hwnd - %08x \n", pcwps->hwnd);
            return CallNextHookEx(g_hHook, nCode, w, l);
        }


        return CallNextHookEx(g_hHook, nCode, w, l);
    }
}

BOOL installCallWndProcHook(HWND hWnd, HWND hMsgWnd)
{
    g_hWnd = hWnd;
    g_hMsgWnd = hMsgWnd;
    if (NULL == g_hHook)
    {
        g_hHook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CallWndProc, g_hModule, 0);
    }
    return TRUE;
}


BOOL setMyHook(HWND hWnd)
{
    //LOG_STRING("SetHook!");

    g_hWnd = hWnd;

    if (NULL == g_hHook)
    {
        //g_hHook = ::SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, g_hModule, 0);
        g_hHook = ::SetWindowsHookEx(WH_MOUSE, MouseProc, g_hModule, 0);
    }

    return NULL != g_hHook;
}

BOOL clearMyHook()
{
    BOOL bRet = FALSE;
    if (g_hHook)
    {
        bRet = ::UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
    }

    return bRet;
}

int getValue()
{
    return aa;
}

void setValue(int val)
{
    aa = val;
}

enum EVENTID
{
    EVENT_LOGON = 20001,
    EVENT_OPEN,
    EVENT_CLOSE,
    EVENT_IMPORT,
    EVENT_EXIT
};

LRESULT newWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const UINT ID_USERID_EDIT_USERNAME = 0x0000697A;
    const UINT ID_USERID_EDIT_PASSWORD = 0x0000697C;

    const UINT ID_MAINWINDOW_MENU_OPEN = 36769;
    const UINT ID_MAINWINDOW_MENU_IMPORT = 40148;
    const UINT ID_MAINWINDOW_MENU_CLOSE = 36770;
    const UINT ID_MAINWINDOW_MENU_EXIT = 39796;
    

    //LOG_STRING("xxxx_WM_COMMAND");
    if (hwnd == g_hWndLogin || hwnd == g_hWndMain || hwnd == g_hWndMainMenuBar || hwnd == g_hWndMainToolBar
        || hwnd == g_hWndOpen || hwnd == g_hWndImport)
    {
        if (uMsg == WM_COMMAND)
        {
            if (HIWORD(wParam) == 0 )
            {
                LOG_STRING("MENU HWND=%d WM_COMMAND=%d", (int)hwnd, LOWORD(wParam));

                if (g_hMsgWnd)
                {
                    char msgContent[MAX_PATH] = {0};
                    wsprintf(msgContent, "MENU HWND=%d WM_COMMAND=%d\r\n", (int)hwnd, LOWORD(wParam));
                    //::SendMessage(g_hMsgWnd, 54321, (WPARAM)msgContent, 0);
                    SendCopyDataMessage(g_hMsgWnd, msgContent);
                    if (LOWORD(wParam) == 1) // IDOK
                    {
                        if (g_hWndLogin!=NULL && hwnd == g_hWndLogin)
                        {
                            wsprintf(msgContent, "%d", (int)hwnd);
                            SendCopyDataMessage(g_hMsgWnd, msgContent, WM_COPYDATA_MESSAGE_ID_CLOSE);
                        }
                        else if (g_hWndOpen != NULL && hwnd == g_hWndOpen)
                        {
                            wsprintf(msgContent, "Open Window %d", (int)hwnd);
                            SendCopyDataMessage(g_hMsgWnd, msgContent, WM_COPYDATA_MESSAGE_ID_CLOSE);
                        }
                    }
                    else if (LOWORD(wParam) == 2) // IDCANCEL
                    {
                        if (g_hWndLogin!=NULL && hwnd == g_hWndLogin)
                        {
                            wsprintf(msgContent, "%d", (int)hwnd);
                            SendCopyDataMessage(g_hMsgWnd, msgContent, WM_COPYDATA_MESSAGE_ID_CLOSE);
                        }
                    }

                    if (hwnd == g_hWndMainMenuBar || hwnd == g_hWndMain)
                    {
                        UINT mid = LOWORD(wParam);
                        SendMessage(g_hMsgWnd, WM_MAINWINDOW_MENU_COMMAND, (WPARAM)mid, 0);
                    }
                }          
            }

            if (HIWORD(wParam) == EN_CHANGE && hwnd == g_hWndLogin)
            {
                UINT id = LOWORD(wParam);
                const char * name = NULL;
                char value[256] = {0};
                if (id == ID_USERID_EDIT_USERNAME)
                {
                    name = "username";
                    GetWindowText((HWND)lParam, value, 255);
                }
                else
                {
                    name = "password";
                    GetWindowText((HWND)lParam, value, 255);
                }

                LOG_STRING("EDIT HWND=%d ID=%d %s %s", lParam, id, name, value);

                if (g_hMsgWnd)
                {
                    char msgContent[MAX_PATH] = {0};
                    wsprintf(msgContent, "EDIT HWND=%d ID=%d %s %s\r\n", lParam, id, name, value);
                    SendCopyDataMessage(g_hMsgWnd, msgContent);
                }  
            }


            switch (LOWORD(wParam))
            {
            case 11112:
                {

                    ::MessageBox(hwnd, "hww1", "info", MB_OK|MB_ICONINFORMATION);
                    break;
                    //return FALSE;
                }
            case 11113:
                {
                    ::MessageBox(hwnd, "hww2", "info", MB_OK|MB_ICONINFORMATION);
                    break;
                    //return FALSE;
                }
            case 2:
                {
                    LOG_STRING("Closing Window");
                    //g_hWnd = NULL;
                }
            default:
                ;
            }
        }
        else if (uMsg == WM_NOTIFY)
        {
            //LOG_STRING("WM_NOTIFY WPARAM=%x LPARAM=%x", (int)hwnd, wParam, lParam);

            //NMHDR * pNMHDR = (NMHDR *)lParam;
            //LOG_STRING("HWND=%d ID=%d Code=%d", pNMHDR->hwndFrom, pNMHDR->idFrom, pNMHDR->code);

            //if (pNMHDR->code == NM_CUSTOMDRAW)
            //{
            //    LPNMCUSTOMDRAW lpnmcd = (LPNMCUSTOMDRAW)lParam;
            //    LOG_STRING("CustomDraw RECT=(%d, %d, %d, %d) DrawStage=%d ItemSpec=%d ItemState=%d Item=%s", 
            //        lpnmcd->rc.left, lpnmcd->rc.top, lpnmcd->rc.right, lpnmcd->rc.bottom,
            //        lpnmcd->dwDrawStage, lpnmcd->dwItemSpec,
            //        lpnmcd->uItemState, (const char *)lpnmcd->lItemlParam);

            //    HWND hTooltip = FindWindow("tooltips_class32", "");

            //    if (hTooltip)
            //    {
            //        char title[MAX_PATH];
            //        GetWindowText(hTooltip, title, MAX_PATH);
            //        LOG_STRING("Item %s", title);
            //    }
            //}

        }
    }

    WNDPROC oldWndProc = s_OldWndProc;

    if (hwnd == g_hWndLogin)
    {
        oldWndProc = s_OldWndProc;
    }
    else if (hwnd == g_hWndMain)
    {
        oldWndProc = s_OldWndProcMain;
    }
    else if (hwnd == g_hWndMainMenuBar)
    {
        oldWndProc = s_OldWndProcMainMenu;
    }
    else if (hwnd == g_hWndOpen)
    {
        oldWndProc = s_OldWndProcOpen;
    }
    else if (hwnd == g_hWndImport)
    {
        oldWndProc = s_OldWndProcImport;
    }

    return CallWindowProc(oldWndProc, hwnd, uMsg, wParam, lParam);
}

ULIB_API BOOL setCBTHook( HWND hWnd, HWND hMsgWnd, DWORD dwThreadID)
{
    g_hWnd = hWnd;
    //s_NewWndProc = (WNDPROC)&newWndProc;
    s_NewWndProc = (WNDPROC)newWndProc;
    g_hMsgWnd = hMsgWnd;

    if (NULL == g_hHook)
    {
        //g_hHook = ::SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, g_hModule, 0);
        g_hHook = ::SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTProc, g_hModule, dwThreadID);
    }

    //::PostMessage(g_hMsgWnd, 51111, 111, 111);

    return NULL != g_hHook;
}



WNDPROC getOldProc()
{
    return s_OldWndProc;
}

BOOL releaseCBTHook()
{
    if (g_bSubclassed && g_hWnd && IsWindow(g_hWnd) && s_OldWndProc)
    {
        ::SetWindowLong(g_hWnd, GWL_WNDPROC, (LONG)s_OldWndProc);
        g_bSubclassed = FALSE;    // Set state as subclassed
        //::PostMessage(g_hMsgWnd, 51111, 111, 111);
    }

    clearMyHook();

    return TRUE;
}

ULIB_API void SetLoginWindow( HWND hWnd )
{
   g_hWndLogin = hWnd;
   g_TrySubclassWndMode = TRYSUBCLASS_LOGIN;
}

ULIB_API void resetCBTHookHWND( HWND hWnd )
{
    if (g_hWnd != hWnd)
    {
        g_bSubclassed = FALSE;
    }

    g_hWnd = hWnd;
}

ULIB_API void SetMainWindow( HWND hWnd )
{
    g_hWndMain = hWnd;
    g_TrySubclassWndMode = TRYSUBCLASS_MAIN;
}

ULIB_API void SetMainWindowMenuBar( HWND hWnd )
{
    g_hWndMainMenuBar = hWnd;
}

ULIB_API void SetMainWindowToolBar( HWND hWnd )
{
    g_hWndMainToolBar = hWnd;
}

ULIB_API void SetOpenWindow( HWND hWnd )
{
    g_hWndOpen = hWnd;
    g_TrySubclassWndMode = TRYSUBCLASS_OPEN;
}

ULIB_API void SetImportWindow( HWND hWnd )
{
    g_hWndImport = hWnd;
    g_TrySubclassWndMode = TRYSUBCLASS_IMPORT;
}

UHook::UHook()
{

}

UHook::~UHook()
{

}
