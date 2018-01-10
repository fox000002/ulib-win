#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "udllman.h"

typedef std::vector<void *> CPtrArray;

#define TRACE_ENABLED

#ifdef TRACE_ENABLED
#define TRACE_STATUS(msg)\
        std::cout << msg << std::endl;
#define TRACE_ERROR(msg,errorcode)\
        OutputFormattedErrorString(msg,errorcode);
#else
#define TRACE_STATUS(msg)
#endif

#define TRACE_STATUS_TO_LIST(msg)\
      {\
       std::cout << ">>" << std::endl;\
      }

typedef struct OVERLAPPEDEX : public OVERLAPPED
{
    LPVOID pCustomData; // Append custom data to the end of the system defined structure
}*LPOVERLAPPEDEX;

const int MAX_TIMEOUT = 60*1000; // 1 minute, thinks it is sufficient

class IAsyncOperationCompletionNotifier
{
public:
    virtual void OnAsyncOperationComplete(BOOL bRead,DWORD dwErrorCode)=0;

};

class AsyncFile
{
public:
    AsyncFile(LPCTSTR lpctszFileName,BOOL bCreate,DWORD dwDesiredAccess, DWORD dwShareMode,
              IAsyncOperationCompletionNotifier* pINotifier,
              BOOL bSequentialMode=FALSE,
              __int64 nStartOffset=0,
              BOOL bInUIThread=FALSE);
    BOOL IsOpen();
    BOOL Write(LPVOID pvBuffer,DWORD dwBufLen,DWORD dwOffsetLow=0,DWORD dwOffsetHigh=0);
    BOOL Read(LPVOID pvBuffer,DWORD dwBufLen,DWORD dwOffsetLow=0,DWORD dwOffsetHigh=0);
    BOOL IsAsyncIOComplete(BOOL bFlushBuffers=TRUE);
    BOOL AbortIO();
    DWORD GetFileLength(DWORD* pdwOffsetHigh);
    __int64 GetLargeFileLength();
    VOID Reset(BOOL bSequentialMode=FALSE,__int64 nStartOffset=0);
    operator HANDLE()
    {
       return m_hAsyncFile;
    }
    BOOL SeekFile(__int64 nBytesToSeek,__int64& nNewOffset,DWORD dwSeekOption);
    ~AsyncFile(void);
private:
    LPOVERLAPPED PreAsyncIO(DWORD dwOffsetLow,DWORD dwOffsetHigh);
    LPOVERLAPPED GetOverlappedPtr(DWORD dwOffsetLow,DWORD dwOffsetHigh);
    BOOL PostAsyncIO(BOOL bRet,DWORD dwBufLen);
    BOOL WaitOnUIThread();
    BOOL NormalWait();
    VOID PumpMessage();
    static VOID WINAPI FileWriteIOCompletionRoutine(DWORD dwErrorCode,
                                                    DWORD dwNumberOfBytesTransfered,
                                                    LPOVERLAPPED lpOverlapped);
    static VOID WINAPI FileReadIOCompletionRoutine(DWORD dwErrorCode,
                                                   DWORD dwNumberOfBytesTransfered,
                                                   LPOVERLAPPED lpOverlapped);

    static VOID OnFileIOComplete(DWORD dwErrorCode,
                                 DWORD dwNumberOfBytesTransfered,
                                 LPOVERLAPPED lpOverlapped,BOOL bRead);
    VOID CheckAndRaiseIOCompleteEvent();
    VOID Cleanup(BOOL bFlushBuffers);
    static VOID OutputFormattedErrorString(TCHAR* ptcMsg,DWORD dwErrorCode);
    AsyncFile();
    AsyncFile(const AsyncFile&);
    AsyncFile& operator=(const AsyncFile&);
private:
    IAsyncOperationCompletionNotifier* m_pNotifier;
    HANDLE m_hAsyncFile;
    HANDLE m_hIOCompleteEvent;
    DWORD m_dwAccessMode;
    BOOL m_bSequentialMode;
    __int64 m_nNumOfBytesRequested;
    __int64 m_nOffset;
    DWORD m_dwReqestedOvpOprnCount;
    DWORD m_dwErrorCode;
    BOOL m_bInUIThread;
    CPtrArray* m_pOvpBufArray;
    BOOL m_bCleanupDone;
    BOOL m_bAborted;
};

