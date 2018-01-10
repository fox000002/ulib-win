#include <windows.h>
#include <tchar.h>
#include <objbase.h>
#include <objidl.h>
#include <assert.h>
#include <stdio.h>


#define ASSERT assert

void SampleCreateDoc(const WCHAR* fn)
{


	HRESULT hr;		// ����ִ�з���ֵ
	IStorage *pStg = NULL;	// ���洢�ӿ�ָ��
	IStorage *pSub = NULL;	// �Ӵ洢�ӿ�ָ��
	IStream *pStm = NULL;	// ���ӿ�ָ��

	::CoInitialize(NULL);	// COM ��ʼ��
				// �����MFC���򣬿���ʹ��AfxOleInit()���

	hr = ::StgCreateDocfile(	// ���������ļ�
		fn,	// �ļ�����
		STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE,	// �򿪷�ʽ
		0,		// ��������
		&pStg);		// ȡ�ø��洢�ӿ�ָ��
	ASSERT( SUCCEEDED(hr) );	// Ϊ��ͻ���ص㣬�򻯳���ṹ������ʹ���˶��ԡ�
				// ��ʵ�ʵĳ�������Ҫʹ�������жϺ��쳣����

	hr = pStg->CreateStorage(	// �����Ӵ洢
		L"SubStg",	// �Ӵ洢����
		STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE,
		0,0,
		&pSub);		// ȡ���Ӵ洢�ӿ�ָ��
	ASSERT( SUCCEEDED(hr) );

	hr = pSub->CreateStream(	// ������
		L"Stm",		// ������
		STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE,
		0,0,
		&pStm);		// ȡ�����ӿ�ָ��
	ASSERT( SUCCEEDED(hr) );

	hr = pStm->Write(		// ������д������
		"Hello",		// ���ݵ�ַ
		5,		// �ֽڳ���(ע�⣬û��д���ַ�����β��\0)
		NULL);		// ����Ҫ�õ�ʵ��д����ֽڳ���
	ASSERT( SUCCEEDED(hr) );

	if( pStm )	pStm->Release();// �ͷ���ָ��
	if( pSub )	pSub->Release();// �ͷ��Ӵ洢ָ��
	if( pStg )	pStg->Release();// �ͷŸ��洢ָ��

	::CoUninitialize();		// COM �ͷ�
				// ���ʹ�� AfxOleInit(),�򲻵��øú���
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
{	// �������Ѿ����� COM ��ʼ����

	//LPCTSTR lpFileName = _T( "c:\\a.stg" );
	HRESULT hr;
	IStorage *pStg = NULL;
	
	//USES_CONVERSION;				// ��ע6��
	//LPCOLESTR lpwFileName = T2COLE( lpFileName );	// ת��T����Ϊ���ַ�
	hr = ::StgIsStorageFile( fn );	// �Ǹ����ļ���
	if( FAILED(hr) )	return;

	hr = ::StgOpenStorage(			// �򿪸����ļ�
		fn,			// �ļ�����
		NULL,
		STGM_READ | STGM_SHARE_DENY_WRITE,
		0,
		0,
		&pStg);				// �õ����洢�ӿ�ָ��

	IEnumSTATSTG *pEnum=NULL;	// ö����
	hr = pStg->EnumElements( 0, NULL, 0, &pEnum );
	ASSERT( SUCCEEDED(hr) );

	STATSTG statstg;
	while( NOERROR == pEnum->Next( 1, &statstg, NULL) )
	{
		// statstg.type �����Ŷ������� STGTY_STREAM �� STGTY_STORAGE
		// statstg.pwcsName �����Ŷ�������
		// ...... ����ʱ�䣬���ȵȺܶ���Ϣ����鿴 MSDN

		printf("name : %s\n", statstg.pwcsName);
		printf("type : %d\n", statstg.type);
		//printf("")

		::CoTaskMemFree( statstg.pwcsName );	// �ͷ�������ʹ�õ��ڴ棨ע6��
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
