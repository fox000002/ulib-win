#include "resource.h"

#include <windows.h>
#include <tchar.h>
#include <commctrl.h>

#include "udialogx.h"
#include "udlgapp.h"
#include "uicon.h"
#include "ubutton.h"
#include "utime.h"
#include "ustatic.h"
#include "umsg.h"

#include "adt/uautoptr.h"
#include "adt/ustring.h"

using huys::UDialogBox;

// GetLastWriteTime - Retrieves the last-write time and converts
//                    the time to a string
//
// Return value - TRUE if successful, FALSE otherwise
// hFile      - Valid file handle
// lpszString - Pointer to buffer to receive string

BOOL GetLastWriteTime(HANDLE hFile, LPTSTR lpszString, DWORD dwSize)
{
    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;
    DWORD dwRet;

    // Retrieve the file times for the file.
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
        return FALSE;

    // Convert the last-write time to local time.
    FileTimeToSystemTime(&ftWrite, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    // Build a string showing the date and time.
    dwRet = wsprintf(lpszString,
        TEXT("%02d/%02d/%d  %02d:%02d"),
        stLocal.wMonth, stLocal.wDay, stLocal.wYear,
        stLocal.wHour, stLocal.wMinute);

    if( S_OK == dwRet )
        return TRUE;
    else return FALSE;
}

int MyGetFileTime(LPTSTR fn)
{
	HANDLE hFile;
    TCHAR szBuf[MAX_PATH];
	
    hFile = CreateFile(fn, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
        showMsgFormat(_T("Info"), _T("CreateFile failed with %d\n"), GetLastError());
        return 0;
    }
    if(GetLastWriteTime( hFile, szBuf, MAX_PATH ))
        showMsgFormat(_T("Info"), TEXT("Last write time is: %s\n"), szBuf);
        
    CloseHandle(hFile); 
	
	return 0;
}


class UDialogExt : public UDialogBox
{
    enum {
        ID_TIMER_UPDATE = 1021
    };
public:
    UDialogExt(HINSTANCE hInst, UINT nID)
        : UDialogBox(hInst, nID)
    {}

    virtual BOOL onInit()
    {
        UTime time;
        time.getSysTime();

        huys::ADT::UStringAnsi text;
        text.format("SysTime: %d-%d-%d %d:%d:%d",
                    time.year(), time.month(), time.day(),
                    time.hour(), time.minute(), time.second());

        m_label = new UStatic(m_hDlg, -1, m_hInst);
        m_label->setPos(100, 100, 200, 100);
        m_label->setText(text);
        m_label->create();

        setTimer(ID_TIMER_UPDATE, 1000);

        return TRUE;
    }

    virtual BOOL onCommand(WPARAM wParam, LPARAM lParam)
    {
        switch (LOWORD(wParam))
        {
        case 0:
        default:
            return UDialogBox::onCommand(wParam, lParam);
        }
    }

    BOOL onClose()
    {
        killTimer(ID_TIMER_UPDATE);
        return UDialogBox::onClose();
    }

    BOOL onTimer(WPARAM wParam, LPARAM lParam)
    {
        if (ID_TIMER_UPDATE == wParam)
        {
            UTime time;
            time.getSysTime();

            huys::ADT::UStringAnsi text;
            text.format("SysTime: %d-%d-%d %d:%d:%d",
                    time.year(), time.month(), time.day(),
                    time.hour(), time.minute(), time.second());
            m_label->setWindowText(text);
        }
        return FALSE;
    }

private:
    huys::ADT::UAutoPtr<UStatic> m_label;
};

UDLGAPP_T(UDialogExt, IDD_TEST);