/*//////////////////////////////////////////////////////////////////////////////////////

The constructor which opens the specified file in overlapped mode. It creates
the file bCreate = TRUE, otherwise open existing. 
An I/O callback interface can be specified by the client. 
This shall be invoked when the IOCompletion routine is triggered by the system. 

The AyncFile object can be created in sequential mode. In this case the offset 
shall be calculated internally. But if the clients specifies the offset, this 
flag is reset and client specified offset will be used. 

The client can also specify whether it is in a UI thread or not. If UI thread, 
the message pumbing shall be done to make the UI responsive.

lpctszFileName               - The full path to file

bCreate                      - Whether a new file needs to be created or not. 
                              If the file already exists, the contents will be lost.

dwDesiredAccess, dwShareMode - Same as that for CreateFile

pINotifier                   - The callback interface specified by client. 
                               Its OnAsyncOperationComplete function shall be invoked 
                               from IOCompletion routine.

bSequentialMode              - If this flag is TRUE the offset is incremented sequentially. Here
                               the client does not need to specify the offset in Read/Write calls. 
                               Otherwise client needs to specify the offset during the Read/Write calls.

nStartOffset                 - The offset from which Read/Write needs to be started. This is relevant
                               only in case bSequentialMode = TRUE

bInUIThread                  - Whether asynchronous I/O is performed in the context of UI thread or not

///////////////////////////////////////////////////////////////////////////////////////*/

AsyncFile::AsyncFile(LPCTSTR lpctszFileName,BOOL bCreate,
                     DWORD dwDesiredAccess, DWORD dwShareMode,
                     IAsyncOperationCompletionNotifier* pINotifier,
                     BOOL bSequentialMode,
                     __int64 nStartOffset,
                     BOOL bInUIThread)
                     :m_pNotifier(pINotifier),
                      m_hAsyncFile(INVALID_HANDLE_VALUE),
                      m_hIOCompleteEvent(0),
                      m_dwAccessMode(dwDesiredAccess),
                      m_bSequentialMode(bSequentialMode),
                      m_nNumOfBytesRequested(0),
                      m_nOffset(nStartOffset),
                      m_dwReqestedOvpOprnCount(0),
                      m_dwErrorCode(0),
                      m_bInUIThread(bInUIThread),
                      m_pOvpBufArray(0),
                      m_bCleanupDone(FALSE),
                      m_bAborted(FALSE)
{
    DWORD dwCreateMode = OPEN_EXISTING;
    if( TRUE == bCreate )
    {

        dwCreateMode = CREATE_ALWAYS;
    }
    if( 0 != lpctszFileName )
    {
        m_hAsyncFile= CreateFile(lpctszFileName,dwDesiredAccess,dwShareMode,0,dwCreateMode,
                                 FILE_FLAG_OVERLAPPED,0);
    }
    if( INVALID_HANDLE_VALUE == m_hAsyncFile)
    {        
        TCHAR tcszErrMsg[100] = {0};
        _stprintf(tcszErrMsg,_T("File open failed -->%s"),lpctszFileName);
        TRACE_ERROR(tcszErrMsg,GetLastError());
    }
    m_pOvpBufArray = new CPtrArray;
    m_hIOCompleteEvent = CreateEvent(0,TRUE,FALSE,0);
}

/*//////////////////////////////////////////////////////////////////////////////////////

Returns TRUE if the file is successfully opened.

///////////////////////////////////////////////////////////////////////////////////////*/

inline BOOL AsyncFile::IsOpen()
{
    return (INVALID_HANDLE_VALUE != m_hAsyncFile);
}

/*//////////////////////////////////////////////////////////////////////////////////////
Performs the asynchronous Write operation.

pvBuffer    - The buffer to be written
dwBufLen    - The length of the buffer specified
dwOffsetLow - The lower part of the offset where input data is to be written
dwOffsetHigh- The higher part of the offset where input data is to be written. Normally
              this is specified for big files > 4 GB
///////////////////////////////////////////////////////////////////////////////////////*/

BOOL AsyncFile::Write(LPVOID pvBuffer,DWORD dwBufLen,DWORD dwOffsetLow,DWORD dwOffsetHigh)
{
    if( 0 == pvBuffer )
        return FALSE;
    LPOVERLAPPEDEX pOverlapped = 0;
    if( 0 == ( pOverlapped = static_cast<LPOVERLAPPEDEX>(PreAsyncIO(dwOffsetLow,dwOffsetHigh))))
    {
        return FALSE;
    }
    
    BOOL bRet = WriteFileEx(m_hAsyncFile,pvBuffer,dwBufLen,pOverlapped,
                            FileWriteIOCompletionRoutine);
    if( FALSE == bRet )
    {
       TRACE_STATUS(_T("WriteFileEx completed --> with error"));
    }
    else
    {
       TRACE_STATUS(_T("WriteFileEx completed --> with success"));
    }
    return PostAsyncIO(bRet,dwBufLen);    
}

