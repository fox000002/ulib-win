/*
 * =====================================================================================
 *
 *       Filename:  busobj.cpp
 *
 *    Description:  BusinessObjects Automation
 *
 * =====================================================================================
 */

 #include "resource.h"

#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include <string>
#include <dbghelp.h>

#include "udialogx.h"
#include "udlgapp.h"
#include "uedit.h"
#include "uthread.h"
#include "uicon.h"
#include "umsg.h"
#include "ushell.h"

#include "adt/uautoptr.h"
#include "adt/ustring.h"

#include "uhook.h"

typedef huys::ADT::UStringAnsi TString;

using huys::UDialogBox;

const char * DIALOG_WINDOWCLASS = "#32770";
const char * USER_IDENTIFICATION_DIALOG_WINDOWTITLE = "User Identification";
const UINT ID_USERID_COMBOBOX_SYSTEM = 0x0000697B;
const UINT ID_USERID_COMBOBOX_AUTHENTICATION = 0x0000698F;
const UINT ID_USERID_EDIT_USERNAME = 0x0000697A;
const UINT ID_USERID_EDIT_PASSWORD = 0x0000697C;
const UINT ID_USERID_BUTTON_OK = 0x00000001;

const char * MACRO_DIALOG_WINDOWTITLE = "BusinessObjects";
const UINT ID_MACRO_BUTTON_ENABLE = 0x000003EB;

const char * DESKTOP_INTELLIGENCE_WINDOWCLASS = "Afx:00400000:0:00000000:00000000:009107D3";

HWND hToolbarStandard = NULL;

const char * TOOLBAR_WINDOWTITLE = "Standard";
const char * TOOLBAR_WINDOWCLASS = "ToolbarWindow32";
const char * DATAMANAGER_WINDOWTITLE = "Data Manager";
const char * FREEHANDSQL_WINDOW_TILTE = "Free-hand SQL";

const char * IMPORT_WINDOWTITLE = "Import";
const char * ERROR_WINDOWTITLE = "Error";
const char * OPEN_WINDOWTITLE = "Open";

//const char * repfile = "mmxi3-fhsql.rep";

const char * repfile = "twbi-fhsql.rep";

const int CMD_DATAMANAGER = 38928;

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
    IN HANDLE hProcess,
    IN DWORD ProcessId,
    IN HANDLE hFile,
    IN MINIDUMP_TYPE DumpType,
    IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, OPTIONAL
    IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, OPTIONAL
    IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL
    );

#pragma comment(lib, "Dbghelp.lib")

BOOL CreateDumpFile(LPCTSTR lpszDumpPath, DWORD dwProcessId, int miniDumpType)
{      
    BOOL bRet = FALSE;
    //TCHAR szDbgHelpPath[_MAX_PATH] = _T("dbghelp.dll");
    //::GetModuleFileName(NULL, szDbgHelpPath, _countof(szDbgHelpPath));
    //LPTSTR p = _tcsrchr(szDbgHelpPath, _T('\\'));
    //    *(p+1) = 0;
    //_tcsncat(szDbgHelpPath, _T("dbghelp.dll"), _MAX_PATH - _tcslen(szDbgHelpPath) - 1);

    //static HMODULE hDll = NULL;
    //if (hDll == NULL)
    //    hDll = ::LoadLibrary(szDbgHelpPath);
    //assert(hDll);

    //if (hDll)
    //{
        //MINIDUMPWRITEDUMP pWriteDumpFun = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
        //if (pWriteDumpFun)
        //{
            // create the file
            HANDLE hFile = ::CreateFile(lpszDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL, NULL);

            if (hFile != INVALID_HANDLE_VALUE)
            {      
                // write the dump
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
                assert(hProcess);
                bRet = MiniDumpWriteDump(hProcess, dwProcessId, hFile, (MINIDUMP_TYPE)miniDumpType, 0, NULL, NULL);
                assert(bRet);
                ::CloseHandle(hProcess);
                ::CloseHandle(hFile);
            }
        //}
    //}

    //assert(bRet);
    return bRet;
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
    char title[256] = {0};
    ::GetWindowText(hwnd, title, 256);
    if (0 == strncmp(title, "Standard", 8))
    {
        hToolbarStandard = hwnd;
        return TRUE;
    }
    return TRUE; // must return TRUE; If return is FALSE it stops the recursion
}

DWORD startProgram( const TCHAR *cmdline,
                  const TCHAR *wdir,
                  BOOL bWait /* = FALSE */ )
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    BOOL bResult;

    ::ZeroMemory( &si, sizeof(STARTUPINFO) );
    si.cb = sizeof(si);
    si.dwFlags=STARTF_USESHOWWINDOW;
    si.wShowWindow=SW_NORMAL;

    ::ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );

    bResult = ::CreateProcess(NULL,
        (TCHAR *)cmdline,
        NULL,
        NULL,
        TRUE,
        NORMAL_PRIORITY_CLASS,
        NULL,
        wdir,
        &si,
        &pi);

    if (!bResult) return FALSE;

    if (bWait)
    {
        ::WaitForSingleObject(pi.hProcess, INFINITE);
    }

    //g_hChildProcess = pi.hProcess;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return pi.dwProcessId;
}

