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
    // ���� SHBrowseForFolder ȡ��Ŀ¼(�ļ���)����
    // ���� hWnd: �����ھ��
    // ���� lpTitle: ���ڱ���
    
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
        if ( SUCCEEDED ( ::SHGetMalloc( &pMalloc ) ) )  // ȡ��IMalloc�������ӿ�
        {
            pMalloc->Free( pidl );    // �ͷ��ڴ�
            pMalloc->Release();       // �ͷŽӿ�
        }
    }
    //return szPath;
}

void FileOperation(HWND hWnd, LPCSTR src, LPCSTR dest, LPCSTR title)
{
	SHFILEOPSTRUCT FileOp; //��ǵ��ļ������ṹ
	FileOp.hwnd = hWnd; //���þ��
	//���ò�����ʽ��������FO_COPY��ɾ���� FO_DELETE
	FileOp.wFunc = FO_COPY;
	FileOp.pFrom = src; //Դ�ļ�·��
	FileOp.pTo = dest; //Ŀ���ļ�·��
	FileOp.fFlags = FOF_ALLOWUNDO; //����ָ�
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = title; //���ñ���
	SHFileOperation(&FileOp); //ִ����ǿ���
	if(FileOp.fAnyOperationsAborted) //���������ֹ
		UShell::msgbox(NULL, "Info", "An Operation was aborted!!!\n");
}

class UMyWindow : public UBaseWindow
{
    enum {
        ID_BN_EXEC   = 1111,
		ID_BN_FOLDER = 1112
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
		
		_bnFolder = new UButton(this, ID_BN_FOLDER);
        _bnFolder->setPos(100, 200, 100, 50);
        _bnFolder->create();
        _bnFolder->setWindowText(_T("folder"));

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
		case ID_BN_FOLDER:
			return onBnFolder();
        default:
            return UBaseWindow::onCommand(wParam, lParam);
        }
    }
private:
    AUI::UButtonP _bnExec;
	AUI::UButtonP _bnFolder;
private:
    BOOL onBnExec()
    {
        UShell::execute(NULL, "notepad");
        return FALSE;
    }
	
	BOOL onBnFolder()
	{
		BrowseFolder(*this, "Select Folder");
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