/*//////////////////////////////////////////////////////////////////////////////////////
Performs the asynchronous Read operation.

pvBuffer    - The buffer to be read
dwBufLen    - The length of the buffer specified
dwOffsetLow - The lower part of the offset from where data is to be read
dwOffsetHigh- The higher part of the offset from where data is to be read. Normally
              this is specified for big files > 4 GB
///////////////////////////////////////////////////////////////////////////////////////*/

BOOL AsyncFile::Read(LPVOID pvBuffer,DWORD dwBufLen,DWORD dwOffsetLow,DWORD dwOffsetHigh)
{
    if( 0 == pvBuffer )
        return FALSE;
    LPOVERLAPPED pOverlapped = 0;
    if( 0 == ( pOverlapped = PreAsyncIO(dwOffsetLow,dwOffsetHigh)))
    {
        return FALSE;
    }
    BOOL bRet = ReadFileEx(m_hAsyncFile,pvBuffer,dwBufLen,pOverlapped,
                           FileReadIOCompletionRoutine);
    if( FALSE == bRet )
    {
       TRACE_STATUS(_T("ReadFileEx completed --> with error"));
    }
    else
    {
       TRACE_STATUS(_T("ReadFileEx completed --> with success"));
    }
    return PostAsyncIO(bRet,dwBufLen);
}


/*//////////////////////////////////////////////////////////////////////////////////////
The common routine called from Read/Write to prepare the OVERLAPPEDEX pointer with the help
of GetOverlappedPtr(). It keeps the OVERLAPPEDEX pointer in an array to be used during the 
GetOverlappedResult call.

dwOffsetLow - The lower part of the offset from where data is to be read/written
dwOffsetHigh- The higher part of the offset from where data is to be read/written. Normally
              this is specified for big files > 4 GB
///////////////////////////////////////////////////////////////////////////////////////*/

LPOVERLAPPED AsyncFile::PreAsyncIO(DWORD dwOffsetLow,DWORD dwOffsetHigh)
{
    if(!IsOpen())
    {
        return 0;
    }
    LPOVERLAPPED pOverlapped = GetOverlappedPtr(dwOffsetLow,dwOffsetHigh);
    if( 0 == pOverlapped )
    {
        return 0;
    }
    if( m_pOvpBufArray )
    {
        m_pOvpBufArray->push_back(pOverlapped);
    }
    ++m_dwReqestedOvpOprnCount;
    m_bAborted = FALSE;
    return pOverlapped;
}

/*//////////////////////////////////////////////////////////////////////////////////////
The common routine called from Read/Write to prepare the OVERLAPPEDEX pointer. It also 
fills the custom pointer to make the context available during the OnFileIOComplete

dwOffsetLow - The lower part of the offset from where data is to be read/written
dwOffsetHigh- The higher part of the offset from where data is to be read/written. Normally
              this is specified for big files > 4 GB
///////////////////////////////////////////////////////////////////////////////////////*/

LPOVERLAPPED AsyncFile::GetOverlappedPtr(DWORD dwOffsetLow,DWORD dwOffsetHigh)
{
    LPOVERLAPPEDEX pOverlapped = new OVERLAPPEDEX;
    ZeroMemory(pOverlapped,sizeof(LPOVERLAPPEDEX));
    if( 0 == pOverlapped )
    {
        return 0;
    } 

    //if the client specified offset, override the m_bSequentialMode
    if( (0 != dwOffsetLow) || (0!=dwOffsetHigh) )
    {
        m_bSequentialMode = FALSE;
    }
    if( TRUE == m_bSequentialMode )
    {
        LARGE_INTEGER LargeOffset = {0};
        LargeOffset.QuadPart = m_nOffset;
        pOverlapped->Offset = LargeOffset.LowPart;
        pOverlapped->OffsetHigh = LargeOffset.HighPart;
    }
    else
    {
        pOverlapped->Offset = dwOffsetLow;
        pOverlapped->OffsetHigh = dwOffsetHigh;
    }
    pOverlapped->pCustomData = this;
    return pOverlapped;
}

/*//////////////////////////////////////////////////////////////////////////////////////
The common routine called from Read/Write after the asynchronous I/O call. It calculates
the next offset if the m_bSequentialMode flag is TRUE. Also it keeps track of bytes requested
to Read/Write. This is to compare it after the whole I/O is completed. If all the bytes are
not Read/Write there is data loss. In the case of UI thread it dispatches the window messages
to make the UI responsive.

bRet     - The return of ReadFileEx/WriteFileEx
dwBufLen - The length of data buffer specified
///////////////////////////////////////////////////////////////////////////////////////*/

