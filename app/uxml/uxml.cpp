#include <windows.h>
#include <tchar.h>
#include <commctrl.h>

#include "udc.h"

#include "uxml.h"

UXmlControl::UXmlControl(HWND hParent, UINT nID, HINSTANCE hInst)
: UStatic(hParent, nID, hInst)
{

}

UXmlControl::UXmlControl()
{}

UXmlControl::~UXmlControl()
{}

UXmlControl::UXmlControl(UBaseWindow *pWndParent, UINT nID)
: UStatic(pWndParent, nID)
{
    m_dwStyles &= ~SS_SIMPLE;
    m_dwStyles |= SS_NOTIFY;
    m_nResourceID = nID;
}

BOOL UXmlControl::create()
{
    return UStatic::create() && this->subclassProc();
}

BOOL UXmlControl::onPaint()
{
    UPaintDC dc(*this);

    RECT rc;
    this->getClientRect(&rc);
 
    return FALSE;
}


void UXmlControl::Start()
{
    this->setTimer(ID_TIMER_UPDATE, 300);
}

void UXmlControl::Pause()
{
}
