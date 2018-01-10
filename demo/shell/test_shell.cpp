#include "resource.h"
#include <windows.h>
#include <tchar.h>
#include <shlobj.h>

#include "uwinapp.h"
#include "ubasewindow.h"

#include "aui/aui.h"

#include "ushell.h"

void BrowseFolder(HWND hWnd, LPCTSTR lpTitle)
{
    // 调用 SHBrowseForFolder 取得目录(文件夹)名称
    // 参数 hWnd: 父窗口句柄
    // 参数 lpTitle: 窗口标题
    
    char szPath[MAX_PATH]={0};
    BROWSEINFO m_bi;

    m_bi.ulFlags = BIF_RETURNONLYFSDIRS  | BIF_STATUSTEXT;
    m_bi.hwndOwner = hWnd;
    m_bi.pidlRoot = NULL;
    m_bi.lpszTitle = lpTitle;
    m_bi.lpfn = NULL;
    m_bi.lParam = NULL;
    m_bi.pszDisplayName = szPath;

    LPITEMIDLIST pidl = ::SHBrowseForFolder( &m_bi );
    if ( pidl )
    {
        if( !::SHGetPathFromIDList ( pidl, szPath ) )  szPath[0]=0;

        IMalloc * pMalloc = NULL;
        if ( SUCCEEDED ( ::SHGetMalloc( &pMalloc ) ) )  // 取得IMalloc分配器接口
        {
            pMalloc->Free( pidl );    // 释放内存
            pMalloc->Release();       // 释放接口
        }
    }
    //return szPath;
}

class UMyWindow : public UBaseWindow
{
    enum {
        ID_BN_EXEC = 1111
    };
public:
    UMyWindow()
        : UBaseWindow(NULL, GetModuleHandle(NULL))
    {
        this->setTitle(_T("UShell Demo"));
        this->setPos(0, 0, 800, 600);
    }

    /* virtual */  BOOL onPreRegisterWindowClass(huys::UWindowClass &uwc)
    {
        uwc.setBKBrush((HBRUSH)::GetStockObject(GRAY_BRUSH));
        return TRUE;
    }

    BOOL onCreate()
    {
        this->setIconBig(IDI_HELP);

        _bnExec = new UButton(this, ID_BN_EXEC);
        _bnExec->setPos(100, 100, 100, 50);
        _bnExec->create();
        _bnExec->setWindowText(_T("notepad"));

        return UBaseWindow::onCreate();
    }

    virtual BOOL onChar(WPARAM wParam, LPARAM lParam)
    {
        switch (wParam)
        {
        case VK_ESCAPE:
            return UBaseWindow::onClose();
        default:
            return UBaseWindow::onChar(wParam, lParam);
        }
    }

    /* virtual */ BOOL onCommand(WPARAM wParam, LPARAM lParam)
    {
        switch (LOWORD(wParam))
        {
        case ID_BN_EXEC:
            return onBnExec();
        default:
            return UBaseWindow::onCommand(wParam, lParam);
        }
    }
private:
    AUI::UButtonP _bnExec;

private:
    BOOL onBnExec()
    {
        UShell::execute(NULL, "notepad");
		
		BrowseFolder(*this, "Select");
        return FALSE;
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpszCmdLine, int nCmdShow)
{
    UWinApp app;
    app.setMainWindow(new UMyWindow);
    app.init(hInstance);
    return app.run();
}