BOOL AsyncFile::PostAsyncIO(BOOL bRet,DWORD dwBufLen)
{
    if( m_bInUIThread )
    {
        PumpMessage();
    }
    DWORD dwLastError = GetLastError();
    if( FALSE == bRet )
    {
        TRACE_ERROR(_T("IO operation failed"),dwLastError);
        return FALSE;
    }
    
    if( ERROR_SUCCESS == dwLastError )
    {
        if( TRUE == m_bSequentialMode )
        {
            m_nOffset += dwBufLen;
        }
        m_nNumOfBytesRequested += dwBufLen;
        return TRUE;
    }
    return FALSE;
}

/*//////////////////////////////////////////////////////////////////////////////////////
The interface to cancel the asynchronous I/O operation. It signals the IOCompletion event
to stop waiting for the I/O to complete.
///////////////////////////////////////////////////////////////////////////////////////*/

BOOL AsyncFile::AbortIO()
{
    BOOL bRet = CancelIo(m_hAsyncFile);
    if( FALSE == bRet )
    {
       TRACE_ERROR(_T("CancelIo failed"),GetLastError());
    }
    else
    {
       TRACE_STATUS(_T("CancelIo completed --> with success"));
    }
    ::SetEvent (m_hIOCompleteEvent);
    m_bAborted = TRUE;
    return bRet;
}

/*//////////////////////////////////////////////////////////////////////////////////////
The call back function registered with WriteFileEx. This will be invoked when a WriteFilEx
oeration is completed.

dwErrorCode                 - The status of the operation
dwNumberOfBytesTransfered   - The number of bytes written
lpOverlapped                - The OVERLAPPEDEX pointer provided during WriteFileEx call
///////////////////////////////////////////////////////////////////////////////////////*/

VOID WINAPI AsyncFile::FileWriteIOCompletionRoutine(DWORD dwErrorCode,
                                                    DWORD dwNumberOfBytesTransfered,
                                                    LPOVERLAPPED lpOverlapped)
{
    OnFileIOComplete( dwErrorCode,dwNumberOfBytesTransfered,lpOverlapped,FALSE);
}

/*//////////////////////////////////////////////////////////////////////////////////////
The call back function registered with WriteFileEx. This will be invoked when a ReadFileEx
oeration is completed.

dwErrorCode                 - The status of the operation
dwNumberOfBytesTransfered   - The number of bytes read
lpOverlapped                - The OVERLAPPEDEX pointer provided during ReadFileEx call
///////////////////////////////////////////////////////////////////////////////////////*/

VOID WINAPI AsyncFile::FileReadIOCompletionRoutine(DWORD dwErrorCode,
                                                   DWORD dwNumberOfBytesTransfered,
                                                   LPOVERLAPPED lpOverlapped)
{
    OnFileIOComplete( dwErrorCode,dwNumberOfBytesTransfered,lpOverlapped,TRUE);
}

/*//////////////////////////////////////////////////////////////////////////////////////
The common routine which handles call back function registered with ReadFileEx/WriteFileEx.
This will be invoked when a ReadFileEx/WriteFileEx oeration is completed. It invokes the
callback interface registered by the client.

dwErrorCode                 - The status of the operation
dwNumberOfBytesTransfered   - The number of bytes read
lpOverlapped                - The OVERLAPPEDEX pointer provided during ReadFileEx/WriteFileEx
                              call
bRead                       - Identified Read/Write
///////////////////////////////////////////////////////////////////////////////////////*/

VOID AsyncFile::OnFileIOComplete(DWORD dwErrorCode,
                                 DWORD dwNumberOfBytesTransfered,
                                 LPOVERLAPPED lpOverlapped,BOOL bRead)
{
    if( 0 == lpOverlapped )
    {
        TRACE_STATUS(_T("FileIOCompletionRoutine completed --> with POVERLAPPED NULL"));
        return;
    }
    LPOVERLAPPEDEX pOverlappedEx = static_cast<LPOVERLAPPEDEX>(lpOverlapped);
    AsyncFile* pAsyncFile = reinterpret_cast<AsyncFile*>(pOverlappedEx->pCustomData);
    pAsyncFile->m_dwErrorCode = dwErrorCode;

    if( ERROR_SUCCESS == dwErrorCode )
    {
        TRACE_STATUS(_T("FileIOCompletionRoutine completed --> with success"));        
    }
    else
    {
        TRACE_ERROR(_T("FileIOCompletionRoutine completed"),dwErrorCode);
    }
    pAsyncFile->CheckAndRaiseIOCompleteEvent();
    if( pAsyncFile->m_pNotifier )
    {
       pAsyncFile->m_pNotifier->OnAsyncOperationComplete( bRead,dwErrorCode);
    }
}

