#ifndef UNICODE
#define UNICODE
#define _UNICODE
#endif
#pragma comment(lib, "mpr.lib")

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <winnetwk.h>

BOOL WINAPI EnumerateFunc(LPNETRESOURCE lpnr);
void DisplayStruct(int i, LPNETRESOURCE lpnrLocal);

int main(int argc, char *argv[])
{
    DWORD dwRetVal;

    WCHAR UserName[MAX_PATH];

    DWORD dwNameLength = MAX_PATH;

    if (argc != 2) {
        printf("Usage: %s [Redirected-LocalDevice or Network-Resource-Remote-name\n", argv[0]);
        exit(1);
    }

    printf("Calling WNetGetUser with Network-Resource = %s\n", argv[1]);

    dwRetVal = WNetGetUser((WCHAR *)argv[1], UserName, &dwNameLength);
    //
    // If the call succeeds, print the user information.
    //
    if (dwRetVal == NO_ERROR) {

        wprintf(L"WNetGetUser returned success\n");
        wprintf(L"\tUsername=%s   NameLength=%d\n", &UserName, dwNameLength);
        exit(0);
    }

    else {
        wprintf(L"WNetGetUser failed with error: %u\n", dwRetVal);
        exit(1);
    }

    DWORD dwResult; 
    //
    // Call the WNetConnectionDialog function.
    //
    dwResult = WNetConnectionDialog(HWND_DESKTOP, RESOURCETYPE_DISK);
    //
    // Call an application-defined error handler 
    //  to process errors.
    //
    if(dwResult != NO_ERROR) 
    {
        //NetErrorHandler(hwnd, dwResult, (LPSTR)"WNetConnectionDialog"); 
        return FALSE; 
    }


    LPNETRESOURCE lpnr = NULL;

    if (EnumerateFunc(lpnr) == FALSE) {
        printf("Call to EnumerateFunc failed\n");
        return 1;
    } else
    return 0;

}

BOOL WINAPI EnumerateFunc(LPNETRESOURCE lpnr)
{
    DWORD dwResult, dwResultEnum;
    HANDLE hEnum;
    DWORD cbBuffer = 16384;     // 16K is a good size
    DWORD cEntries = -1;        // enumerate all possible entries
    LPNETRESOURCE lpnrLocal;    // pointer to enumerated structures
    DWORD i;
    //
    // Call the WNetOpenEnum function to begin the enumeration.
    //
    dwResult = WNetOpenEnum(RESOURCE_GLOBALNET, // all network resources
    RESOURCETYPE_ANY,   // all resources
    0,  // enumerate all resources
    lpnr,       // NULL first time the function is called
    &hEnum);    // handle to the resource

    if (dwResult != NO_ERROR) {
        printf("WnetOpenEnum failed with error %d\n", dwResult);
        return FALSE;
    }
    //
    // Call the GlobalAlloc function to allocate resources.
    //
    lpnrLocal = (LPNETRESOURCE) GlobalAlloc(GPTR, cbBuffer);
    if (lpnrLocal == NULL) {
        printf("WnetOpenEnum failed with error %d\n", dwResult);
        //      NetErrorHandler(hwnd, dwResult, (LPSTR)"WNetOpenEnum");
        return FALSE;
    }

    do {
        //
        // Initialize the buffer.
        //
        ZeroMemory(lpnrLocal, cbBuffer);
        //
        // Call the WNetEnumResource function to continue
        //  the enumeration.
        //
        dwResultEnum = WNetEnumResource(hEnum,  // resource handle
        &cEntries,      // defined locally as -1
        lpnrLocal,      // LPNETRESOURCE
        &cbBuffer);     // buffer size
        //
        // If the call succeeds, loop through the structures.
        //
        if (dwResultEnum == NO_ERROR) {
            for (i = 0; i < cEntries; i++) {
                // Call an application-defined function to
                //  display the contents of the NETRESOURCE structures.
                //
                DisplayStruct(i, &lpnrLocal[i]);

                // If the NETRESOURCE structure represents a container resource, 
                //  call the EnumerateFunc function recursively.

                if (RESOURCEUSAGE_CONTAINER == (lpnrLocal[i].dwUsage
                            & RESOURCEUSAGE_CONTAINER))
                //          if(!EnumerateFunc(hwnd, hdc, &lpnrLocal[i]))
                if (!EnumerateFunc(&lpnrLocal[i]))
                printf("EnumerateFunc returned FALSE\n");
                //            TextOut(hdc, 10, 10, "EnumerateFunc returned FALSE.", 29);
            }
        }
        // Process errors.
        //
        else if (dwResultEnum != ERROR_NO_MORE_ITEMS) {
            printf("WNetEnumResource failed with error %d\n", dwResultEnum);

            //      NetErrorHandler(hwnd, dwResultEnum, (LPSTR)"WNetEnumResource");
            break;
        }
    }
    //
    // End do.
    //
    while (dwResultEnum != ERROR_NO_MORE_ITEMS);
    //
    // Call the GlobalFree function to free the memory.
    //
    GlobalFree((HGLOBAL) lpnrLocal);
    //
    // Call WNetCloseEnum to end the enumeration.
    //
    dwResult = WNetCloseEnum(hEnum);

    if (dwResult != NO_ERROR) {
        //
        // Process errors.
        //
        printf("WNetCloseEnum failed with error %d\n", dwResult);
        //    NetErrorHandler(hwnd, dwResult, (LPSTR)"WNetCloseEnum");
        return FALSE;
    }

    return TRUE;
}

