#ifndef U_XML_H
#define U_XML_H

#include "ustatic.h"

class UXmlControl : public UStatic
{
    enum {
        ID_TIMER_UPDATE = 1111
    };
public:
    UXmlControl(HWND hParent, UINT nID, HINSTANCE hInst);
    UXmlControl();
    ~UXmlControl();
    UXmlControl(UBaseWindow *pWndParent, UINT nID = -1);
    
    //
    BOOL create();

    //
    BOOL onPaint();
    
    void Start();

	//
    void Pause();
    
private:


};

#endif // U_XML_H