/*//////////////////////////////////////////////////////////////////////////////////////
Checks whether all the requested overlapped calls completed by decrementing a counter.
If the counter is 0, all the requested asynchronous operations got callbacks. Then it
will set the m_hIOCompleteEvent to end the alertable wait state.
///////////////////////////////////////////////////////////////////////////////////////*/

VOID AsyncFile::CheckAndRaiseIOCompleteEvent()
{
    (m_dwReqestedOvpOprnCount > 0 )?--m_dwReqestedOvpOprnCount:m_dwReqestedOvpOprnCount;
    if( 0 == m_dwReqestedOvpOprnCount )
    {
        ::SetEvent(m_hIOCompleteEvent);
    }
}

/*//////////////////////////////////////////////////////////////////////////////////////
The multiple overlapped Read/Write requests can be done. Now, the thread is free to do
other operations. Finally, when the thread needs to know the status of the asynchronous
operation, it can place the thread in alretable wait state with the help of 
WaitForSingleObjectEx or MsgWaitForMultipleObjectsEx(for UI thread). Then the IOCompletion
routine registered will be called per asynchrnous I/O call requested. Once all the callbacks
are invoked the m_hIOCompleteEvent is signalled and the alertable wait state is completed.
Now, the GetOverlappedResult() API is invoked to find whether all the requested I/O operations
compeleted successfully.

bFlushBuffers - Whether to flush the file buffers or not. 
                This is required for preventing data loss as system performs a delayed write.
                So when an abnormal shutdown/restart of system happens, data is lost.
///////////////////////////////////////////////////////////////////////////////////////*/

BOOL AsyncFile::IsAsyncIOComplete(BOOL bFlushBuffers)
{
    if(!IsOpen())
    {
        return FALSE;
    }
    
    BOOL bWaitReturn = FALSE;
    if( m_bInUIThread )
    {
        bWaitReturn = WaitOnUIThread();
    }
    else
    {
        bWaitReturn = NormalWait();
    }

    int nTotalNumOfBytesTrans = 0;
    int nOvpBufferCount = m_pOvpBufArray->size();

    for( int nIdx = 0; nIdx < nOvpBufferCount; ++nIdx )
    {
        LPOVERLAPPED lpOverlapped = reinterpret_cast<LPOVERLAPPED>(m_pOvpBufArray->at(nIdx));
        DWORD dwNumberOfBytesTransferred = 0;
        BOOL bRet = GetOverlappedResult(m_hAsyncFile,lpOverlapped,&dwNumberOfBytesTransferred,TRUE);
        if( FALSE == bRet )
        {
            m_dwErrorCode = GetLastError();
            if( ERROR_IO_INCOMPLETE == m_dwErrorCode )
            {
                TRACE_STATUS(_T("IsAsyncIOComplete --> IO pending..."));
            }            
        }
        nTotalNumOfBytesTrans += dwNumberOfBytesTransferred;
        if( TRUE == m_bInUIThread )
        {
            PumpMessage();
        }
    }
    
    Cleanup(bFlushBuffers);

    if( m_nNumOfBytesRequested == nTotalNumOfBytesTrans )
    {
        m_nNumOfBytesRequested = 0;
        return TRUE;
    }

    if( FALSE == bWaitReturn )
    {
        return FALSE;
    }
    if( ERROR_SUCCESS != m_dwErrorCode )
    {
        TRACE_ERROR(_T("AsyncIO operation completed with error"),m_dwErrorCode);
        return FALSE;
    }
    return FALSE;
}

/*//////////////////////////////////////////////////////////////////////////////////////
Clears all the data structures. If bFlushBuffers = TRUE calls, FlushFileBuffers() API
to write all the pending data to disk.

bFlushBuffers - Whether to flush the file buffers or not. 
                This is required for preventing data loss as system performs a delayed write.
                So when an abnormal shutdown/restart of system happens, data is lost.
///////////////////////////////////////////////////////////////////////////////////////*/

VOID AsyncFile::Cleanup(BOOL bFlushBuffers)
{
    if( TRUE == m_bCleanupDone )
        return;
    int nOvpBufCount = m_pOvpBufArray->size();
    for( int nIdx = 0; nIdx < nOvpBufCount; ++nIdx )
    {
        LPOVERLAPPEDEX pOverlapped = reinterpret_cast<LPOVERLAPPEDEX>(m_pOvpBufArray->at(nIdx));
        delete pOverlapped;
    }
    
    m_pOvpBufArray->clear();

    if( ( GENERIC_READ != m_dwAccessMode ) && ( TRUE == bFlushBuffers ) && ( FALSE == m_bAborted) )
    {
        TRACE_STATUS(_T("Flushing file buffers..."));
        if( FALSE == FlushFileBuffers(m_hAsyncFile))
        {
            TRACE_ERROR(_T("FlushFileBuffers failed"),GetLastError());
        }
    }
    ::ResetEvent(m_hIOCompleteEvent);
    m_bCleanupDone = TRUE;
}