enum {
    IDTIMER_TRY_HOOKING_MAINWINDOW = 1000,
    IDTIMER_TRY_HOOKING_OPENWINDOW,
    IDTIMER_TRY_HOOKING_IMPORTWINDOW,
    IDTIMER_TRY_HOOKING_LOGINWINDOW
};

int waitForTimer()
{
    HANDLE hTimer = NULL;
    LARGE_INTEGER liDueTime;

    liDueTime.QuadPart = -10000000LL;

    // Create an unnamed waitable timer.
    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (NULL == hTimer)
    {
        printf("CreateWaitableTimer failed (%d)\n", GetLastError());
        return 1;
    }

    printf("Waiting for 10 seconds...\n");

    // Set a timer to wait for 10 seconds.
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
    {
        printf("SetWaitableTimer failed (%d)\n", GetLastError());
        return 2;
    }

    // Wait for the timer.

    if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
    {
        LOG_STRING("WaitForSingleObject failed (%d)\n", GetLastError());
    }
    else
    {
        LOG_STRING("Timer was signaled.\n");
    }

    return 0;
}

// BUSOBJ.EXE_PID|EVENTID | USER | OS | DATETIME | EVENTNAME | EVENTDETAIL | EVENTVALUE
// 19221 |20001 | Yuan | WIN764BITSP2 | LOGON | USERNAME | Administrator


enum EVENTID
{
    EVENT_LOGON = 20001,
    EVENT_OPEN,
    EVENT_CLOSE,
    EVENT_IMPORT,
    EVENT_EXIT
};

class UDialogBusObj : public UDialogBox
{
    enum {
        IDC_EDIT_VIEW = 1111,
        WM_MAINWINDOW_MENU_COMMAND = WM_USER + 106
    };
    enum {
        ID_MAINWINDOW_MENU_OPEN = 36769,
        ID_MAINWINDOW_MENU_IMPORT = 40148,
        ID_MAINWINDOW_MENU_CLOSE = 36770,
        ID_MAINWINDOW_MENU_EXIT = 39796
    };
public:
    UDialogBusObj(HINSTANCE hInst, UINT nID)
    : UDialogBox(hInst, nID),
    dwPidBusObj(0),
    hWndDataManager(NULL),
    hWndDesktopIntelligence(NULL)
    {}


    BOOL onInit()
    {
        m_ico.loadIconEx(m_hInst, IDI_FLUENT);
        this->setDlgIconBig(m_ico.getHICON());

       // m_pBack = new UBitmap(IDB_BACKGROUND, m_hInst);
        RECT rc;
        ::GetClientRect(m_hDlg, &rc);
        rc.left += 20;
        rc.right -= 20;
        rc.top += 20;
        rc.bottom -= 200;

        m_pEdit = new UEdit(m_hDlg, IDC_EDIT_VIEW, m_hInst);
        m_pEdit->setRect(&rc);
        m_pEdit->setStyles(WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN);
        m_pEdit->create();

        this->addLines("Hello SAP BusinessObjects Guys!\r\n");

        return TRUE;
    }

    virtual BOOL DialogProc(UINT message, WPARAM wParam, LPARAM lParam)
    {

        if (message == WM_COPYDATA)
        {
            if (wParam == 54321)
            {
                PCOPYDATASTRUCT pMyCDS = (PCOPYDATASTRUCT) lParam;
                const char * content = (const char *)pMyCDS->lpData; 
                addLines(content);
            }
            else if (wParam == 54322) // IDOK
            {
                //PostMessage(m_hDlg, WM_TRY_HOOKING_MAINWINDOW, 0, 0);
                this->setTimer(IDTIMER_TRY_HOOKING_MAINWINDOW, 100);
            }
        }

        if (message == WM_MAINWINDOW_MENU_COMMAND)
        {
            UINT id = wParam;

            switch (id)
            {
            case ID_MAINWINDOW_MENU_OPEN:
                addLines("EVENT OPEN\r\n");
                this->setTimer(IDTIMER_TRY_HOOKING_OPENWINDOW, 100);
                break;
            case ID_MAINWINDOW_MENU_IMPORT:
                addLines("EVENT IMPORT\r\n");
                this->setTimer(IDTIMER_TRY_HOOKING_IMPORTWINDOW, 100);
                break;
            case ID_MAINWINDOW_MENU_CLOSE:
                addLines("EVENT CLOSE\r\n");
                break;
            case ID_MAINWINDOW_MENU_EXIT:
                addLines("EVENT EXIT\r\n");
                break;
            default:
                break;
            }
        }

        BOOL result = UDialogBox::DialogProc(message, wParam, lParam);
        return result;
    }

