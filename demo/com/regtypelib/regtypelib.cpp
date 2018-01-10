#include <Windows.h>
#include <stdio.h>
#include <ctype.h>

void ShowErrorMessage();
void ShowUsage();

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        ShowErrorMessage();
        return 1;
    }

    if(strlen(argv[1]) != 2)
    {
        ShowErrorMessage();
        return 1;
    }

    if(argv[1][0] != '/' && argv[1][0] != '-')
    {
        ShowErrorMessage();
        return 1;
    }

    if(argv[1][1] == '?')
    {
        ShowUsage();
        return 0;
    }

    if(argc < 3)
    {
        ShowErrorMessage();
        return 1;
    }

    if(tolower(argv[1][1]) == 'r')
    {
        HANDLE hFile = CreateFile(argv[2], 0, 0, NULL, OPEN_EXISTING, 0, NULL);

        if(hFile == INVALID_HANDLE_VALUE)
        {
            printf("The file %s does not exists!\r\n", argv[2]);
            return 1;
        }

        CloseHandle(hFile);

        HRESULT hr = S_OK;
        ITypeLib *pTypeLib;
		
		wchar_t  ws[256];
		swprintf(ws, L"%hs", argv[2]);

        
        hr = LoadTypeLibEx(ws, REGKIND_REGISTER, &pTypeLib);

        if(FAILED(hr))
        {
            printf("Register TypeLib failed. Error Code: %lu", hr);
            return 1;
        }
        else
        {
            printf("Register TypeLib succeed.");
            pTypeLib->Release();
        }
    }
    else if(tolower(argv[1][1]) == 'u')
    {
        HRESULT hr = S_OK;
        GUID guid;

		wchar_t  ws[256];
		swprintf(ws, L"%hs", argv[2]);
        hr = CLSIDFromString(ws, &guid);

        if(FAILED(hr))
        {
           printf("Can't convert %s to GUID. Error Code: %lu", argv[2], hr);
        }
        else
        {
            hr = UnRegisterTypeLib(guid, 1, 0, LANG_NEUTRAL, SYS_WIN32);

            if(FAILED(hr))
            {
                printf("Can't unregister the TypeLib. Error Code: %lu", hr);
                return 1;
            }
            else
            {
                printf("Unregister TypeLib %s successfully.", argv[2]);
            }
        }
    }
    else
    {
        ShowErrorMessage();
        return 1;
    }

    return 0;
}

void ShowErrorMessage()
{
    printf("The syntax of the command is incorrect.\r\n\r\n");

    ShowUsage();
}

void ShowUsage()
{
    printf("RegTypeLib v1.0 (c) Zhenxing Zhou\r\n\r\n");
    printf("A tool to register TypeLib or unregister TypeLib\r\n");
    printf("Using RegTypeLib.exe /? to show help\r\n");
    printf("Using RegTypeLib.exe /r <TypeLib file path> to register TypeLib\r\n");
    printf("Using RegTypeLib.exe /u <TypeLib guid> to unregister TypeLib\r\n");
    printf("Example:\r\n");
    printf("\tRegTypeLib.exe /r D:\\a.tlb\r\n");
    printf("\tRegTypeLib.exe /u {bb6df56e-cace-11dc-9992-0019b93a3a84}\r\n");
}