/*//////////////////////////////////////////////////////////////////////////////////////
The IOCompletion callbacks shall be invoked by the system, when the thread is palaced in
the alertable wait state. Actaully, from my observation, these callbacks are called in the
context of the wait API. In the case of UI thread, to make the UI responsive the window
messages should be dispatched. So the MsgWaitForMultipleEx() API is used to wait. Therefore
when a message is available in the queue, it will be dispatched. The function returns failure
in the case of timeout or any other failure. Otherwise it waits till the m_hIOCompleteEvent
is signalled.
///////////////////////////////////////////////////////////////////////////////////////*/

BOOL AsyncFile::WaitOnUIThread()
{
    for( ;; )
    {
        DWORD dwWaitOvpOprn = MsgWaitForMultipleObjectsEx( 1, &m_hIOCompleteEvent, 
                                                           MAX_TIMEOUT, QS_ALLEVENTS, MWMO_ALERTABLE|MWMO_INPUTAVAILABLE );
        switch( dwWaitOvpOprn )
        {
        case WAIT_FAILED:
            return FALSE;
        case WAIT_OBJECT_0:
            return TRUE;
        case WAIT_TIMEOUT:
            return FALSE;
        }
        
        if( m_bAborted )
        {
            return FALSE;
        }
        
        // Make the UI responsive, dispatch any message in the queue
        PumpMessage();
    }
    return FALSE;
}

/*//////////////////////////////////////////////////////////////////////////////////////
This is for non UI threads. The IOCompletion callbacks shall be invoked by the system, 
when the thread is palaced in the alertable wait state. Actaully, from my observation, 
these callbacks are called in the context of the wait API. The function returns failure
in the case of timeout or any other failure. Otherwise it waits till the m_hIOCompleteEvent
is signalled.
///////////////////////////////////////////////////////////////////////////////////////*/

BOOL AsyncFile::NormalWait()
{
    for( ; ; )
    {
        DWORD dwWaitOvpOprn = WaitForSingleObjectEx( m_hIOCompleteEvent,MAX_TIMEOUT, TRUE );
        switch( dwWaitOvpOprn )
        {
        case WAIT_FAILED:
            return FALSE;
        case WAIT_OBJECT_0:
            return TRUE;
        case WAIT_TIMEOUT:
            return FALSE;
        }
        
        if( m_bAborted )
        {
            return FALSE;
        }
    }
    return FALSE;
}

/*//////////////////////////////////////////////////////////////////////////////////////
To prepare the AsyncFile instance for next set of I/O operations. Resets all required
members.
///////////////////////////////////////////////////////////////////////////////////////*/

VOID AsyncFile::Reset(BOOL bSequentialMode,__int64 nStartOffset)
{
    m_bSequentialMode = bSequentialMode;
    m_nOffset = nStartOffset;
    m_nNumOfBytesRequested = 0;
    m_dwReqestedOvpOprnCount = 0;
    m_dwErrorCode = 0;
    m_bCleanupDone = FALSE;
    m_bAborted = FALSE;
}

/*//////////////////////////////////////////////////////////////////////////////////////
Returns the size of the file. This function can be used if the size is required as low
and high parts.
///////////////////////////////////////////////////////////////////////////////////////*/

DWORD AsyncFile::GetFileLength(DWORD* pdwOffsetHigh)
{
    return GetFileSize(m_hAsyncFile,pdwOffsetHigh);
}

/*//////////////////////////////////////////////////////////////////////////////////////
Returns the size of the file. This function can be used if the size as an int64 value.
///////////////////////////////////////////////////////////////////////////////////////*/

//BOOL WINAPI GetFileSizeEx(
//     HANDLE hFile,
//     PLARGE_INTEGER lpFileSize
//);

__int64 AsyncFile::GetLargeFileLength()
{
    LARGE_INTEGER liFileSize = {0};
	
	//
    UDllMan udm(_T("Kernel32.dll"));
	BOOL bRet = udm.callFunc<BOOL, HANDLE, PLARGE_INTEGER>(_T("GetFileSizeEx"), m_hAsyncFile,&liFileSize);
	
    //BOOL bRet = GetFileSizeEx(m_hAsyncFile,&liFileSize);
    if( FALSE != bRet )
    {
        return liFileSize.QuadPart;
    }
    return -1;
}