    virtual BOOL onTimer(WPARAM wParam, LPARAM lParam)
    {
        if (wParam == IDTIMER_TRY_HOOKING_MAINWINDOW)
        {
            if (hookMainWindow() || isLoginFailed())
            {
                this->killTimer(IDTIMER_TRY_HOOKING_MAINWINDOW);
            }
        }
        else if (wParam == IDTIMER_TRY_HOOKING_OPENWINDOW)
        {
             if (hookOpenWindow())
             {
                 this->killTimer(IDTIMER_TRY_HOOKING_OPENWINDOW);
             }
        }
        else if (wParam == IDTIMER_TRY_HOOKING_IMPORTWINDOW)
        {
             if (hookImportWindow())
             {
                 this->killTimer(IDTIMER_TRY_HOOKING_IMPORTWINDOW);
             }
        }
        else if (wParam == IDTIMER_TRY_HOOKING_LOGINWINDOW)
        {
            if (hookLoginWindow(dwPidBusObj))
            {
                this->killTimer(IDTIMER_TRY_HOOKING_LOGINWINDOW);
            }
        }

        return FALSE;
    }

    virtual BOOL onCommand(WPARAM wParam, LPARAM lParam)
    {
        switch (LOWORD(wParam))
        {
        case IDC_BN_OPENREP:
            return onBnOpenRep();
        case IDC_BN_FLUENT:
            return onBnFluent();
        case IDC_BN_EXIT:
            {
                releaseCBTHook();
            }
            return UDialogBox::onCancel();
        case IDC_BN_SENDCMD:
            return onBnSendCmd();
        case  IDC_BN_HOOKUID:
            return hookLoginWindow();
        case IDC_BN_HOOKMAIN:
            return hookMainWindow();
        case IDOK:
        default:
            return UDialogBox::onCommand(wParam, lParam);
        }
    }
private:
//    UBitmap *m_pBack;
    huys::ADT::UAutoPtr<UEdit> m_pEdit;

    UIcon m_ico;
    //
    //std::string m_strBuf;
    TString m_strBuf;
private:
    void addLines(const char *sLines)
    {
        m_strBuf += sLines;
        m_pEdit->setText(m_strBuf);
        m_pEdit->scrollToEnd();
    }

    BOOL onBnOpenRep()
    {
        addLines("-->Start OpenRep.\r\n");
        //UShell::open(repfile);
        //UShell::executeEx(NULL, "C:\\Program Files (x86)\\Business Objects\\BusinessObjects Enterprise 12.0\\win32_x86\\busobj.exe",
        //    repfile, NULL, SW_HIDE);
        TString cmdline;
        cmdline.format("C:\\Program Files (x86)\\Business Objects\\BusinessObjects Enterprise 12.0\\win32_x86\\busobj.exe");

        dwPidBusObj = startProgram(cmdline, "C:\\Program Files (x86)\\Business Objects", FALSE);

        if (!dwPidBusObj)
        {
            cmdline.format("C:\\Program Files\\Business Objects\\BusinessObjects Enterprise 12.0\\win32_x86\\busobj.exe");
            dwPidBusObj = startProgram(cmdline, "C:\\Program Files (x86)\\Business Objects", FALSE);
        }

        while (!hookLoginWindow(dwPidBusObj))
        {
            waitForTimer();
        }

        return FALSE;
    }

    DWORD dwPidBusObj;

    HWND hWndDataManager;
    HWND hWndDesktopIntelligence;

    BOOL isTopWindowByTitle(HWND parent, const char * title)
    {
        char buffer[256] = {0};

        if (parent == NULL)
        {
            return FALSE;
        }
        HWND hTopWindow = GetTopWindow(parent);
        GetWindowText(hTopWindow, buffer, 256);

        return strcmp(buffer, title) == 0;
    }

