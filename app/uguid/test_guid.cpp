#include "resource.h"

#include <windows.h>
#include <tchar.h>
#include <commctrl.h>

#include "uwinapp.h"
#include "ubasewindow.h"
#include "ustatic.h"
#include "ulistview.h"
#include "ubitmap.h"
#include "uedit.h"
#include "ucombobox.h"
#include "ubutton.h"
#include "adt/uautoptr.h"
#include "aui/aui_label.h"
#include "aui/aui_button.h"

class UMyWindow : public UBaseWindow
{
    enum {
        ID_BN_GENERATE     = 11006,
        ID_RB_START        = 11007,
        ID_LB_RESULT       = 11008
    };
public:
   UMyWindow()
   : UBaseWindow(NULL, GetModuleHandle(NULL))
   {
        this->setTitle(_T("GUID Utility 0.0.1"));
        this->setExStyles(WS_EX_TOOLWINDOW);
        this->setPos(0, 0, 400, 500);
   }

   BOOL onCreate()
   {
       m_pBack = new UBitmap(IDB_BACKGROUND, getInstance());


       m_bnGenerate = new UButton(this, ID_BN_GENERATE);
       m_bnGenerate->setPos(40, 260, 80, 40);
       m_bnGenerate->create();
       m_bnGenerate->setWindowText(_T("Generate"));

       m_bnCopy = new UButton(this, ID_BN_GENERATE);
       m_bnCopy->setPos(140, 260, 80, 40);
       m_bnCopy->create();
       m_bnCopy->setWindowText(_T("Copy"));

       m_gboxFormat = new AUI::UTransGroupBox(this);
       m_gboxFormat->setPos(10, 20, 370, 220);
       m_gboxFormat->setWindowText(_T("Format"));
       m_gboxFormat->create();

       m_gboxResult = new AUI::UTransGroupBox(this);
       m_gboxResult->setPos(10, 320, 370, 120);
       m_gboxResult->setWindowText(_T("Result"));
       m_gboxResult->create();

       const TCHAR *RadioButtonText[] = {
           _T("1. IMPLEMENT_OLECREATE(...)"),
           _T("2. DEFINE_GUID(...)"),
           _T("3. static const struct GUID = {...}"),
           _T("4. Registry Format (ie. {xxxxxxx-xxxx..-xxx})")
       };

       for (int i=0; i<4; i++)
       {
           m_radioButtons[i] = new AUI::UTransRadioButton(this, ID_RB_START+i);
           m_radioButtons[i]->setPos(20, 50+40*i, 320, 20);
           m_hbr[i] =  CreatePatternBrushFromFile(m_pBack->clipImage(20, 50+40*i, 320, 20));
           m_radioButtons[i]->SetHBrush(m_hbr[i]);
           m_radioButtons[i]->create();
           m_radioButtons[i]->setWindowText(RadioButtonText[i]);
       }

       m_radioButtons[0]->check();

       m_labelResult = new AUI::UTransLabel(this, ID_LB_RESULT);
       m_labelResult->setPos(20, 330, 350, 100);
       m_labelResult->create();
       m_labelResult->setWindowText("Result:");

       return UBaseWindow::onCreate();
   }

   virtual BOOL onEraseBkgnd(HDC hdc)
    {
        RECT rc;
        this->getClientRect(&rc);
        m_pBack->showStretch(hdc, rc);
        return TRUE;
    }

    virtual BOOL onCommand(WPARAM wParam, LPARAM lParam)
    {
        switch (LOWORD(wParam))
        {
        case IDM_EXIT:
            return UBaseWindow::onClose();
        default:
            return UBaseWindow::onCommand(wParam, lParam);
        }
    }

private:
    huys::ADT::UAutoPtr<UListView> m_pContactList;
    huys::ADT::UAutoPtr<UBitmap> m_pBack;
    huys::ADT::UAutoPtr<UEdit> m_pEdtName;
    huys::ADT::UAutoPtr<UComboBox> m_pCboSex;
    huys::ADT::UAutoPtr<UEdit> m_pEdtAddr;
    //
    AUI::UTransLabelP us[3];
    //
    huys::ADT::UAutoPtr<UButton> m_bnGenerate;
    huys::ADT::UAutoPtr<UButton> m_bnCopy;
    //
    AUI::UTransRadioButtonP m_radioButtons[4];
    AUI::UTransGroupBoxP m_gboxFormat;
    AUI::UTransGroupBoxP m_gboxResult;
    AUI::UTransLabelP m_labelResult;

    HBRUSH m_hbr[4];

private:
    HBRUSH CreatePatternBrushFromFile(HBITMAP hbm)
    {
        HBRUSH hbr = NULL;
        if (hbm) {
            hbr = CreatePatternBrush(hbm);
        }
        return hbr;
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpszCmdLine, int nCmdShow)
{
    UWinApp app;
    app.setMainWindow(new UMyWindow);
    app.init(hInstance);
    return app.run();
}