/*//////////////////////////////////////////////////////////////////////////////////////
Wrapper for the SetFilePointer() API. Moves the file pointer to the specified offset.
///////////////////////////////////////////////////////////////////////////////////////*/

BOOL AsyncFile::SeekFile(__int64 nBytesToSeek,__int64& nNewOffset,DWORD dwSeekOption)
{
    LARGE_INTEGER liBytesToSeek = {0};
    liBytesToSeek.QuadPart = nBytesToSeek;
    LARGE_INTEGER liNewOffset   = {0};
    BOOL bRet = SetFilePointerEx( m_hAsyncFile,liBytesToSeek,&liNewOffset,dwSeekOption);
    if( bRet )
    {
        nNewOffset = liNewOffset.QuadPart;
        return TRUE;
    }
    return FALSE;
}

/*//////////////////////////////////////////////////////////////////////////////////////
The destructor. Obiviously, clears all the members.
///////////////////////////////////////////////////////////////////////////////////////*/

AsyncFile::~AsyncFile(void)
{
    Cleanup(TRUE);
    delete m_pOvpBufArray;
    m_pOvpBufArray = 0;
    ::SetEvent (m_hIOCompleteEvent);
    CloseHandle(m_hIOCompleteEvent);
    m_hIOCompleteEvent = 0;
    if(IsOpen())
    {
        CloseHandle(m_hAsyncFile);
    }
    m_hAsyncFile = 0;
}

/*//////////////////////////////////////////////////////////////////////////////////////
Dispatches the window messages to make the UI responsive.
///////////////////////////////////////////////////////////////////////////////////////*/

VOID AsyncFile::PumpMessage()
{
    MSG stMsg = {0};
    while( ::PeekMessage(&stMsg,0,0,0,PM_REMOVE))
    {
        TranslateMessage(&stMsg);
        DispatchMessage(&stMsg);
    }
}

/*//////////////////////////////////////////////////////////////////////////////////////
Formats error message from system error code.
///////////////////////////////////////////////////////////////////////////////////////*/

VOID AsyncFile::OutputFormattedErrorString(TCHAR* ptcMsg,DWORD dwErrorCode)
{
    LPTSTR lptszMsgBuf = 0;
    DWORD dwBufLen = 0;
    if( 0!= (dwBufLen=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
                                    FORMAT_MESSAGE_FROM_SYSTEM|
                                    FORMAT_MESSAGE_IGNORE_INSERTS,
                                    0,dwErrorCode,
                                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                    reinterpret_cast<LPTSTR>(&lptszMsgBuf),0,0)))
    {
        TCHAR* INFIX = _T("-->");
        dwBufLen +=_tcslen(ptcMsg)+_tcslen(INFIX)+1;
        TCHAR* ptcErrMsg = new TCHAR[dwBufLen];
        ZeroMemory(ptcErrMsg,sizeof(TCHAR)*dwBufLen);
        _stprintf(ptcErrMsg,_T("%s%s%s"),ptcMsg,INFIX,lptszMsgBuf);
        TRACE_STATUS(ptcErrMsg);
        LocalFree(lptszMsgBuf);
        delete[] ptcErrMsg;
    }
}

class ProgressNotifier
{
public:
    virtual void ShowProgress(LPCTSTR lpctszMessage)=0;
};
class CopyHandler : public IAsyncOperationCompletionNotifier
{
public:
    CopyHandler(ProgressNotifier* pProgressNotifier);
    ~CopyHandler(void);
    void CopyFile(LPCTSTR lpctszSourceFile,LPCTSTR lpctszTargetFile);
private:
    BOOL CopyFileData( AsyncFile& SourceFile, AsyncFile& TargetFile, CPtrArray& BufArray, int nBufSize );
    void OnAsyncOperationComplete(BOOL bRead,DWORD dwErrorCode);
    ProgressNotifier* m_pProgressNotifier;
    BOOL m_bAbort;
};

CopyHandler::CopyHandler(ProgressNotifier* pProgressNotifier):m_pProgressNotifier(pProgressNotifier),m_bAbort(FALSE)
{
}


CopyHandler::~CopyHandler(void)
{
}

