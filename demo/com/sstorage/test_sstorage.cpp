#include <windows.h>
#include <tchar.h>
#include <objbase.h>
#include <objidl.h>
#include <assert.h>
#include <stdio.h>


#define ASSERT assert

void SampleCreateDoc(const WCHAR* fn)
{


	HRESULT hr;		// 函数执行返回值
	IStorage *pStg = NULL;	// 根存储接口指针
	IStorage *pSub = NULL;	// 子存储接口指针
	IStream *pStm = NULL;	// 流接口指针

	::CoInitialize(NULL);	// COM 初始化
				// 如果是MFC程序，可以使用AfxOleInit()替代

	hr = ::StgCreateDocfile(	// 建立复合文件
		fn,	// 文件名称
		STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE,	// 打开方式
		0,		// 保留参数
		&pStg);		// 取得根存储接口指针
	ASSERT( SUCCEEDED(hr) );	// 为了突出重点，简化程序结构，所以使用了断言。
				// 在实际的程序中则要使用条件判断和异常处理

	hr = pStg->CreateStorage(	// 建立子存储
		L"SubStg",	// 子存储名称
		STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE,
		0,0,
		&pSub);		// 取得子存储接口指针
	ASSERT( SUCCEEDED(hr) );

	hr = pSub->CreateStream(	// 建立流
		L"Stm",		// 流名称
		STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE,
		0,0,
		&pStm);		// 取得流接口指针
	ASSERT( SUCCEEDED(hr) );

	hr = pStm->Write(		// 向流中写入数据
		"Hello",		// 数据地址
		5,		// 字节长度(注意，没有写入字符串结尾的\0)
		NULL);		// 不需要得到实际写入的字节长度
	ASSERT( SUCCEEDED(hr) );

	if( pStm )	pStm->Release();// 释放流指针
	if( pSub )	pSub->Release();// 释放子存储指针
	if( pStg )	pStg->Release();// 释放根存储指针

	::CoUninitialize();		// COM 释放
				// 如果使用 AfxOleInit(),则不调用该函数
}

/*
typedef struct tagSTATSTG {
  LPOLESTR       pwcsName;
  DWORD          type;
  ULARGE_INTEGER cbSize;
  FILETIME       mtime;
  FILETIME       ctime;
  FILETIME       atime;
  DWORD          grfMode;
  DWORD          grfLocksSupported;
  CLSID          clsid;
  DWORD          grfStateBits;
  DWORD          reserved;
} STATSTG;
*/

void SampleEnum(const WCHAR* fn) 
{	// 假设你已经做过 COM 初始化了

	//LPCTSTR lpFileName = _T( "c:\\a.stg" );
	HRESULT hr;
	IStorage *pStg = NULL;
	
	//USES_CONVERSION;				// （注6）
	//LPCOLESTR lpwFileName = T2COLE( lpFileName );	// 转换T类型为宽字符
	hr = ::StgIsStorageFile( fn );	// 是复合文件吗？
	if( FAILED(hr) )	return;

	hr = ::StgOpenStorage(			// 打开复合文件
		fn,			// 文件名称
		NULL,
		STGM_READ | STGM_SHARE_DENY_WRITE,
		0,
		0,
		&pStg);				// 得到根存储接口指针

	IEnumSTATSTG *pEnum=NULL;	// 枚举器
	hr = pStg->EnumElements( 0, NULL, 0, &pEnum );
	ASSERT( SUCCEEDED(hr) );

	STATSTG statstg;
	while( NOERROR == pEnum->Next( 1, &statstg, NULL) )
	{
		// statstg.type 保存着对象类型 STGTY_STREAM 或 STGTY_STORAGE
		// statstg.pwcsName 保存着对象名称
		// ...... 还有时间，长度等很多信息。请查看 MSDN

		printf("name : %s\n", statstg.pwcsName);
		printf("type : %d\n", statstg.type);
		//printf("")

		::CoTaskMemFree( statstg.pwcsName );	// 释放名称所使用的内存（注6）
	}
	
	if( pEnum )	pEnum->Release();
	if( pStg )	pStg->Release();
}

int main(int argc, char **argv)
{
	SampleCreateDoc(L"hello.stg");
	
	SampleEnum(L"hello.stg");

    return 0;
}