void DisplayStruct(int i, LPNETRESOURCE lpnrLocal)
{
    printf("NETRESOURCE[%d] Scope: ", i);
    switch (lpnrLocal->dwScope) {
    case (RESOURCE_CONNECTED):
        printf("connected\n");
        break;
    case (RESOURCE_GLOBALNET):
        printf("all resources\n");
        break;
    case (RESOURCE_REMEMBERED):
        printf("remembered\n");
        break;
    default:
        printf("unknown scope %d\n", lpnrLocal->dwScope);
        break;
    }

    printf("NETRESOURCE[%d] Type: ", i);
    switch (lpnrLocal->dwType) {
    case (RESOURCETYPE_ANY):
        printf("any\n");
        break;
    case (RESOURCETYPE_DISK):
        printf("disk\n");
        break;
    case (RESOURCETYPE_PRINT):
        printf("print\n");
        break;
    default:
        printf("unknown type %d\n", lpnrLocal->dwType);
        break;
    }

    printf("NETRESOURCE[%d] DisplayType: ", i);
    switch (lpnrLocal->dwDisplayType) {
    case (RESOURCEDISPLAYTYPE_GENERIC):
        printf("generic\n");
        break;
    case (RESOURCEDISPLAYTYPE_DOMAIN):
        printf("domain\n");
        break;
    case (RESOURCEDISPLAYTYPE_SERVER):
        printf("server\n");
        break;
    case (RESOURCEDISPLAYTYPE_SHARE):
        printf("share\n");
        break;
    case (RESOURCEDISPLAYTYPE_FILE):
        printf("file\n");
        break;
    case (RESOURCEDISPLAYTYPE_GROUP):
        printf("group\n");
        break;
    case (RESOURCEDISPLAYTYPE_NETWORK):
        printf("network\n");
        break;
    default:
        printf("unknown display type %d\n", lpnrLocal->dwDisplayType);
        break;
    }

    printf("NETRESOURCE[%d] Usage: 0x%x = ", i, lpnrLocal->dwUsage);
    if (lpnrLocal->dwUsage & RESOURCEUSAGE_CONNECTABLE)
    printf("connectable ");
    if (lpnrLocal->dwUsage & RESOURCEUSAGE_CONTAINER)
    printf("container ");
    printf("\n");

    printf("NETRESOURCE[%d] Localname: %S\n", i, lpnrLocal->lpLocalName);
    printf("NETRESOURCE[%d] Remotename: %S\n", i, lpnrLocal->lpRemoteName);
    printf("NETRESOURCE[%d] Comment: %S\n", i, lpnrLocal->lpComment);
    printf("NETRESOURCE[%d] Provider: %S\n", i, lpnrLocal->lpProvider);
    printf("\n");
}