BOOL CopyHandler::CopyFileData( AsyncFile& SourceFile, AsyncFile& TargetFile, CPtrArray& BufArray, int nBufSize )
{
    BYTE* pbyFileBuf = new BYTE[nBufSize];
    if( 0 == pbyFileBuf )
        return FALSE;
    BufArray.push_back( pbyFileBuf );
    if( FALSE == SourceFile.Read( pbyFileBuf,nBufSize ))
    {
        m_pProgressNotifier->ShowProgress(_T("Reading-->failed..."));        
        return FALSE;
    }
        
    m_pProgressNotifier->ShowProgress(_T("Reading-->success..."));
    if( FALSE == TargetFile.Write( pbyFileBuf,nBufSize ))
    {
        m_pProgressNotifier->ShowProgress(_T("Writing-->failed..."));
        return FALSE;
    }     
    m_pProgressNotifier->ShowProgress(_T("Writing-->success..."));
    return TRUE;
}

void CopyHandler::CopyFile(LPCTSTR lpctszSourceFile,LPCTSTR lpctszTargetFile) 
{
    CPtrArray ReadBufArray;
    const int CHUNK_SIZE = 32*1024*1024; // 32 MB
    
    AsyncFile SourceFile(lpctszSourceFile,FALSE,GENERIC_READ,FILE_SHARE_READ,this,TRUE,0,TRUE);
    AsyncFile TargetFile(lpctszTargetFile,TRUE,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,this,TRUE,0,TRUE);

    __int64 nFileSize = SourceFile.GetLargeFileLength();
    int nLoopCount = static_cast<int>(nFileSize/CHUNK_SIZE);
    int nRemainingBytes = static_cast<int>(nFileSize%CHUNK_SIZE);

    m_pProgressNotifier->ShowProgress(_T("Copying..."));
    int nIdx = 0; 
    for( nIdx = 0; nIdx < nLoopCount; ++nIdx )
    {
        if( m_bAbort )
        {
           break;
        }
        if( FALSE == CopyFileData( SourceFile,TargetFile,ReadBufArray,CHUNK_SIZE ))
        {
            break;
        }
    }
    if( m_bAbort )
    {
        SourceFile.AbortIO();
        TargetFile.AbortIO();        
    }
    else
    {
        if( nRemainingBytes > 0 )
        {
            CopyFileData( SourceFile,TargetFile,ReadBufArray,nRemainingBytes );
        }    
        if( FALSE == SourceFile.IsAsyncIOComplete())
        {
            m_pProgressNotifier->ShowProgress(_T("Read-->IsAsyncIOComplete failed..."));            
        }
        if( FALSE == TargetFile.IsAsyncIOComplete())
        {
            m_pProgressNotifier->ShowProgress(_T("Write-->IsAsyncIOComplete failed..."));            
        }        
    }
    int nReadBufCount = ReadBufArray.size();
    for( nIdx = 0; nIdx < nReadBufCount; ++nIdx )
    {
        BYTE* pbyFileBuf = static_cast<BYTE*>( ReadBufArray.at( nIdx ));
        delete[] pbyFileBuf;
    }
    ReadBufArray.clear();
}

void CopyHandler::OnAsyncOperationComplete(BOOL bRead,DWORD dwErrorCode)
{
    if( TRUE == bRead )
    {
        m_pProgressNotifier->ShowProgress(_T("OnAsyncOperationComplete->Read"));
    }
    else
    {
        m_pProgressNotifier->ShowProgress(_T("OnAsyncOperationComplete->Write"));
    }    
}

class FileCopyDemo : IAsyncOperationCompletionNotifier,ProgressNotifier
{
public:
	void OnAsyncOperationComplete(BOOL bRead,DWORD dwErrorCode);
	
	void Go(LPCTSTR m_csSourceFilePath, LPCTSTR m_csTargetFilePath)
	{
		CopyHandler CpyHndlrObj(this);
		CpyHndlrObj.CopyFile (m_csSourceFilePath, m_csTargetFilePath);
	}
	
	void ShowProgress(LPCTSTR lpctszMessage);
};

void  FileCopyDemo::OnAsyncOperationComplete(BOOL bRead,DWORD dwErrorCode)
{
    TCHAR tcszMsg[100] = {0};
    if( TRUE == bRead )
    {
        _stprintf( tcszMsg,_T("OnAsyncOperationComplete-->Read, error code:%d"),dwErrorCode);
    }
    else
    {
        _stprintf( tcszMsg,_T("OnAsyncOperationComplete-->Write, error code:%d"),dwErrorCode);
    }
    TRACE_STATUS_TO_LIST( tcszMsg );
}

void FileCopyDemo::ShowProgress(LPCTSTR lpctszMessage)
{
    TRACE_STATUS_TO_LIST( lpctszMessage );
}

int main()
{
	FileCopyDemo fcd;
	
	fcd.Go(_T("AsyncFileDemo.sdf"), _T("AsyncFileDemo.sdf.bak"));

    return 0;
}
