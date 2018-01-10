#include "resource.h"

#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <assert.h>
#include <iostream>

using std::cout;
using std::endl;

#include "RWLockImpl.h"

//创建一个读写锁对象
CMyRWLock g_myRWLock;
volatile int g_counter = 0;

//线程函数
unsigned int __stdcall StartThread(void *pParam)
{
	int id = (int)pParam;

	int lastCount = 0;
	for (int i = 0; i < 10000; ++i)
	{
		g_myRWLock.ReadLock();
		lastCount = g_counter;
		//在读锁域，两个线程不断循环交替访问全局变量g_counter
		cout << "Read in thread " << id << endl;
		for (int k = 0; k < 100; ++k)
		{
			if (g_counter != lastCount)
			{
				cout<<"the value of g_counter has been updated."<<endl;
			}
			Sleep(0);
		}
		g_myRWLock.Unlock();


		g_myRWLock.WriteLock();
		//在写锁域，只有一个线程可以修改全局变量g_counter的值
		cout << "Write in thread " << id << endl;
		for (int k = 0; k < 100; ++k)
		{
			--g_counter;
			Sleep(0);
		}
		for (int k = 0; k < 100; ++k)
		{
			++g_counter;
			Sleep(0);
		}
		++g_counter;
		if (g_counter <= lastCount)
		{
			cout<<"the value of g_counter is error."<<endl;
		}
		g_myRWLock.Unlock();
	}

	return (unsigned int)0;
}

int main(int argc, char* argv[])
{
	HANDLE hThread1, hThread2;
	unsigned int uiThreadId1, uiThreadId2;

	//创建两个工作线程
	hThread1 = (HANDLE)_beginthreadex(NULL, 0, &StartThread, (void *)1, 0, &uiThreadId1);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, &StartThread, (void *)2, 0, &uiThreadId2);

	//等待线程结束
	DWORD dwRet = WaitForSingleObject(hThread1,INFINITE);
	if ( dwRet == WAIT_TIMEOUT )
	{
		TerminateThread(hThread1,0);
	}
	dwRet = WaitForSingleObject(hThread2,INFINITE);
	if ( dwRet == WAIT_TIMEOUT )
	{
		TerminateThread(hThread2,0);
	}

	//关闭线程句柄，释放资源
	CloseHandle(hThread1);
	CloseHandle(hThread2);

	assert (g_counter == 20000);

	system("pause");
	return 0;
}