{
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>


#include <iostream>

#include "winsock2.h"
#include <string>
#include <Wininet.h>
#include <windows.h>
#include <fstream>��//Ҫʹ���ļ���������������ͷ�ļ�
using namespace std;
#define __HTTP_VERB_GET    "GET"
#define __HTTP_VERB_POST "POST"
#define __HTTP_ACCEPT_TYPE "*/*"
#define __HTTP_ACCEPT "Accept: */*\r\n"
#define __SIZE_HTTP_BUFFER    100000
#define __SIZE_HTTP_RESPONSE_BUFFER    100000
#define __SIZE_HTTP_HEAD_LINE    2048

void CALLBACK InternetStatusCallback(

                                    HINTERNET hInternet,
                                    DWORD dwContext,
                                    DWORD dwInternetStatus,
                                    LPVOID lpvStatusInformation,
                                    DWORD dwStatusInformationLength);
HANDLE hEvent[3];

HINTERNET hFile;
HINTERNET hNet;
HINTERNET hSession,hConnect,hRequest;
int WaitExitEvent()
{
    //return 1;
    DWORD dwRet = ::WaitForMultipleObjects(3, hEvent, FALSE, 30000);//INFINITE);
    int x=-1;
    switch (dwRet)
    {
        //����������¼����߶���������ɹ�����¼�
    case WAIT_OBJECT_0:
        x=0;
        cout<<"WAIT_OBJECT_0"<<endl;
        //������ر��¼�
        break;
    case WAIT_OBJECT_0+1:
        x=1;
        cout<<"WAIT_OBJECT_1"<<endl;
        //�û�Ҫ����ֹ���߳��¼����߷��������¼�
        break;
    case WAIT_OBJECT_0+2:
        x=2;
        cout<<"WAIT_OBJECT_2"<<endl;
        
        break;
    default:
        cout<<"WaitForMultipleObjects time out"<<endl;
        return -1;

    }
    return x;
}

// ֧�ִ������ã� �Ƿ��첽���ã� �����¼�����
void WinINet3(bool setProxy, bool ASYNC)
{
    hSession=NULL;
    hConnect=NULL;
    hRequest=NULL;
    for (int i = 0; i < 3; i++) 
    { 
        hEvent[i] = CreateEvent( 
            NULL,   // default security attributes
            FALSE, // auto-reset event object
            FALSE, // initial state is nonsignaled
            NULL); // unnamed object

        if (hEvent[i] == NULL) 
        { 
            printf("CreateEvent error: %d\n", GetLastError() ); 
            ExitProcess(0); 
        } 
    } 
    char *url = "http://down.360safe.com/setup.exe";
    char *pip = "down.360safe.com";
    char *paim = "/setup.exe";



    //   step 1
    if(ASYNC)    cout<<"�첽ģʽ"<<endl;
   // setProxy =false;
    if(setProxy)
    {
        cout<<"����ģʽ"<<endl;
        if(ASYNC)
          hSession = InternetOpen("name",
        INTERNET_OPEN_TYPE_DIRECT,//|INTERNET_OPEN_TYPE_PROXY,// INTERNET_OPEN_TYPE_PROXY,
        NULL,NULL,INTERNET_FLAG_ASYNC); // �첽
        else
          hSession = InternetOpen("name",INTERNET_OPEN_TYPE_PROXY,NULL,NULL,0); // ͬ��
    }
    else
    {
        if(ASYNC)
            hSession = InternetOpen("name",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,INTERNET_FLAG_ASYNC); // �첽
        else
            hSession = InternetOpen("name",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0); // ͬ��
    }
    if(!hSession){
        DWORD er = ::GetLastError();
        cout<<"InternetOpen error"<<endl;//, "Err", MB_OK);
        return;
    }
    if(ASYNC)
    {
        //Sleep(500);
        INTERNET_STATUS_CALLBACK res = ::InternetSetStatusCallback(hSession,InternetStatusCallback);
        if(res == INTERNET_INVALID_STATUS_CALLBACK)
        {
            cout<<"InternetSetStatusCallback failed, so return "<<endl;
            return ;    
        }
        else
        {
            cout<<"InternetSetStatusCallback succeed, so go on "<<endl;

        }
        //Sleep(500);
    }
    
    char   strProxyList[MAX_PATH],   strUsername[64],   strPassword[64];
    strcpy(strProxyList,   "SOCKS=58.56.87.2:1080"); //   д��socks��ô����Ч���أ�����SOCKS5=172.18.132.27:1080
    strcpy(strUsername,   "user01"); 
    strcpy(strPassword,   "baidu"); 
    INTERNET_PROXY_INFO proxy;
    proxy.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
    proxy.lpszProxy    = strProxyList;
    proxy.lpszProxyBypass = NULL;
    if( setProxy &&!InternetSetOption(hSession,INTERNET_OPTION_PROXY ,&proxy,sizeof(INTERNET_PROXY_INFO)))
    {
        cout<<"InternetSetOption failed"<<endl;
        return ;
    }
    
    // step 2
    //�����ȷ֪����Ҫ��֤����4��5���������������û���������"administrator","password"
    //��2��3������ΪĿ������IP���˿ں�(���Ǵ���������Ĳ���)
    hConnect = InternetConnect(hSession,pip,INTERNET_DEFAULT_HTTP_PORT,NULL,NULL,INTERNET_SERVICE_HTTP,INTERNET_FLAG_RELOAD,0);
    if(!ASYNC &&!hConnect){
        cout<<"ͬ����InternetConnect error"<<endl;//, "Err", MB_OK);
        return;
    }
    if( ASYNC&& hConnect== NULL)// �첽 ��Ҫ�ȴ�   ��Ȼֱ�Ӵ�������
    {
        int er = GetLastError();
        DWORD dwError = ::GetLastError();
        if (dwError != ERROR_IO_PENDING) 
        {
            cout<<"CHttpDownload::OpenInternetConnection| ����ʧ��" <<endl;
            return ;
        }
        else //
        {
            cout<<"hConnect == NULL, so run WaitExitEvent"<<endl;
            WaitExitEvent(); // �ȴ��ɹ����� // ����Ӧ�õȴ�   ����Ӧ����ʾһ��ѽ
            ::ResetEvent(hEvent[0]);
            ::ResetEvent(hEvent[1]);
            ::ResetEvent(hEvent[2]);
        }
    }
    cout<<"step 2 :InternetConnect secced"<<endl;

    // ::InternetSetStatusCallback(hConnect,InternetStatusCallback);
    
    // step 3!!!
    char   szHead[] = "Accept: */*\r\n\r\n";
    char **p = new char*[2];*p = szHead;*(p+1) = NULL;

    //hRequest = HttpOpenRequest(hConnect,"GET","download/BaiduHi_1.0_Beta2.exe",NULL,NULL,/*(const char **)p*/NULL,0/*INTERNET_FLAG_NO_COOKIES|INTERNET_FLAG_RELOAD*/,0); // no request;
    CONST TCHAR *szAcceptType=__HTTP_ACCEPT_TYPE;
    hRequest = ::HttpOpenRequest(hConnect,
        "GET",
        paim,
        HTTP_VERSION,
        "",
        &szAcceptType,
        INTERNET_FLAG_RELOAD|INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_NO_CACHE_WRITE,
        0);
    
    //::HttpAddRequestHeaders( hRequest, __HTTP_ACCEPT, strlen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE);
    /*_hHTTPRequest=::HttpOpenRequest(    _hHTTPConnection,
        __HTTP_VERB_GET, // HTTP Verb
        szURI, // Object Name
        HTTP_VERSION, // Version
        "", // Reference
        &szAcceptType, // Accept Type
        INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE,
        0); // context call-back point
    */
    if (!ASYNC&& !hRequest){
        cout<<"ͬ����HttpOpenRequest error"<<endl;//, "Err", MB_OK);
        return;
    }
    if( ASYNC&& hRequest== NULL)// �첽 ��Ҫ�ȴ�
    {
        int er = GetLastError();
        DWORD dwError = ::GetLastError();
        if (dwError != ERROR_IO_PENDING) 
        {
            cout<<"CHttpDownload::OpenInternetConnection| ����ʧ��" <<endl;
            return ;
        }
        else //
        {
            cout<<"hRequest == NULL, so run WaitExitEvent"<<endl;
            WaitExitEvent(); // �ȴ��ɹ�����
            ::ResetEvent(hEvent[0]);
            ::ResetEvent(hEvent[1]);
            ::ResetEvent(hEvent[2]);
        }
    }
    //Sleep(10000);
    cout << "step 3 : HttpOpenRequest success"<<endl;
    //::InternetSetStatusCallback(hRequest,InternetStatusCallback);
    //////////////////////////////////////////////
    if (setProxy )
    {
        // InternetSetOption ��Ҫ�첽�ȴ�
        if( !InternetSetOption(hRequest,INTERNET_OPTION_PROXY_USERNAME ,strUsername,strlen(strUsername)+1))
        {
            cout<<"InternetSetOption Username failed"<<endl;
            return ;
        }
        if( !InternetSetOption(hRequest,INTERNET_OPTION_PROXY_PASSWORD ,strPassword,strlen(strPassword)+1))
        {
            cout<<"InternetSetOption Password failed"<<endl;
            return ;
        }
    }
    // step 4
    //HttpSendRequest(hRequest,NULL,0,NULL,0);
    //Sleep(3000);
    ::ResetEvent(hEvent[0]);
    ::ResetEvent(hEvent[1]);
    ::ResetEvent(hEvent[2]);
    if(!::HttpSendRequest(hRequest,NULL,0,NULL,0)) // Ϊʲôʧ�ܣ�����
    {
        //Sleep(3000);
        if(!ASYNC)// ͬ��
        {
            DWORD dwError = ::GetLastError();
              cout<<"ͬ����HttpSendRequest failed, GetLastError=="<<dwError<<endl;
            return ;
        
        }
        else
        {
            Sleep(3000);
            DWORD dwError = ::GetLastError();
            cout<<"dwError =="<<dwError<<endl;
            if (dwError != ERROR_IO_PENDING) 
            {
                cout<<"dwError != ERROR_IO_PENDING, so quit,dwError =="<<dwError<<endl;
                return ;
            }
            else //
            {
                cout<<"HttpSendRequest, so run WaitExitEvent"<<endl;
                Sleep(3000);
                //if(WaitExitEvent()!=2)//; // �ȴ��ɹ����� �ȴ��Ƿ񲻶ԣ�����
                {
                       cout<<"had not recv complete event, so quit"<<endl;
                    // return ;
                }
            }
        }
        
    }
    Sleep(3000);
    cout << "step 4: HttpSendRequest success!"<<endl;

    int bufh[1000];
    DWORD dwLen,dwIndex;
    /*if(!::HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, bufh, &dwLen, &dwIndex))// ��仰������
    {
        //return E_FAIL;
        return;
    }
*/
    // �ж�״̬�룻
    char m_dwStatusCode[90];
    DWORD dwStatusSize = sizeof(m_dwStatusCode);
    /*if (FALSE == ::HttpQueryInfo(hRequest,   // ��ѯʧЧ����
        HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
        &m_dwStatusCode,
        &dwStatusSize,
        NULL))   //��ȡ����״̬��
    {
        return ;
    }
    //�ж�״̬���ǲ��� 200
    //if (HTTP_STATUS_OK != m_dwStatusCode)
    {
        //return ;
    }
*/

    DWORD dwByteToRead = 0;
    DWORD dwSizeOfRq = 4;
    DWORD dwBytes = 0;
    //������ֵ�ֱ�洢�ļ��Ĵ�С��HttpQueryInfo���ݵĴ�С���ܹ���ȡ���ֽ�����
    //HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL);
    //��Ҫ˵������ HttpQueryInfo ��������������������������Ҫ�����첽�����Ĵ��� 
    if (!HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL))
    { // ����ʧ���ˣ�����
        
        DWORD dwError = ::GetLastError();
        cout<<"HttpQueryInfo failed, so return�� GetLastError() =="<<dwError<<endl;
        return ;
    }
    FILE * pFile = fopen("e://baidu01.exe", "wb" );
    //ofstream mfile("out.txt");//�����ļ������ouf����������out.txt
    int i=0;
    DWORD leftB = dwByteToRead;
    cout<<"��ʼ����"<<endl;
    if( !ASYNC) // ͬ������
    {
        while(true)
        {
            const int MAX_BUFFER_SIZE = 65536;
            unsigned long nSize = 0;
            char szBuffer[MAX_BUFFER_SIZE+2];
            int num = MAX_BUFFER_SIZE;
            if( leftB < num);
            num = leftB;
            BOOL bRet = ::InternetReadFile(hRequest, szBuffer, num, &nSize); // �첽 ��Ҫ�ȴ�

            leftB -= nSize;
            cout<<i++<<" size: "<<nSize<<endl;
            if(!bRet || nSize <= 0)
                break;
            fwrite(szBuffer, sizeof(char), nSize, pFile);
        }
    }
    else // �첽����
    {
        INTERNET_BUFFERS i_buf = {0};
        i_buf.dwStructSize = sizeof(INTERNET_BUFFERS);
        i_buf.lpvBuffer = new TCHAR[10242];
        i_buf.dwBufferLength = 10240;
         for( DWORD i=0;i<dwByteToRead;)
        {
            //���ö������¼�
            ::ResetEvent( hEvent[0]);
            int num = 10240;
            if(dwByteToRead-i<10240)
            {
                num = dwByteToRead-i;
                   i_buf.dwBufferLength = dwByteToRead-i;
            }
            if (FALSE == ::InternetReadFileEx(hRequest,
                &i_buf,
                IRF_ASYNC,
                NULL))
            {
                if (ERROR_IO_PENDING == ::GetLastError())
                {
                    if ( NULL)//WaitExitEvent()!=2)
                    {
                        delete[] i_buf.lpvBuffer;
                        return ;
                    }
                }
                else 
                {
                    cout<<"down failed,so return"<<endl;
                    delete[] i_buf.lpvBuffer;
                    return ;
                }
            }
            else
            {
                //�����紫���ٶȿ죬������С������£�
                //InternetReadFileEx ������ֱ�ӷ��سɹ���
                //���Ҫ�ж��Ƿ������û�Ҫ����ֹ���߳��¼���
                cout<<"����ܺã�InternetReadFileEx����true"<<endl;

                // �ݲ������û��˳�
            }
            i += i_buf.dwBufferLength; // ���һ��д���ˣ�����
            fwrite(i_buf.lpvBuffer, sizeof(char), i_buf.dwBufferLength, pFile);
            cout<<"i== "<<i<<endl;
            //��������
             //֪ͨ���߳����ؽ���
                    
        }
    }
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hSession);
    cout<<"success download file"<<endl;
    
    return;
}

