#include <windows.h>
#include <stdio.h>
#include <shldisp.h>

#ifdef _MSC_VER
#pragma comment(lib, "LZ32")
#endif 

bool Unzip2Folder( BSTR lpZipFile, BSTR lpFolder)
{
    IShellDispatch *pISD;

    Folder  *pZippedFile = 0L;
    Folder  *pDestination = 0L;

    long FilesCount = 0;
    IDispatch* pItem = 0L;
    FolderItems *pFilesInside = 0L;

    VARIANT Options, OutFolder, InZipFile, Item;
    CoInitialize( NULL);
    //__try
    //{
        if (CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pISD) != S_OK)
            return 1;

        InZipFile.vt = VT_BSTR;
        InZipFile.bstrVal = lpZipFile;
        pISD->NameSpace( InZipFile, &pZippedFile);
        if (!pZippedFile)
        {
            pISD->Release();
            return 1;
        }

        OutFolder.vt = VT_BSTR;
        OutFolder.bstrVal = lpFolder;
        pISD->NameSpace( OutFolder, &pDestination);
        if(!pDestination)
        {
            pZippedFile->Release();
            pISD->Release();
            return 1;
        }

        pZippedFile->Items(&pFilesInside);
        if(!pFilesInside)
        {
            pDestination->Release();
            pZippedFile->Release();
            pISD->Release();
            return 1;
        }

        pFilesInside->get_Count( &FilesCount);
        if( FilesCount < 1)
        {
            pFilesInside->Release();
            pDestination->Release();
            pZippedFile->Release();
            pISD->Release();
            return 0;
        }

        pFilesInside->QueryInterface(IID_IDispatch,(void**)&pItem);

        Item.vt = VT_DISPATCH;
        Item.pdispVal = pItem;

        Options.vt = VT_I4;
        Options.lVal = 1024 | 512 | 16 | 4;//http://msdn.microsoft.com/en-us/library/bb787866(VS.85).aspx

        bool retval = pDestination->CopyHere( Item, Options) == S_OK;

        pItem->Release();pItem = 0L;
        pFilesInside->Release();pFilesInside = 0L;
        pDestination->Release();pDestination = 0L;
        pZippedFile->Release();pZippedFile = 0L;
        pISD->Release();pISD = 0L;

        //return retval;

    //}
    //__finally    
    //{
        CoUninitialize();
    //}

        return retval;
}

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        printf("Usage: test_zip <file name>\n");
        return -1;
    }
    
    LPSTR filename = argv[1];

    OFSTRUCT ost = {0};

    INT hFile =  LZOpenFile(filename, &ost, OF_READ);;
    
    if (hFile)
    {
        printf("LZOpenFile() for reading compressed file looks OK!\n");
    }
    else
    {  // Can't use GetLastError() lol
        printf("%d, LZOpenFile() for reading compressed file failed miserably!\n", hFile);
    }
    
    LZClose(hFile);

    BSTR ziparch = SysAllocString((const OLECHAR *)L"E:\\Working\\ulib-win\\demo\\zip\\test_zip.zip");//alloc memory
    BSTR dstfld = SysAllocString((const OLECHAR *)L"E:\\Working\\ulib-win\\demo\\zip\\uncompressed");//alloc memory

    Unzip2Folder(ziparch, dstfld);

    SysFreeString(ziparch);//free memory
    SysFreeString(dstfld);//free memory


    return 0;
}
