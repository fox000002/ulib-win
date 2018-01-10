#include <windows.h>
#include <tchar.h>
#include <exdisp.h>

#ifdef  __GNUC__
//CLSID_InternetExplorer : 0002DF01-0000-0000-C000-000000000046
#define   INITGUID
#include <initguid.h>
DEFINE_GUID(CLSID_InternetExplorer, 0x0002DF01, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
#endif /* __GNUC__ */

int main()
{
	if (SUCCEEDED(OleInitialize(NULL)))
	{
		IWebBrowser2 * pBrowser2;
		
		CoCreateInstance(CLSID_InternetExplorer, (LPUNKNOWN)NULL, CLSCTX_LOCAL_SERVER,
			IID_IWebBrowser2, (void **) &pBrowser2);
			
		if (pBrowser2)
		{
			VARIANT vEmpty;
			VariantInit(&vEmpty);
		
			VARIANT vFlags;
			V_VT(&vFlags) = VT_I4;
			V_I4(&vFlags) = navOpenInNewWindow;
		
			BSTR bstrURL = SysAllocString(L"file:///F:/ulib-win/demo/com/launchie/index.html");
			
			HRESULT hr = pBrowser2->Navigate(bstrURL, &vEmpty, &vEmpty, &vEmpty, &vEmpty);
			if (SUCCEEDED(hr))
			{
				pBrowser2->put_Visible(VARIANT_TRUE);
			}
			else
			{
				pBrowser2->Quit();
			}
			
			hr = pBrowser2->Navigate(bstrURL, &vFlags, &vEmpty, &vEmpty, &vEmpty);
			
			SysFreeString(bstrURL);
			pBrowser2->Release();
		}
	
		OleUninitialize();
	}

	return 0;
}