int main( void )
{
    
    WinINet3(true,true);
    return 1;
}

void OnInternetHandleCreated(HINTERNET hInternet, LPINTERNET_ASYNC_RESULT lpInetStatusResult)
{
    if(NULL == lpInetStatusResult)
    {
        //ATLASSERT( 0 );
        return;
    }
    hFile = HINTERNET(lpInetStatusResult->dwResult);
    HINTERNET    hInet = HINTERNET(lpInetStatusResult->dwResult);
    DWORD        dwInetHandleType;
    DWORD        dwTypeLen = sizeof(dwInetHandleType);

    InternetQueryOption( hInet, INTERNET_OPTION_HANDLE_TYPE, &dwInetHandleType, &dwTypeLen);
    switch(dwInetHandleType) 
    {
    case INTERNET_HANDLE_TYPE_CONNECT_HTTP:
        //CloseInternetConnection(); //   �����Ǻ��⣿������ ͨ���ص� ����httpConnect
        hConnect = hInet;     // 
        break;
    case INTERNET_HANDLE_TYPE_HTTP_REQUEST:
        //CloseInternetFile();    //    �����Ǻ��⣿��    ͨ���ص� ����httpFile
        hRequest = hInet;    //
        break; 
    default:
        break;
    }
    cout<<"OnInternetHandleCreated, so ::SetEvent(hEvent[0])"<<endl;
    // HANDLE�Ѵ����¼����첽���ƣ�
    ::SetEvent(hEvent[0]);
}
void OnInternetRequestComplete(HINTERNET hInternet, LPINTERNET_ASYNC_RESULT lpInetStatusResult)
{

    if( lpInetStatusResult == NULL )
    {
        //ATLASSERT( 0 );
        return;
    }
    cout<<"OnInternetRequestComplete, so ::SetEvent(hEvent[2])"<<endl;
    // ������������¼����첽���ƣ�
    ::SetEvent(hEvent[0]);
}