    BOOL onBnFluent()
    {
        addLines("-->Dump Import window.\r\n");
        HWND hWndImport = NULL;
        if ((hWndImport = ::FindWindow(DIALOG_WINDOWCLASS,
            IMPORT_WINDOWTITLE)) != NULL )
        {

            DWORD pid = 0;

            DWORD tid = ::GetWindowThreadProcessId(hWndImport, &pid);

            //setCBTHook(hWndImport, m_hDlg, tid);

            dumpProcessByPid(pid);
        }

  //      //HWND hWndDesktopIntelligence = NULL;
  //      TString mainWindowTitle;
  //      mainWindowTitle.format("Desktop Intelligence - %s", repfile);

  //      addLines("-->OperateUserID.\r\n");
        //HWND hUserId = NULL;
        //if ((hUserId = ::FindWindow(DIALOG_WINDOWCLASS,
        //	USER_IDENTIFICATION_DIALOG_WINDOWTITLE)) != NULL )
        //{
        //	int sel = ::SendMessage(::GetDlgItem(hUserId, ID_USERID_COMBOBOX_AUTHENTICATION), 
        //		CB_GETCURSEL, 0, 0);
        //	TString message;
        //	message.format("CurSel : %d\r\n", sel);
        //	addLines(message);
        //	::SendMessage(::GetDlgItem(hUserId, ID_USERID_COMBOBOX_AUTHENTICATION), 
        //		CB_SETCURSEL, 2, 0);
        //	sel = ::SendMessage(::GetDlgItem(hUserId, ID_USERID_COMBOBOX_AUTHENTICATION), 
        //		CB_GETCURSEL, 0, 0);
        //	message.format("CurSel : %d\r\n", sel);
        //	addLines(message);
        //	
        //	//
        //	::SetFocus(::GetDlgItem(hUserId, ID_USERID_BUTTON_OK));
        //	::SendMessage(::GetDlgItem(hUserId, ID_USERID_BUTTON_OK), WM_LBUTTONDOWN, 0, 0);
        //	::SendMessage(::GetDlgItem(hUserId, ID_USERID_BUTTON_OK), WM_LBUTTONUP, 0, 0);
        //}
        //
        ////Sleep(2000);
  //      while(hUserId && ::FindWindow(DIALOG_WINDOWCLASS, MACRO_DIALOG_WINDOWTITLE) == NULL)
  //      {
  //          addLines("Wait for Macro Dialog\r\n");
  //          Sleep(1000);
  //      }
        //
        //HWND hWndMacro = NULL;
        //if ((hWndMacro = ::FindWindow(DIALOG_WINDOWCLASS,
        //	MACRO_DIALOG_WINDOWTITLE)) != NULL )
        //{
        //	//
        //	::SetFocus(::GetDlgItem(hWndMacro, ID_MACRO_BUTTON_ENABLE));
        //	::SendMessage(::GetDlgItem(hWndMacro, ID_MACRO_BUTTON_ENABLE), WM_LBUTTONDOWN, 0, 0);
        //	::SendMessage(::GetDlgItem(hWndMacro, ID_MACRO_BUTTON_ENABLE), WM_LBUTTONUP, 0, 0);			
        //}
        //

        //
  //      while(::FindWindow(NULL, mainWindowTitle) == NULL)
  //      {
  //          addLines("Wait for Desktop Intelligence Main Window\r\n");
  //          Sleep(1000);
  //      }

        //if ((hWndDesktopIntelligence = ::FindWindow(NULL, mainWindowTitle)) != NULL)
        //{
        //	//
        //	addLines("Find DesktopIntelligence\r\n");

  //          HWND hTopWindow = ::GetTopWindow(hWndDesktopIntelligence);

            //EnumChildWindows(hWndDesktopIntelligence, EnumChildProc, 0);
            ////hToolbarStandard = ::GetDlgItem(hWndDesktopIntelligence, 0x0000E803);
            //if (hToolbarStandard != NULL)
            //{
            //	addLines("Find Toolbar Standard\r\n");
            //	TBBUTTON btn = {0};
            //	
            //	int count = ::SendMessage(hToolbarStandard, TB_BUTTONCOUNT, 0, 0);
            //	TString message;
            //	message.format("Count = %d\r\n", count);
            //	addLines(message);
            //	
   //             DWORD dwProcessId;
   //             LPVOID lpRemoteBuffer;
   //             DWORD lpNumberOfBytesRead;
   //             HANDLE hProc;

   //             GetWindowThreadProcessId(hToolbarStandard, &dwProcessId);
   //             hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, dwProcessId);

   //             //Allocate RemoteBuffer
   //             lpRemoteBuffer = VirtualAllocEx(hProc, NULL, sizeof(TBBUTTON), MEM_COMMIT, PAGE_READWRITE);

   //             //ReadProcessMemory(hProc, (LPVOID)btn.dwData, &ent, sizeof(BUTTONDATA), &lpNumberOfBytesRead);


            //	for (int i=1; i<count; ++i)
            //	{
            //		//if (::SendMessage(hToolbarStandard, TB_GETBUTTON, (WPARAM)25 /*index*/, (LPARAM)lpRemoteBuffer))

   //                 ::SendMessage(hToolbarStandard, TB_GETBUTTON, (WPARAM)i, (LPARAM)lpRemoteBuffer);

   //                 ReadProcessMemory(hProc, lpRemoteBuffer, &btn, sizeof(TBBUTTON), &lpNumberOfBytesRead);
            //		{					
            //			message.format("iCmd = %d %d\r\n", btn.idCommand, btn.iString);
            //			addLines(message);

            //		}
            //	}

                //PostMessage(hWndDesktopIntelligence, WM_COMMAND, 38928, 0);
            //}

  //          if ( ::FindWindow(DIALOG_WINDOWCLASS, DATAMANAGER_WINDOWTITLE) == NULL &&
  //              ::FindWindow(DIALOG_WINDOWCLASS, FREEHANDSQL_WINDOW_TILTE) == NULL )
  //          {
  //              PostMessage(hWndDesktopIntelligence, WM_COMMAND, CMD_DATAMANAGER, 0);
  //          }
        //}

  //      while(::FindWindow(DIALOG_WINDOWCLASS, DATAMANAGER_WINDOWTITLE) == NULL)
  //      {
  //          addLines("Wait for Data Manager\r\n");
  //          Sleep(1000);
  //      }
  //      if ((hWndDataManager= ::FindWindow(DIALOG_WINDOWCLASS, DATAMANAGER_WINDOWTITLE)) != NULL &&
  //          ::FindWindow(DIALOG_WINDOWCLASS, FREEHANDSQL_WINDOW_TILTE) == NULL)
  //      {
  //          UINT ID_DATAMAN_EDIT = 0x000064E0;
  //          //
  //          ::SetFocus(::GetDlgItem(hWndDataManager, ID_DATAMAN_EDIT));
  //          ::SendMessage(::GetDlgItem(hWndDataManager, ID_DATAMAN_EDIT), WM_LBUTTONDOWN, 0, 0);
  //          ::SendMessage(::GetDlgItem(hWndDataManager, ID_DATAMAN_EDIT), WM_LBUTTONUP, 0, 0);	
  //      }

  //      while(::FindWindow(DIALOG_WINDOWCLASS, FREEHANDSQL_WINDOW_TILTE) == NULL)
  //      {
  //          addLines("Wait for FreeHand SQL\r\n");
  //          Sleep(1000);
  //      }

  //      HWND hWndFreehandSQL = NULL;
  //      if ((hWndFreehandSQL = ::FindWindow(DIALOG_WINDOWCLASS, FREEHANDSQL_WINDOW_TILTE)) != NULL)
  //      {
  //          UINT ID_CONNECTINFO_BUTTON = 0x0000044E;
  //          //
  //          ::SetFocus(::GetDlgItem(hWndFreehandSQL, ID_CONNECTINFO_BUTTON));
  //          ::SendMessage(::GetDlgItem(hWndFreehandSQL, ID_CONNECTINFO_BUTTON), WM_LBUTTONDOWN, 0, 0);
  //          ::PostMessage(::GetDlgItem(hWndFreehandSQL, ID_CONNECTINFO_BUTTON), WM_LBUTTONUP, 0, 0);	
  //      }

  //      {
  //          DWORD dwProcessId;
  //          HANDLE hProc;

  //          if (hWndDesktopIntelligence)
  //          {
  //              GetWindowThreadProcessId(hWndDesktopIntelligence, &dwProcessId);
  //              hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, dwProcessId);
  //              CreateDumpFile("rep.dmp", dwProcessId, 
  //                  MiniDumpNormal|MiniDumpWithThreadInfo|MiniDumpWithHandleData|MiniDumpWithIndirectlyReferencedMemory);
  //          }
  //      }

  //      HWND hWndConnectionInfo = NULL;
  //      if ((hWndConnectionInfo = ::FindWindow(DIALOG_WINDOWCLASS, "Edit some-other-mssql-free connection")) != NULL)
  //      {
  //          UINT ID_CANCEL_BUTTON = 0x0000002;
  //          //
  //          ::SetFocus(::GetDlgItem(hWndConnectionInfo, ID_CANCEL_BUTTON));
  //          ::PostMessage(::GetDlgItem(hWndConnectionInfo, ID_CANCEL_BUTTON), WM_LBUTTONDOWN, 0, 0);
  //          ::PostMessage(::GetDlgItem(hWndConnectionInfo, ID_CANCEL_BUTTON), WM_LBUTTONUP, 0, 0);	
  //      }
        //
  //      if ((hWndFreehandSQL = ::FindWindow(DIALOG_WINDOWCLASS, FREEHANDSQL_WINDOW_TILTE)) != NULL)
  //      {
  //          UINT ID_CANCEL_BUTTON = 0x00000002;
  //          //
  //          ::SetFocus(::GetDlgItem(hWndFreehandSQL, ID_CANCEL_BUTTON));
  //          ::PostMessage(::GetDlgItem(hWndFreehandSQL, ID_CANCEL_BUTTON), WM_LBUTTONDOWN, 0, 0);
  //          ::PostMessage(::GetDlgItem(hWndFreehandSQL, ID_CANCEL_BUTTON), WM_LBUTTONUP, 0, 0);	
  //      }

  //      if ((hWndDesktopIntelligence = ::FindWindow(NULL, mainWindowTitle)) != NULL)
  //      {
  //          //
  //          ::PostMessage(hWndDesktopIntelligence, WM_CLOSE, 0, 0);
  //      }

  //      HWND hWndExitMessageBox = NULL;
  //      if ((hWndExitMessageBox = ::FindWindow(DIALOG_WINDOWCLASS, "Desktop Intelligence")))
  //      {
  //          UINT ID_NO_BUTTON = 0x00000007;
  //          //
  //          ::SetFocus(::GetDlgItem(hWndExitMessageBox, ID_NO_BUTTON));
  //          ::PostMessage(::GetDlgItem(hWndExitMessageBox, ID_NO_BUTTON), WM_LBUTTONDOWN, 0, 0);
  //          ::PostMessage(::GetDlgItem(hWndExitMessageBox, ID_NO_BUTTON), WM_LBUTTONUP, 0, 0);	
  //      }

        return FALSE;
    }

