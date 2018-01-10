#include "resource.h"

#include <windows.h>
#include <tchar.h>
#include <ocidl.h>
#include <olectl.h>

#define   INITGUID
#include <initguid.h>
#define CINTERFACE

//#include "InfTypeLib.h"

#include "ubasewindow.h"
#include "uwinapp.h"
#include "ubutton.h"
#include "adt/uautoptr.h"


class UCatiaWindow : public UBaseWindow
{
    enum {
        ID_BN_TEST = 10001
    };
public:
    UCatiaWindow()
    : UBaseWindow(NULL, ::GetModuleHandle(NULL))
    {
        this->setTitle(_T("Catia Test 0.0.1"));
        this->setPos(100, 100, 800, 600);

        ::CoInitialize(NULL);
    }

    ~UCatiaWindow()
    {
        ::CoUninitialize();
    }

    BOOL onCreate()
    {
        this->setIconBig(IDI_APP);


        m_bnTest = new UButton(this, ID_BN_TEST);
        m_bnTest->setPos(100, 100, 100, 100);
        m_bnTest->create();
        m_bnTest->setWindowText(_T("test"));

        return UBaseWindow::onCreate();
    }


    /* virtual */ BOOL onChar(WPARAM wParam, LPARAM lParam)
    {
        switch (wParam)
        {
        case VK_ESCAPE:
            return UBaseWindow::onClose();
        default:
            return UBaseWindow::onChar(wParam, lParam);
        }
    }

 

    BOOL onCommand(WPARAM wParam, LPARAM lParam)
    {
        switch (LOWORD(wParam))
        {
        case ID_BN_TEST:
            return onBnTest();
        default:
            return UBaseWindow::onCommand(wParam, lParam);
        }
    }
private:
   

    huys::ADT::UAutoPtr<UButton> m_bnTest;

    BOOL onBnTest()
    {
        ;
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpszCmdLine, int nCmdShow)
{
    UWinApp app;
    app.setMainWindow(new UCatiaWindow);
    app.init(hInstance);
    return app.run();
}