void CALLBACK InternetStatusCallback(
                                    HINTERNET hInternet,
                                    DWORD_PTR dwContext,
                                    DWORD dwInternetStatus,
                                    LPVOID lpvStatusInformation,
                                    DWORD dwStatusInformationLength
                                    )
{
    cout<<"����ص�"<<endl;
    switch (dwInternetStatus)
    {
    case INTERNET_STATUS_RESOLVING_NAME:
        break;
    case INTERNET_STATUS_NAME_RESOLVED:
        break;
    case INTERNET_STATUS_CONNECTING_TO_SERVER:
        break;
    case INTERNET_STATUS_CONNECTED_TO_SERVER:
        break;
    case INTERNET_STATUS_SENDING_REQUEST:
        break;
    case INTERNET_STATUS_REQUEST_SENT:
        break;
    case INTERNET_STATUS_RECEIVING_RESPONSE:
        break;
    case INTERNET_STATUS_RESPONSE_RECEIVED:
        break;
    case INTERNET_STATUS_CLOSING_CONNECTION:
        break;
    case INTERNET_STATUS_CONNECTION_CLOSED:
        break;
    case INTERNET_STATUS_HANDLE_CREATED:
        cout<<"�ص���INTERNET_STATUS_HANDLE_CREATED"<<endl;
        OnInternetHandleCreated(hInternet, LPINTERNET_ASYNC_RESULT(lpvStatusInformation)); // ������HINTERNET ���Ǿ���ѽ
        break;
    case INTERNET_STATUS_HANDLE_CLOSING:
        break;
    case INTERNET_STATUS_REQUEST_COMPLETE:
        cout<<"�ص���INTERNET_STATUS_REQUEST_COMPLETE"<<endl;
        OnInternetRequestComplete(hInternet, LPINTERNET_ASYNC_RESULT(lpvStatusInformation));
        break;
    case INTERNET_STATUS_REDIRECT:
    case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
    case INTERNET_STATUS_STATE_CHANGE:
    default:
        break;
    }
}


}