    BOOL onFluentExit()
    {
        addLines("-->Fluent exit.\r\n");
        return FALSE;
    }

    BOOL onGambitExit()
    {
        addLines("-->Gambit exit.\r\n");
        //::MessageBox(m_hDlg, "xx", "xx", MB_OK);
        return FALSE;
    }

    BOOL onFluentUpdate(char * result)
    {
        addLines("======================================================\r\n");
        addLines(result);
        addLines("======================================================\r\n");
        return FALSE;
    }

    BOOL hookLoginWindow(DWORD targetPid = 0)
    {
        addLines("-->Try hooking UserID window.\r\n");
        HWND hUserId = NULL;
        if ((hUserId = ::FindWindow(DIALOG_WINDOWCLASS,
            USER_IDENTIFICATION_DIALOG_WINDOWTITLE)) != NULL )
        {

            DWORD pid = 0;

            DWORD tid = ::GetWindowThreadProcessId(hUserId, &pid);

            if (targetPid == 0 || pid == targetPid)
            {
                SetLoginWindow(hUserId);
                resetCBTHookHWND(hUserId);
                setCBTHook(hUserId, m_hDlg, tid);

                HWND hComboSystem = ::GetDlgItem(hUserId, ID_USERID_COMBOBOX_SYSTEM);
                if (hComboSystem)
                {
                    int sel = ::SendMessage(hComboSystem,	CB_GETCURSEL, 0, 0);
                    TString message;
                    message.format("CurSel : %d\r\n", sel);
                    addLines(message);

                    int textlen = SendMessage(hComboSystem, CB_GETLBTEXTLEN, sel, 0);
                    message.reserve(textlen+3);
                    textlen = SendMessage(hComboSystem, CB_GETLBTEXT, sel, (LPARAM)(LPSTR)message);
                    TString line;
                    line.format("System : %s\r\n", message);
                    addLines(line);
                }

                HWND hComboAuth = ::GetDlgItem(hUserId, ID_USERID_COMBOBOX_AUTHENTICATION);
                if (hComboAuth)
                {
                    int sel = ::SendMessage(hComboAuth,	CB_GETCURSEL, 0, 0);
                	TString message;
                	message.format("CurSel : %d\r\n", sel);
                    addLines(message);

                    int textlen = SendMessage(hComboAuth, CB_GETLBTEXTLEN, sel, 0);
                    message.reserve(textlen+3);
                    textlen = SendMessage(hComboAuth, CB_GETLBTEXT, sel, (LPARAM)(LPSTR)message);
                    TString line;
                    line.format("Auth : %s\r\n", message);
                    addLines(line);
                }

                HWND hEditUserName = GetDlgItem(hUserId, ID_USERID_EDIT_USERNAME);

                if (hEditUserName)
                {
                    char name[256] = {0};
                    SendMessage(hEditUserName, WM_GETTEXT, 255, (LPARAM)name);

                    LOG_STRING("User: %s", name);

                    TString msgLine;
                    msgLine.format("User: %s\r\n", name);
                    addLines(msgLine);
                }

                addLines("-->UserID window is hooked.\r\n");

                RECT rect = {0};
                ::GetWindowRect(hUserId, &rect);
                ::MoveWindow(hUserId, rect.left+10, rect.top, rect.right-rect.left, rect.bottom-rect.top, FALSE);
                ::MoveWindow(hUserId, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, FALSE);
                ::SetActiveWindow(m_hDlg);
                ::SetActiveWindow(hUserId);

                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL isLoginFailed()
    {
        HWND hLoginError = ::FindWindow(DIALOG_WINDOWCLASS, ERROR_WINDOWTITLE);

        if (hLoginError)
        {
            DWORD pid = 0;
            DWORD tid = ::GetWindowThreadProcessId(hLoginError, &pid);

            if (pid == dwPidBusObj)
            {
                addLines("Login failed\r\n");
                this->setTimer(IDTIMER_TRY_HOOKING_LOGINWINDOW, 100);
                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL hookMainWindow()
    {
        //TString str;
        //str.format("%d -- %d", LVN_FIRST, 0xFFFFFDEE);
        //showMsg(str);
        //return FALSE;

        addLines("Hooking Main Window\r\n");

        TString mainWindowTitlePrefix = "Desktop Intelligence - [Administrator - @VPDEV32]";

        if ((hWndDesktopIntelligence = ::FindWindow(NULL, mainWindowTitlePrefix)) != NULL)
        {
            //
            TString msg;
            msg.format("Find DesktopIntelligence : HWND=%d\r\n", hWndDesktopIntelligence);
            addLines(msg);
            SetMainWindow(hWndDesktopIntelligence);

            DWORD pid = 0;

            DWORD tid = ::GetWindowThreadProcessId(hWndDesktopIntelligence, &pid);

            HWND hWndClient = GetDlgItem(hWndDesktopIntelligence, 0x0000E81B);
            if (hWndClient) 
            {                  
                    addLines("Find Client Window\r\n");

                    HWND hMenuBarChild = GetDlgItem(hWndClient, 0x0000E800);

                    if (hMenuBarChild)
                    {
                        msg.format("Install hook for MenuBar : HWND=%d\r\n", hMenuBarChild);
                        addLines(msg);
                        //installCallWndProcHook(hMenuBarChild, m_hDlg);
                        //resetCBTHookHWND(hMenuBarChild);
                        //setCBTHook(hMenuBarChild, m_hDlg, tid);
                        SetMainWindowMenuBar(hMenuBarChild);
                        resetCBTHookHWND(hMenuBarChild);
                    }

                    HWND hToolBar = GetDlgItem(hWndClient, 0x0000E803);
                    if (hToolBar)
                    {
                        msg.format("Find Toolbar : HWND=%d\r\n", hToolBar);
                        addLines(msg);
                        SetMainWindowToolBar(hToolBar);
                    }


                    return TRUE;
            }       
        }

        return FALSE;
    }

    BOOL hookOpenWindow()
    {
        addLines("Try to hook Open Window\r\n");
        HWND hWndOpen = ::FindWindow(DIALOG_WINDOWCLASS, OPEN_WINDOWTITLE);

        if (hWndOpen)
        {
            addLines("Found Window Open\r\n");

            DWORD pid = 0;
            DWORD tid = ::GetWindowThreadProcessId(hWndOpen, &pid);

            if (pid == dwPidBusObj)
            {
                SetOpenWindow(hWndOpen);
                resetCBTHookHWND(hWndOpen);

                addLines("Open Window is hooked\r\n");
                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL hookImportWindow()
    {
        addLines("Try to hook Import Window\r\n");
        HWND hWndImport = ::FindWindow(DIALOG_WINDOWCLASS, IMPORT_WINDOWTITLE);

        if (hWndImport)
        {
            DWORD pid = 0;
            DWORD tid = ::GetWindowThreadProcessId(hWndImport, &pid);

            if (pid == dwPidBusObj)
            {
                SetOpenWindow(hWndImport);
                resetCBTHookHWND(hWndImport);

                addLines("Import Window is hooked\r\n");
                return TRUE;
            }
        }
        return FALSE;
    }

    BOOL FindOpenWindowAndExtractRepPath()
    {
        HWND hWndOpen = ::FindWindow(DIALOG_WINDOWCLASS, OPEN_WINDOWTITLE);

        if (hWndOpen)
        {
            addLines("Found Window Open\r\n");

            DWORD pid = 0;
            DWORD tid = ::GetWindowThreadProcessId(hWndOpen, &pid);

            //if (pid == dwPidBusObj)
            {
                HWND hCombobox = GetDlgItem(hWndOpen, 0x00000471);

                if (hCombobox)
                {
                    int count = ::SendMessage(hCombobox, CB_GETCOUNT, 0, 0);
                    int sel = ::SendMessage(hCombobox,	CB_GETCURSEL, 0, 0);
                    TString message;
                    message.format("CurSel : %d (%d)\r\n", sel, count);
                    addLines(message);

                    for (int i=0; i<count; ++i)
                    {
                        int textlen = SendMessage(hCombobox, CB_GETLBTEXTLEN, i, 0);
                        message.reserve(textlen+3);
                        textlen = SendMessage(hCombobox, CB_GETLBTEXT, i, (LPARAM)(LPSTR)message);

                        int data = ::SendMessage(hCombobox, CB_GETITEMDATA, i, 0);

                        TString line;
                        line.format("Dir : %d %s === %d\r\n", i, message, data);
                        addLines(line);
                    }
                }

                HWND hListViewParent = GetDlgItem(hWndOpen, 0x00000461);
                if (hListViewParent)
                {
                    HWND hListView = GetDlgItem(hListViewParent, 0x00000001);
                    if (hListView)
                    {
                        //LVITEMINDEX lvii = {-1, -1};
                        //int count = ::SendMessage(hListView, LVM_GETITEMCOUNT, 0, 0);
                        //int sel = ::SendMessage(hListView, LVM_GETNEXTITEMINDEX, (WPARAM)&lvii, LVNI_SELECTED);
                        //TString message;
                        //message.format("CurSel : %d (%d)\r\n", sel, count);
                        //addLines(message);
                    }
                }

                HWND hOpenEdit = GetDlgItem(hWndOpen, 0x00000480);
                if (hOpenEdit)
                {
                    addLines("Find filename Edit in open window\r\n");

                    //int len = GetWindowTextLength(hOpenEdit);

                    //len += 3;

                    TString buffer;
                    buffer.reserve(MAX_PATH);

                    int len = ::SendMessage(hOpenEdit, WM_GETTEXT, (WPARAM)MAX_PATH, (LPARAM)(LPTSTR)buffer);

                    TString line;
                    line.format("Filename : %d === %s\r\n", len, buffer);
                    addLines(line);
                }


                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL onBnSendCmd()
    {
        //DWORD dwProcessId;
        //HANDLE hProc;

        //HWND hWnd = NULL;
        //hWnd = ::FindWindow(DIALOG_WINDOWCLASS, "Desktop Intelligence");

        //if (hWnd == NULL)
        //{
        //    hWnd = ::FindWindow(DIALOG_WINDOWCLASS, FREEHANDSQL_WINDOW_TILTE);
        //}

        //if (hWnd)
        //{
        //    GetWindowThreadProcessId(hWnd, &dwProcessId);
        //    hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, dwProcessId);
        //    CreateDumpFile("rep.dmp", dwProcessId,
        //        MiniDumpNormal|MiniDumpWithThreadInfo|MiniDumpWithHandleData|MiniDumpWithIndirectlyReferencedMemory);
        //}

        //HWND hWndConnectionInfo = NULL;
        //if ((hWndConnectionInfo = ::FindWindow(DIALOG_WINDOWCLASS, "Edit some-other-mssql-free connection")) != NULL)
        //{
        //    UINT ID_CANCEL_BUTTON = 0x0000002;
        //    //
        //   //::MoveWindow(hWndConnectionInfo, 0, 0, 0, 0, FALSE);
        //    UINT ID_EDIT_PASS = 0x00000CD;

        //    char szBuf[200];
        //   
        //    DWORD dwProcessId;
        //    
        //    LPVOID lpRemoteBuffer;
        //    DWORD lpNumberOfBytesRead;
        //    HANDLE hProc;

        //    GetWindowThreadProcessId(hWndConnectionInfo, &dwProcessId);
        //    hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, dwProcessId);

        //    //Allocate RemoteBuffer
        //    lpRemoteBuffer = VirtualAllocEx(hProc, NULL, sizeof(szBuf), MEM_COMMIT, PAGE_READWRITE);
        //    int len = GetWindowText(GetDlgItem(hWndConnectionInfo, ID_EDIT_PASS), (LPSTR)lpRemoteBuffer, sizeof(szBuf)); 
        //    ReadProcessMemory(hProc, lpRemoteBuffer, szBuf, sizeof(szBuf ), &lpNumberOfBytesRead);
        //    VirtualFree(lpRemoteBuffer, sizeof(szBuf), PAGE_READWRITE);
        //    if(0<len)
        //    {
        //        showMsg(szBuf);
        //    }
        //}
        //dumpBusObj();

        return FindOpenWindowAndExtractRepPath();
    }

    void dumpProcessByPid(DWORD pid)
    {
        if (pid)
        {
            //HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
            CreateDumpFile("rep.dmp", pid, 
                MiniDumpNormal|MiniDumpWithThreadInfo|MiniDumpWithHandleData|MiniDumpWithIndirectlyReferencedMemory);
        }
    }

    void dumpBusObj()
    {
        if (dwPidBusObj)
        {
            HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, dwPidBusObj);
            CreateDumpFile("rep.dmp", dwPidBusObj, 
                MiniDumpNormal|MiniDumpWithThreadInfo|MiniDumpWithHandleData|MiniDumpWithIndirectlyReferencedMemory);
        }

    }

    bool IsImportSuccessful()
    {
        //
        const char * ImportResultsWindowTitle = "Import results";
        const UINT ID_IMPORTRESULTS_LABEL = 0x000003F0;

        HWND hWndImportResults = ::FindWindow(DIALOG_WINDOWCLASS, ImportResultsWindowTitle);
        if (hWndImportResults)
        {
            addLines("Find Import Results\r\n");
            HWND hLabelResult = GetDlgItem(hWndImportResults, ID_IMPORTRESULTS_LABEL);
            if (IsWindowVisible(hLabelResult))
            {
                addLines("Import Successfully\r\n");
            }
        }
    }

    void FindCUIDInDumpFile(const char * filePath)
    {
        // universe-listing (1)
        // 08&I=<AXGnBxuIs4FAgPVr7b43_nM>,
        // 83&c8=AXGnBxuIs4FAgPVr7b43_nM,
        // 08&I=AXGnBxuIs4FAgPVr7b43_nM,
    }
};

UDLGAPP_T(UDialogBusObj, IDD_FLUENT);
