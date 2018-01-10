#ifndef U_MOBI_H
#define U_MOBI_H

#include "ustatic.h"

class UMobi : public UStatic
{
public:
    UMobi(HWND hParent, UINT nID, HINSTANCE hInst);
    UMobi();
    ~UMobi();

    UMobi(UBaseWindow *pWndParent, UINT nID = -1);
private:

};

#endif // U_MOBI_H
