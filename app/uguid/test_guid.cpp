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
        ID_LISTCTRL  = 11002,
        ID_ED_NAME   = 11003,
        ID_CB_SEX    = 11004,
        ID_ED_ADDR   = 11005,
        ID_BN_ADD    = 11006,
		ID_RB_START  = 11007
    };
public:
   UMyWindow()
   : UBaseWindow(NULL, GetModuleHandle(NULL))
   {
        this->setTitle(_T("GUID Utility 0.0.1"));
		this->setExStyles(WS_EX_TOOLWINDOW);
		this->setPos(0, 0, 400, 400);
   }

   BOOL onCreate()
   {
//         us[0] = new AUI::UTransLabel(this, 1333);
//         us[0]->setPos(20, 20, 50, 20);
//         us[0]->create();
//         us[0]->setWindowText("Name:");
// 
//        m_pEdtName = new UEdit(this, ID_ED_NAME);
//        m_pEdtName->setPos(80, 20, 100, 20);
//        m_pEdtName->create();
// 
//        us[1] = new AUI::UTransLabel(this, 2333);
//        us[1]->setPos(200, 20, 30, 25);
//        us[1]->create();
//        us[1]->setWindowText("Sex:");
// 
//        m_pCboSex = new UComboBox(this, ID_CB_SEX);
//        m_pCboSex->setStyles(CBS_DROPDOWN);
//        m_pCboSex->setPos(240, 20, 100, 20);
//        m_pCboSex->create();
//        m_pCboSex->addText(_T("F"));
//        m_pCboSex->addText(_T("M"));
// 
//        us[2] = new AUI::UTransLabel(this, 3333);
//        us[2]->setPos(360, 20, 60, 25);
//        us[2]->create();
//        us[2]->setWindowText("Address:");
// 
//        m_pEdtAddr= new UEdit(this, ID_ED_ADDR);
//        m_pEdtAddr->setPos(440, 20, 200, 30);
//        m_pEdtAddr->create();

//        m_pContactList = new UListView(this, ID_LISTCTRL);
//        m_pContactList->setPos(18, 60, 622, 300);
//        m_pContactList->setStyles(WS_BORDER | LVS_REPORT);
//        m_pContactList->create();
// 
//        m_pContactList->addColTextWidth(0, "name", 150);
//        m_pContactList->addColTextWidth(1, "sex", 50);
//        m_pContactList->addColTextWidth(2, "address", 300);
// 
//        m_pContactList->addItemTextImage(0, "fox", 0);
//        m_pContactList->setItemText(0, 1, _T("f"));
//        m_pContactList->setItemText(0, 2, _T("Another world"));

       m_pBack = new UBitmap(IDB_BACKGROUND, getInstance());

//        m_bnAdd = new UButton(this, ID_BN_ADD);
//        m_bnAdd->setPos(660, 20, 100, 50);
//        m_bnAdd->create();
//        m_bnAdd->setWindowText(_T("ADD"));

	   m_gboxFormat = new AUI::UTransGroupBox(this);
	   m_gboxFormat->setPos(10, 20, 340, 220);
	   m_gboxFormat->setWindowText(_T("Format"));
	   m_gboxFormat->create();

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
		   m_radioButtons[i]->create();
		   m_radioButtons[i]->setWindowText(RadioButtonText[i]);
	   }
	   
		m_radioButtons[0]->check();

       return UBaseWindow::onCreate();
   }
   
//    virtual BOOL onEraseBkgnd(HDC hdc)
//     {
//         RECT rc;
//         this->getClientRect(&rc);
//         m_pBack->showStretch(hdc, rc);
//         return TRUE;
//     }


    BOOL onCommand(WPARAM wParam, LPARAM lParam)
    {
        switch (LOWORD(wParam))
        {
        case ID_BN_ADD:
            return onBnAdd();
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
    huys::ADT::UAutoPtr<UButton> m_bnAdd;

	//
	AUI::UTransRadioButtonP m_radioButtons[4];
	AUI::UTransGroupBoxP m_gboxFormat;

private:
    BOOL onBnAdd()
    {
        TCHAR name[256];
        TCHAR sex[5];
        TCHAR addr[256];

        m_pEdtName->getText(name);
        m_pCboSex->getWindowText(sex, 256);
        m_pEdtAddr->getText(addr);

        if (lstrcmp(name, "")!=0 && lstrcmp(sex, "")!=0 && lstrcmp(addr, "")!=0)
            addRecord(name, sex, addr);

        return FALSE;
    }

    void addRecord(LPCTSTR name, LPCTSTR sex, LPCTSTR addr)
    {
        int i = m_pContactList->getItemCount();
       m_pContactList->addItemTextImage(i, name, 0);
       m_pContactList->setItemText(i, 1, sex);
       m_pContactList->setItemText(i, 2, addr);
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpszCmdLine, int nCmdShow)
{
    UWinApp app;
    app.setMainWindow(new UMyWindow);
    app.init(hInstance);
    return app.run();
}

