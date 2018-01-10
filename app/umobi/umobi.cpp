#include <windows.h>
#include <tchar.h>
#include <commctrl.h>

#include "umobi.h"

UMobi::UMobi(HWND hParent, UINT nID, HINSTANCE hInst)
: UStatic(hParent, nID, hInst)
{
}

UMobi::UMobi()
{
}

UMobi::~UMobi()
{}

UMobi::UMobi(UBaseWindow *pWndParent, UINT nID)
: UStatic(pWndParent, nID)
{
    m_dwStyles &= ~SS_SIMPLE;
    m_dwStyles |= SS_NOTIFY;
    m_nResourceID = nID;
}
